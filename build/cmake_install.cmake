# Install script for directory: /home/pi/CubicSDR-0.2.3

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/CubicSDR" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/CubicSDR")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/CubicSDR"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/pi/CubicSDR-0.2.3/build/x86/CubicSDR")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/CubicSDR" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/CubicSDR")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/CubicSDR"
         OLD_RPATH "/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/CubicSDR")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cubicsdr" TYPE FILE FILES "/home/pi/CubicSDR-0.2.3/src/CubicSDR.png")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cubicsdr/fonts" TYPE FILE FILES
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono12.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono16.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono18.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono24.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono27.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono32.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono36.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono48.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono64.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono72.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono96.fnt"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono12_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono16_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono18_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono24_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono27_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono32_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono36_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono48_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono64_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono72_0.png"
    "/home/pi/CubicSDR-0.2.3/font/vera_sans_mono96_0.png"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications" TYPE FILE FILES "/home/pi/CubicSDR-0.2.3/build/CubicSDR.desktop")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/pi/CubicSDR-0.2.3/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
