///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-85-gdf26f269)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class SpriteFormFrame
///////////////////////////////////////////////////////////////////////////////
class SpriteFormFrame : public wxFrame
{
	private:

	protected:
		wxPanel* SpriteCanvas;
		wxGridSizer* gSizer21;
		wxStaticText* m_staticText11;
		wxButton* OpenButton;
		wxStaticText* m_staticText111;
		wxSpinCtrl* XCtrl;
		wxStaticText* m_staticText1111;
		wxSpinCtrl* YCtrl;
		wxStaticText* m_staticText11111;
		wxSpinCtrl* WidthCtrl;
		wxStaticText* m_staticText21;
		wxSpinCtrl* HeightCtrl;
		wxStaticText* m_staticText111111;
		wxSpinCtrl* goalWidth;
		wxStaticText* m_staticText212;
		wxSpinCtrl* goalHeight;
		wxStaticText* m_staticText211;
		wxSpinCtrl* SelectedIndexCtrl;
		wxButton* CancelButton;
		wxButton* ContinueButton;

		// Virtual event handlers, override them in your derived class
		virtual void OnKeyUp( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnEnterWindow( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLeaveWindow( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLeftMDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMotion( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnRightMDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnXChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnYChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnWidthChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnHeightChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnGWidth( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnGHeight( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnSelectedIndex( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnContinue( wxCommandEvent& event ) { event.Skip(); }


	public:

		SpriteFormFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("SpriteSheet Selector"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 560,425 ), long style = wxCAPTION|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxTAB_TRAVERSAL );

		~SpriteFormFrame();

};

