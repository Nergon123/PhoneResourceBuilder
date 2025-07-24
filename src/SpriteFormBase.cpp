///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-85-gdf26f269)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SpriteFormBase.h"

///////////////////////////////////////////////////////////////////////////

SpriteFormFrame::SpriteFormFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 560,425 ), wxSize( -1,-1 ) );

	wxGridSizer* gSizer8;
	gSizer8 = new wxGridSizer( 0, 2, 0, 0 );

	gSizer8->SetMinSize( wxSize( 512,512 ) );
	SpriteCanvas = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 280,380 ), wxTAB_TRAVERSAL );
	SpriteCanvas->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW ) );
	SpriteCanvas->SetMinSize( wxSize( 280,380 ) );
	SpriteCanvas->SetMaxSize( wxSize( 280,380 ) );

	gSizer8->Add( SpriteCanvas, 1, wxALL, 10 );

	gSizer21 = new wxGridSizer( 0, 2, 0, 0 );

	gSizer21->SetMinSize( wxSize( 200,340 ) );
	m_staticText11 = new wxStaticText( this, wxID_ANY, _("Spritesheet..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	gSizer21->Add( m_staticText11, 0, wxALIGN_CENTER|wxALL, 5 );

	OpenButton = new wxButton( this, wxID_ANY, _("Open"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer21->Add( OpenButton, 0, wxALIGN_CENTER|wxALL, 5 );

	m_staticText111 = new wxStaticText( this, wxID_ANY, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	gSizer21->Add( m_staticText111, 0, wxALIGN_CENTER|wxALL, 5 );

	XCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	gSizer21->Add( XCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

	m_staticText1111 = new wxStaticText( this, wxID_ANY, _("Y"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1111->Wrap( -1 );
	gSizer21->Add( m_staticText1111, 0, wxALIGN_CENTER|wxALL, 5 );

	YCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	gSizer21->Add( YCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

	m_staticText11111 = new wxStaticText( this, wxID_ANY, _("Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11111->Wrap( -1 );
	gSizer21->Add( m_staticText11111, 0, wxALIGN_CENTER|wxALL, 5 );

	WidthCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	gSizer21->Add( WidthCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

	m_staticText21 = new wxStaticText( this, wxID_ANY, _("Height"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	gSizer21->Add( m_staticText21, 0, wxALIGN_CENTER|wxALL, 5 );

	HeightCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	gSizer21->Add( HeightCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

	m_staticText111111 = new wxStaticText( this, wxID_ANY, _("Real Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111111->Wrap( -1 );
	gSizer21->Add( m_staticText111111, 0, wxALIGN_CENTER|wxALL, 5 );

	goalWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	gSizer21->Add( goalWidth, 0, wxALL, 5 );

	m_staticText212 = new wxStaticText( this, wxID_ANY, _("Real Height"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText212->Wrap( -1 );
	gSizer21->Add( m_staticText212, 0, wxALIGN_CENTER|wxALL, 5 );

	goalHeight = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	gSizer21->Add( goalHeight, 0, wxALL, 5 );

	m_staticText211 = new wxStaticText( this, wxID_ANY, _("Selected Index"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText211->Wrap( -1 );
	gSizer21->Add( m_staticText211, 0, wxALIGN_CENTER|wxALL, 5 );

	SelectedIndexCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	gSizer21->Add( SelectedIndexCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

	CancelButton = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer21->Add( CancelButton, 0, wxALIGN_CENTER|wxALL, 5 );

	ContinueButton = new wxButton( this, wxID_ANY, _("Continue"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer21->Add( ContinueButton, 0, wxALIGN_CENTER|wxALL, 5 );


	gSizer8->Add( gSizer21, 5, wxALIGN_RIGHT, 5 );


	this->SetSizer( gSizer8 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_KEY_UP, wxKeyEventHandler( SpriteFormFrame::OnKeyUp ) );
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( SpriteFormFrame::OnSize ) );
	SpriteCanvas->Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( SpriteFormFrame::OnEnterWindow ), NULL, this );
	SpriteCanvas->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( SpriteFormFrame::OnLeaveWindow ), NULL, this );
	SpriteCanvas->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SpriteFormFrame::OnLeftMDown ), NULL, this );
	SpriteCanvas->Connect( wxEVT_MOTION, wxMouseEventHandler( SpriteFormFrame::OnMotion ), NULL, this );
	SpriteCanvas->Connect( wxEVT_PAINT, wxPaintEventHandler( SpriteFormFrame::OnPaint ), NULL, this );
	SpriteCanvas->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( SpriteFormFrame::OnRightMDown ), NULL, this );
	OpenButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SpriteFormFrame::OnOpen ), NULL, this );
	XCtrl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SpriteFormFrame::OnXChange ), NULL, this );
	YCtrl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SpriteFormFrame::OnYChange ), NULL, this );
	WidthCtrl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SpriteFormFrame::OnWidthChange ), NULL, this );
	HeightCtrl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SpriteFormFrame::OnHeightChange ), NULL, this );
	goalWidth->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SpriteFormFrame::OnGWidth ), NULL, this );
	goalHeight->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SpriteFormFrame::OnGHeight ), NULL, this );
	SelectedIndexCtrl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SpriteFormFrame::OnSelectedIndex ), NULL, this );
	CancelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SpriteFormFrame::OnCancel ), NULL, this );
	ContinueButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SpriteFormFrame::OnContinue ), NULL, this );
}

SpriteFormFrame::~SpriteFormFrame()
{
}
