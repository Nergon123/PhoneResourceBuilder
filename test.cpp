#include <wx/wx.h>
#include <wx/filedlg.h>

class MyApp : public wxApp {
public:
    bool OnInit() override {
        wxFrame* frame = new wxFrame(nullptr, wxID_ANY, "Test wxFileDialog");
        frame->Show();

        wxFileDialog openFileDialog(frame, "Open File", "", "", "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_OK) {
            wxMessageBox("File selected: " + openFileDialog.GetPath());
        }

        return true;
    }
};

wxIMPLEMENT_APP(MyApp);