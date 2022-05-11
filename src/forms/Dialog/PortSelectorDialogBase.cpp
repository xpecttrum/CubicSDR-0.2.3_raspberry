///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 27 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "PortSelectorDialogBase.h"

///////////////////////////////////////////////////////////////////////////

PortSelectorDialogBase::PortSelectorDialogBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* dlgSizer;
	dlgSizer = new wxBoxSizer(wxVERTICAL);

	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Select a detected port or enter your own"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	dlgSizer->Add(m_staticText1, 0, wxEXPAND | wxALL, 5);

	m_portList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	dlgSizer->Add(m_portList, 1, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);

	bSizer3->SetMinSize(wxSize(-1, 30));
	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	bSizer3->Add(m_staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_portSelection = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(m_portSelection, 1, wxEXPAND | wxRIGHT, 5);


	dlgSizer->Add(bSizer3, 1, wxEXPAND, 5);

	m_buttonPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	m_cancelButton = new wxButton(m_buttonPanel, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_cancelButton, 0, wxALL | wxALIGN_BOTTOM, 5);


	bSizer2->Add(0, 0, 1, wxEXPAND, 5);

	m_okButton = new wxButton(m_buttonPanel, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_okButton, 0, wxALL | wxALIGN_BOTTOM, 5);


	m_buttonPanel->SetSizer(bSizer2);
	m_buttonPanel->Layout();
	bSizer2->Fit(m_buttonPanel);
	dlgSizer->Add(m_buttonPanel, 0, wxEXPAND | wxALL, 5);


	this->SetSizer(dlgSizer);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	m_portList->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(PortSelectorDialogBase::onListSelect), NULL, this);
	m_cancelButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PortSelectorDialogBase::onCancelButton), NULL, this);
	m_okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PortSelectorDialogBase::onOKButton), NULL, this);
}

PortSelectorDialogBase::~PortSelectorDialogBase()
{
	// Disconnect Events
	m_portList->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(PortSelectorDialogBase::onListSelect), NULL, this);
	m_cancelButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PortSelectorDialogBase::onCancelButton), NULL, this);
	m_okButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PortSelectorDialogBase::onOKButton), NULL, this);

}
