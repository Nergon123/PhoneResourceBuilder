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
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/clrpicker.h>
#include <wx/menu.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame1
///////////////////////////////////////////////////////////////////////////////
class MyFrame1 : public wxFrame
{
	private:

	protected:
		wxPanel* m_canvasPanel;
		wxStaticText* m_staticText12;
		wxCheckListBox* currentImagesCheckList;
		wxButton* List_UP;
		wxButton* List_DOWN;
		wxButton* List_BIND;
		wxButton* List_NEWIMAGE;
		wxButton* List_DELETEIMAGE;
		wxBoxSizer* bSizer5;
		wxStaticText* IDText;
		wxStaticText* Description;
		wxSpinCtrl* IDCtrl;
		wxStaticText* CountText;
		wxSpinCtrl* CountControl;
		wxStaticText* IndexText;
		wxSpinCtrl* IndexControl;
		wxStaticText* XposText;
		wxSpinCtrl* XposControl;
		wxStaticText* YposText;
		wxSpinCtrl* YposControl;
		wxStaticText* WidthText;
		wxSpinCtrl* WidthControl;
		wxStaticText* HeightText;
		wxSpinCtrl* HeightControl;
		wxCheckBox* includeInExport;
		wxSlider* transpSlider;
		wxColourPickerCtrl* ColorPicker;
		wxCheckBox* TransCBox;
		wxStaticText* RGBStatus;
		wxButton* m_button1;
		wxButton* bakeButton;
		wxButton* hardBakeButton;
		wxMenuBar* m_menubar1;
		wxMenu* MenuBar;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnImagesListSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnImagesListToggle( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClkUpList( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClkDownList( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClkBindList( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClkNewList( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClkDelList( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateID( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnUpdateCount( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnUpdateIndex( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnUpdateX( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnUpdateY( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnUpdateWidth( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnUpdateHeight( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnIncludeInFileBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTranspSlider( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnTransColorChanged( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void OnUpdateTrans( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickPICK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBakeImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHardBake( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSaveAs( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnImport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExportPNG( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExportAllPNG( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExitMenu( wxCommandEvent& event ) { event.Skip(); }


	public:

		MyFrame1( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("PhoneResourceBuilder"), const wxPoint& pos = wxPoint( -1,-1 ), const wxSize& size = wxSize( 600,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MyFrame1();

};

