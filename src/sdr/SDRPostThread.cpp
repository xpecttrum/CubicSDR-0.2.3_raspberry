// Copyright (c) Charles J. Cliffe
// SPDX-License-Identifier: GPL-2.0+

#include "SDRPostThread.h"
#include "CubicSDRDefs.h"
#include "CubicSDR.h"

#include <vector>
#include <deque>
#include <memory>

//50 ms
#define HEARTBEAT_CHECK_PERIOD_MICROS (50 * 1000) 

SDRPostThread::SDRPostThread() : IOThread(), buffers("SDRPostThreadBuffers"), visualDataBuffers("SDRPostThreadVisualDataBuffers"), frequency(0) {
    iqDataInQueue = nullptr;
    iqDataOutQueue = nullptr;
    iqVisualQueue = nullptr;

    numChannels = 0;
    channelizer = nullptr;
    
    sampleRate = 0;
    
    visFrequency.store(0);
    visBandwidth.store(0);
    
    doRefresh.store(false);
    dcFilter = iirfilt_crcf_create_dc_blocker(0.0005f);
}

SDRPostThread::~SDRPostThread() {
}

void SDRPostThread::notifyDemodulatorsChanged() {
  
    doRefresh.store(true);
   
}

void SDRPostThread::initPFBChannelizer() {
//    std::cout << "Initializing post-process FIR polyphase filterbank channelizer with " << numChannels << " channels." << std::endl;
    if (channelizer) {
        firpfbch_crcf_destroy(channelizer);
    }
    channelizer = firpfbch_crcf_create_kaiser(LIQUID_ANALYZER, numChannels, 4, 60);
    
    chanBw = (sampleRate / numChannels);
    
    chanCenters.resize(numChannels+1);
    demodChannelActive.resize(numChannels+1);
    
//    std::cout << "Channel bandwidth spacing: " << (chanBw) << std::endl;
}

void SDRPostThread::updateActiveDemodulators() {
    // In range?
   
    runDemods.clear();
    demodChannel.clear();

    long long centerFreq = wxGetApp().getFrequency();

    //retreive the current list of demodulators:
    auto demodulators = wxGetApp().getDemodMgr().getDemodulators();
   
    for (auto demod : demodulators) {
            
        // not in range?
        if (demod->isDeltaLock()) {
            if (demod->getFrequency() != centerFreq + demod->getDeltaLockOfs()) {
                demod->setFrequency(centerFreq + demod->getDeltaLockOfs());
                demod->updateLabel(demod->getFrequency());
                demod->setFollow(false);
                demod->setTracking(false);
            }
        }
        
        if (abs(frequency - demod->getFrequency()) > (sampleRate / 2)) {
            // deactivate if active
           
            if (wxGetApp().getDemodMgr().getLastActiveDemodulator() == demod) {

                demod->setActive(false);
            }
            else if (demod->isActive() && !demod->isFollow() && !demod->isTracking()) {
                demod->setActive(false);
            } 
            
            // follow if follow mode
            if (demod->isFollow() && centerFreq != demod->getFrequency()) {
                wxGetApp().setFrequency(demod->getFrequency());
                demod->setFollow(false);
            }
        } else if (!demod->isActive()) { // in range, activate if not activated
            demod->setActive(true);
            if (wxGetApp().getDemodMgr().getLastActiveDemodulator() == nullptr) {

                wxGetApp().getDemodMgr().setActiveDemodulator(demod);
            }
        }
        
        if (!demod->isActive()) {
            continue;
        }
        
        // Add active demods to the current run:

        runDemods.push_back(demod);
        demodChannel.push_back(-1);
    }
}

void SDRPostThread::resetAllDemodulators() {
    
    //retreive the current list of demodulators:
    auto demodulators = wxGetApp().getDemodMgr().getDemodulators();

    for (auto demod : demodulators) {

        demod->setActive(false);
        demod->getIQInputDataPipe()->flush();
    }

    doRefresh = true;
}

void SDRPostThread::updateChannels() {
    // calculate channel center frequencies, todo: cache
    for (int i = 0; i < numChannels/2; i++) {
        int ofs = ((chanBw) * i);
        chanCenters[i] = frequency + ofs;
        chanCenters[i+(numChannels/2)] = frequency - (sampleRate/2) + ofs;
    }
    chanCenters[numChannels] = frequency + (sampleRate/2);
}

int SDRPostThread::getChannelAt(long long frequency) {
    int chan = -1;
    long long minDelta = sampleRate;
    for (int i = 0; i < numChannels+1; i++) {
        long long fdelta = abs(frequency - chanCenters[i]);
        if (fdelta < minDelta) {
            minDelta = fdelta;
            chan = i;
        }
    }
    return chan;
}

void SDRPostThread::setIQVisualRange(long long frequency, int bandwidth) {
    visFrequency.store(frequency);
    visBandwidth.store(bandwidth);
}

void SDRPostThread::run() {
#ifdef __APPLE__
    pthread_t tID = pthread_self();  // ID of this thread
    int priority = sched_get_priority_max( SCHED_FIFO);
    sched_param prio = {priority}; // scheduling priority of thread
    pthread_setschedparam(tID, SCHED_FIFO, &prio);
#endif

//    std::cout << "SDR post-processing thread started.." << std::endl;

    iqDataInQueue = std::static_pointer_cast<SDRThreadIQDataQueue>(getInputQueue("IQDataInput"));
    iqDataOutQueue = std::static_pointer_cast<DemodulatorThreadInputQueue>(getOutputQueue("IQDataOutput"));
    iqVisualQueue = std::static_pointer_cast<DemodulatorThreadInputQueue>(getOutputQueue("IQVisualDataOutput"));
    iqActiveDemodVisualQueue = std::static_pointer_cast<DemodulatorThreadInputQueue>(getOutputQueue("IQActiveDemodVisualDataOutput"));
    
    while (!stopping) {
        SDRThreadIQDataPtr data_in;
        
        if (!iqDataInQueue->pop(data_in, HEARTBEAT_CHECK_PERIOD_MICROS)) {
            continue;
        }
         
        bool doUpdate = false;

        if (data_in && data_in->data.size()) {
            if(data_in->numChannels > 1) {
                runPFBCH(data_in.get());
            } else {
                runSingleCH(data_in.get());
            }
        }
        
        for (size_t j = 0; j < runDemods.size(); j++) {
            DemodulatorInstancePtr demod = runDemods[j];
            if (abs(frequency - demod->getFrequency()) > (sampleRate / 2)) {
                doUpdate = true;
            }
        }
        
        //Only update the list of demodulators here
        if (doUpdate || doRefresh) {
            updateActiveDemodulators();
        }
    } //end while
    
    //Be safe, remove as many elements as possible
    iqVisualQueue->flush();   
    iqDataInQueue->flush();
    iqDataOutQueue->flush();
    iqActiveDemodVisualQueue->flush();

//    std::cout << "SDR post-processing thread done." << std::endl;
}

void SDRPostThread::terminate() {
    IOThread::terminate();
    //unblock push()
    iqVisualQueue->flush();
    iqDataInQueue->flush();
    iqDataOutQueue->flush();
    iqActiveDemodVisualQueue->flush();
}

void SDRPostThread::runSingleCH(SDRThreadIQData *data_in) {
    if (sampleRate != data_in->sampleRate) {
        sampleRate = data_in->sampleRate;
        numChannels = 1;
        doRefresh.store(true);
    }
    
    size_t dataSize = data_in->data.size();
    size_t outSize = data_in->data.size();
    
    if (outSize > dataOut.capacity()) {
        dataOut.reserve(outSize);
    }
    if (outSize != dataOut.size()) {
        dataOut.resize(outSize);
    }
    
    if (frequency != data_in->frequency) {
        frequency = data_in->frequency;
        doRefresh.store(true);
    }
    
    if (doRefresh.load()) {
        updateActiveDemodulators();
        doRefresh.store(false);
    }
    
    size_t refCount = runDemods.size();
    bool doIQDataOut = (iqDataOutQueue != nullptr && !iqDataOutQueue->full());
    bool doDemodVisOut = (runDemods.size() > 0 && iqActiveDemodVisualQueue != nullptr && !iqActiveDemodVisualQueue->full());
    bool doVisOut = (iqVisualQueue != nullptr && !iqVisualQueue->full());
    
    if (doIQDataOut) {
        refCount++;
    }
    if (doDemodVisOut) {
        refCount++;
    }
    if (doVisOut) {
        refCount++;
    }
    
    if (refCount) {
        DemodulatorThreadIQDataPtr demodDataOut = buffers.getBuffer();

        demodDataOut->frequency = frequency;
        demodDataOut->sampleRate = sampleRate;
        
        if (demodDataOut->data.size() != dataSize) {
            if (demodDataOut->data.capacity() < dataSize) {
                demodDataOut->data.reserve(dataSize);
            }
            demodDataOut->data.resize(dataSize);
        }
        
        iirfilt_crcf_execute_block(dcFilter, &data_in->data[0], dataSize, &demodDataOut->data[0]);

        if (doDemodVisOut) {
            //VSO: blocking push
            iqActiveDemodVisualQueue->push(demodDataOut);
        }
        
        if (doIQDataOut) {
            //VSO: blocking push
            iqDataOutQueue->push(demodDataOut);
        }

        if (doVisOut) {
            //VSO: blocking push
            iqVisualQueue->push(demodDataOut);
        }
        
        for (size_t i = 0; i < runDemods.size(); i++) {
            // try-push() : we do our best to only stimulate active demods, but some could happen to be dead, full, or indeed non-active.
            //so in short never block here no matter what.
            if (!runDemods[i]->getIQInputDataPipe()->try_push(demodDataOut)) {

             //   std::cout << "SDRPostThread::runSingleCH() attempt to push into demod '" << runDemods[i]->getLabel()
             //       << "' (" << runDemods[i]->getFrequency() << " Hz) failed, demod is either too busy, not-active, or dead..." << std::endl << std::flush;
                std::this_thread::yield();
            }
        }
    }
}

void SDRPostThread::runPFBCH(SDRThreadIQData *data_in) {
    if (numChannels != data_in->numChannels || sampleRate != data_in->sampleRate) {
        numChannels = data_in->numChannels;
        sampleRate = data_in->sampleRate;
        initPFBChannelizer();
        doRefresh.store(true);
    }
    
    size_t dataSize = data_in->data.size();
    size_t outSize = data_in->data.size();
    
    if (outSize > dataOut.capacity()) {
        dataOut.reserve(outSize);
    }
    if (outSize != dataOut.size()) {
        dataOut.resize(outSize);
    }
    
    if (iqDataOutQueue != nullptr && !iqDataOutQueue->full()) {
        DemodulatorThreadIQDataPtr iqDataOut = visualDataBuffers.getBuffer();
        
        bool doVis = false;
        
        if (iqVisualQueue != nullptr && !iqVisualQueue->full()) {
            doVis = true;
        }
        
        iqDataOut->frequency = data_in->frequency;
        iqDataOut->sampleRate = data_in->sampleRate;
        iqDataOut->data.assign(data_in->data.begin(), data_in->data.begin() + dataSize);
        
        //VSO: blocking push
        iqDataOutQueue->push(iqDataOut);
   
        if (doVis) {
            //VSO: blocking push
            iqVisualQueue->push(iqDataOut);
        }
    }
    
    if (frequency != data_in->frequency) {
        frequency = data_in->frequency;
        doRefresh.store(true);
    }
    
    if (doRefresh.load()) {
        updateActiveDemodulators();
        updateChannels();
        doRefresh.store(false);
    }
    
    DemodulatorInstancePtr activeDemod = wxGetApp().getDemodMgr().getLastActiveDemodulator();
    int activeDemodChannel = -1;
    
    // Find active demodulators
    if (runDemods.size() > 0) {
        
        // channelize data
        // firpfbch output rate is (input rate / channels)
        for (int i = 0, iMax = dataSize; i < iMax; i+=numChannels) {
            firpfbch_crcf_analyzer_execute(channelizer, &data_in->data[i], &dataOut[i]);
        }
        
        for (int i = 0, iMax = numChannels+1; i < iMax; i++) {
            demodChannelActive[i] = 0;
        }
        
        // Find nearest channel for each demodulator
        for (size_t i = 0; i < runDemods.size(); i++) {
            DemodulatorInstancePtr demod = runDemods[i];
            demodChannel[i] = getChannelAt(demod->getFrequency());
            if (demod == activeDemod) {
                activeDemodChannel = demodChannel[i];
            }
        }
        
        for (size_t i = 0; i < runDemods.size(); i++) {
            // cache channel usage refcounts
            if (demodChannel[i] >= 0) {
                demodChannelActive[demodChannel[i]]++;
            }
        }
        
        // Run channels
        for (int i = 0; i < numChannels+1; i++) {
            int doDemodVis = ((activeDemodChannel == i) && (iqActiveDemodVisualQueue != nullptr) && !iqActiveDemodVisualQueue->full())?1:0;
            
            if (!doDemodVis && demodChannelActive[i] == 0) {
                continue;
            }
            
            DemodulatorThreadIQDataPtr demodDataOut = buffers.getBuffer();
            demodDataOut->frequency = chanCenters[i];
            demodDataOut->sampleRate = chanBw;
            
            // Calculate channel buffer size
            size_t chanDataSize = (outSize/numChannels);
            
            if (demodDataOut->data.size() != chanDataSize) {
                if (demodDataOut->data.capacity() < chanDataSize) {
                    demodDataOut->data.reserve(chanDataSize);
                }
                demodDataOut->data.resize(chanDataSize);
            }
            
            int idx = i;
            
            // Extra channel wraps lower side band of lowest channel
            // to fix frequency gap on upper side of spectrum
            if (i == numChannels) {
                idx = (numChannels/2);
            }
            
            // prepare channel data buffer
            if (i == 0) {   // Channel 0 requires DC correction
                if (dcBuf.size() != chanDataSize) {
                    dcBuf.resize(chanDataSize);
                }
                for (size_t j = 0; j < chanDataSize; j++) {
                    dcBuf[j] = dataOut[idx];
                    idx += numChannels;
                }
                iirfilt_crcf_execute_block(dcFilter, &dcBuf[0], chanDataSize, &demodDataOut->data[0]);
            } else {
                for (size_t j = 0; j < chanDataSize; j++) {
                    demodDataOut->data[j] = dataOut[idx];
                    idx += numChannels;
                }
            }
            
            if (doDemodVis) {
                //VSO: blocking push
                iqActiveDemodVisualQueue->push(demodDataOut);
            }
            
            for (size_t j = 0; j < runDemods.size(); j++) {
                if (demodChannel[j] == i) {
                    
                    // try-push() : we do our best to only stimulate active demods, but some could happen to be dead, full, or indeed non-active.
                    //so in short never block here no matter what.
                    if (!runDemods[j]->getIQInputDataPipe()->try_push(demodDataOut)) {
                    //    std::cout << "SDRPostThread::runPFBCH() attempt to push into demod '" << runDemods[i]->getLabel()
                    //        << "' (" << runDemods[i]->getFrequency() << " Hz) failed, demod is either too busy, not-active, or dead..." << std::endl << std::flush;
                        std::this_thread::yield();
                    }
                }
            } //end for
        }
    }
}
