///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-85-gdf26f269)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MyProjectBase.h"

///////////////////////////////////////////////////////////////////////////

MyFrame1::MyFrame1( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 600,600 ), wxSize( 600,600 ) );

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 0, 2, 0, 0 );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_canvasPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 240,320 ), wxTAB_TRAVERSAL );
	m_canvasPanel->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	m_canvasPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW ) );
	m_canvasPanel->SetMinSize( wxSize( 240,320 ) );
	m_canvasPanel->SetMaxSize( wxSize( 240,320 ) );

	bSizer1->Add( m_canvasPanel, 1, wxEXPAND | wxALL, 20 );

	m_staticText12 = new wxStaticText( this, wxID_ANY, _("Current Images:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer1->Add( m_staticText12, 0, wxALL, 5 );

	wxArrayString currentImagesCheckListChoices;
	currentImagesCheckList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, currentImagesCheckListChoices, 0 );
	bSizer1->Add( currentImagesCheckList, 0, wxALL|wxEXPAND, 5 );

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 0, 5, 0, 0 );

	List_UP = new wxButton( this, wxID_ANY, _("^"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	gSizer4->Add( List_UP, 0, wxALL, 5 );

	List_DOWN = new wxButton( this, wxID_ANY, _("v"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	gSizer4->Add( List_DOWN, 0, wxALL, 5 );

	List_BIND = new wxButton( this, wxID_ANY, _("BIND"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	gSizer4->Add( List_BIND, 0, wxALL, 5 );

	List_NEWIMAGE = new wxButton( this, wxID_ANY, _("NEW"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	gSizer4->Add( List_NEWIMAGE, 0, wxALL, 5 );

	List_DELETEIMAGE = new wxButton( this, wxID_ANY, _("DEL"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	gSizer4->Add( List_DELETEIMAGE, 0, wxALL, 5 );


	bSizer1->Add( gSizer4, 1, wxEXPAND, 5 );

	nameTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !nameTextCtrl->HasFlag( wxTE_MULTILINE ) )
	{
	nameTextCtrl->SetMaxLength( 31 );
	}
	#else
	nameTextCtrl->SetMaxLength( 31 );
	#endif
	bSizer1->Add( nameTextCtrl, 0, wxALL|wxEXPAND, 5 );


	gSizer2->Add( bSizer1, 1, wxEXPAND, 5 );

	bSizer5 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 2, 0, 0 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	IDText = new wxStaticText( this, wxID_ANY, _("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	IDText->Wrap( -1 );
	bSizer3->Add( IDText, 0, wxALL, 2 );

	Description = new wxStaticText( this, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	Description->Wrap( 0 );
	Description->SetFont( wxFont( 7, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_LIGHT, false, wxEmptyString ) );

	bSizer3->Add( Description, 0, wxALL, 5 );


	gSizer3->Add( bSizer3, 1, wxEXPAND, 5 );

	IDCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 125,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	gSizer3->Add( IDCtrl, 0, wxALL, 5 );

	CountText = new wxStaticText( this, wxID_ANY, _("Count"), wxDefaultPosition, wxDefaultSize, 0 );
	CountText->Wrap( -1 );
	gSizer3->Add( CountText, 0, wxALL, 5 );

	CountControl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxPoint( -1,-1 ), wxSize( 125,-1 ), wxSP_ARROW_KEYS, 1, 255, 1 );
	gSizer3->Add( CountControl, 0, wxALL, 5 );

	IndexText = new wxStaticText( this, wxID_ANY, _("Index"), wxDefaultPosition, wxDefaultSize, 0 );
	IndexText->Wrap( -1 );
	gSizer3->Add( IndexText, 0, wxALL, 5 );

	IndexControl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 125,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	gSizer3->Add( IndexControl, 0, wxALL, 5 );

	XposText = new wxStaticText( this, wxID_ANY, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
	XposText->Wrap( -1 );
	gSizer3->Add( XposText, 0, wxALL, 5 );

	XposControl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 125,-1 ), wxSP_ARROW_KEYS, 0, 240, 0 );
	gSizer3->Add( XposControl, 0, wxALL, 5 );

	YposText = new wxStaticText( this, wxID_ANY, _("Y"), wxDefaultPosition, wxDefaultSize, 0 );
	YposText->Wrap( -1 );
	gSizer3->Add( YposText, 0, wxALL, 5 );

	YposControl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 125,-1 ), wxSP_ARROW_KEYS, 0, 320, 0 );
	gSizer3->Add( YposControl, 0, wxALL, 5 );

	WidthText = new wxStaticText( this, wxID_ANY, _("Width"), wxDefaultPosition, wxDefaultSize, 0 );
	WidthText->Wrap( -1 );
	gSizer3->Add( WidthText, 0, wxALL, 5 );

	WidthControl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 125,-1 ), wxSP_ARROW_KEYS, 1, 400, 1 );
	gSizer3->Add( WidthControl, 0, wxALL, 5 );

	HeightText = new wxStaticText( this, wxID_ANY, _("Height"), wxDefaultPosition, wxDefaultSize, 0 );
	HeightText->Wrap( -1 );
	gSizer3->Add( HeightText, 0, wxALL, 5 );

	HeightControl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 125,-1 ), wxSP_ARROW_KEYS, 1, 400, 1 );
	gSizer3->Add( HeightControl, 0, wxALL, 5 );

	includeInExport = new wxCheckBox( this, wxID_ANY, _("Include In Export"), wxDefaultPosition, wxDefaultSize, 0 );
	includeInExport->SetValue(true);
	gSizer3->Add( includeInExport, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	transpSlider = new wxSlider( this, wxID_ANY, 255, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	transpSlider->Enable( false );

	gSizer3->Add( transpSlider, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	bSizer5->Add( gSizer3, 1, wxEXPAND, 1 );

	wxGridSizer* gSizer31;
	gSizer31 = new wxGridSizer( 0, 3, 0, 0 );

	ColorPicker = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 0, 0, 0 ), wxPoint( -1,-1 ), wxSize( 60,-1 ), wxCLRP_DEFAULT_STYLE );
	gSizer31->Add( ColorPicker, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	TransCBox = new wxCheckBox( this, wxID_ANY, _("Transparent?"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( TransCBox, 0, wxALL, 5 );

	RGBStatus = new wxStaticText( this, wxID_ANY, _("RGB888 -> RGB565"), wxDefaultPosition, wxDefaultSize, 0 );
	RGBStatus->Wrap( -1 );
	bSizer4->Add( RGBStatus, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	gSizer31->Add( bSizer4, 1, wxALIGN_CENTER, 5 );

	m_button1 = new wxButton( this, wxID_ANY, _("PICK"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_button1->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	gSizer31->Add( m_button1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 30 );

	bakeButton = new wxButton( this, wxID_ANY, _("Bake Image"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer31->Add( bakeButton, 0, wxALIGN_CENTER|wxALL, 5 );

	hardBakeButton = new wxButton( this, wxID_ANY, _("Hard Bake"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer31->Add( hardBakeButton, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer5->Add( gSizer31, 1, wxEXPAND, 5 );


	gSizer2->Add( bSizer5, 1, wxEXPAND, 5 );


	this->SetSizer( gSizer2 );
	this->Layout();
	m_menubar1 = new wxMenuBar( 0 );
	MenuBar = new wxMenu();
	wxMenuItem* openItem;
	openItem = new wxMenuItem( MenuBar, wxID_ANY, wxString( _("Open") ) , wxEmptyString, wxITEM_NORMAL );
	MenuBar->Append( openItem );

	MenuBar->AppendSeparator();

	wxMenuItem* saveItem;
	saveItem = new wxMenuItem( MenuBar, wxID_ANY, wxString( _("Save") ) , wxEmptyString, wxITEM_NORMAL );
	MenuBar->Append( saveItem );

	wxMenuItem* saveAsItem;
	saveAsItem = new wxMenuItem( MenuBar, wxID_ANY, wxString( _("Save as...") ) , wxEmptyString, wxITEM_NORMAL );
	MenuBar->Append( saveAsItem );

	MenuBar->AppendSeparator();

	wxMenuItem* ImportItem;
	ImportItem = new wxMenuItem( MenuBar, wxID_ANY, wxString( _("Import RES...") ) , wxEmptyString, wxITEM_NORMAL );
	MenuBar->Append( ImportItem );

	wxMenuItem* ExportItem;
	ExportItem = new wxMenuItem( MenuBar, wxID_ANY, wxString( _("Export RES...") ) , wxEmptyString, wxITEM_NORMAL );
	MenuBar->Append( ExportItem );

	wxMenuItem* ExportPNG;
	ExportPNG = new wxMenuItem( MenuBar, wxID_ANY, wxString( _("Export PNG...") ) , wxEmptyString, wxITEM_NORMAL );
	MenuBar->Append( ExportPNG );

	wxMenuItem* ExportAllPNG;
	ExportAllPNG = new wxMenuItem( MenuBar, wxID_ANY, wxString( _("Export all PNGs...") ) , wxEmptyString, wxITEM_NORMAL );
	MenuBar->Append( ExportAllPNG );

	MenuBar->AppendSeparator();

	wxMenuItem* ExitItem;
	ExitItem = new wxMenuItem( MenuBar, wxID_ANY, wxString( _("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	MenuBar->Append( ExitItem );

	m_menubar1->Append( MenuBar, _("File") );

	this->SetMenuBar( m_menubar1 );


	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MyFrame1::OnClose ) );
	currentImagesCheckList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( MyFrame1::OnImagesListSelect ), NULL, this );
	currentImagesCheckList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( MyFrame1::OnImagesListToggle ), NULL, this );
	List_UP->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnClkUpList ), NULL, this );
	List_DOWN->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnClkDownList ), NULL, this );
	List_BIND->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnClkBindList ), NULL, this );
	List_NEWIMAGE->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnClkNewList ), NULL, this );
	List_DELETEIMAGE->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnClkDelList ), NULL, this );
	nameTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MyFrame1::OnTextChange ), NULL, this );
	IDCtrl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MyFrame1::OnUpdateID ), NULL, this );
	CountControl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MyFrame1::OnUpdateCount ), NULL, this );
	IndexControl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MyFrame1::OnUpdateIndex ), NULL, this );
	XposControl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MyFrame1::OnUpdateX ), NULL, this );
	YposControl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MyFrame1::OnUpdateY ), NULL, this );
	WidthControl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MyFrame1::OnUpdateWidth ), NULL, this );
	HeightControl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MyFrame1::OnUpdateHeight ), NULL, this );
	includeInExport->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame1::OnIncludeInFileBox ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	transpSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( MyFrame1::OnTranspSlider ), NULL, this );
	ColorPicker->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( MyFrame1::OnTransColorChanged ), NULL, this );
	TransCBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame1::OnUpdateTrans ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnClickPICK ), NULL, this );
	bakeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBakeImage ), NULL, this );
	hardBakeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnHardBake ), NULL, this );
	MenuBar->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MyFrame1::OnOpen ), this, openItem->GetId());
	MenuBar->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MyFrame1::OnSave ), this, saveItem->GetId());
	MenuBar->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MyFrame1::OnSaveAs ), this, saveAsItem->GetId());
	MenuBar->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MyFrame1::OnImport ), this, ImportItem->GetId());
	MenuBar->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MyFrame1::OnExport ), this, ExportItem->GetId());
	MenuBar->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MyFrame1::OnExportPNG ), this, ExportPNG->GetId());
	MenuBar->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MyFrame1::OnExportAllPNG ), this, ExportAllPNG->GetId());
	MenuBar->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MyFrame1::OnExitMenu ), this, ExitItem->GetId());
}

MyFrame1::~MyFrame1()
{
}
