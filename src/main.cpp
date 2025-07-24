#include <vector>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/spinctrl.h>
#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/choice.h>
#include <fstream>

#include <cstdint>
#include "ImageCanvas.h"
#include "SpriteForm.h"
#include "MainFrame.h"



class MyApp : public wxApp {
   public:
    bool OnInit() override {
        wxInitAllImageHandlers();
        MainFrame* frame = new MainFrame(nullptr);
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
