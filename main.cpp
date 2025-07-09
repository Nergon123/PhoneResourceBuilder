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

#define ID_LOAD_BUTTON    1001
#define ID_EXPORT_BUTTON  1002
#define ID_PARSE_BUTTON   1003
#define ID_NEWFILE_BUTTON 1004

u_int32_t currentOffset = 0;

#pragma pack(push, 1)
struct Header {
    char     MAGIC[6] = "NerPh";
    uint16_t imageCount;
    uint16_t colors[8];
    uint16_t params[16];
};

struct ImageData {
    uint16_t id;
    uint8_t  count;
    uint16_t x, y, width, height;
    uint32_t offset;
    uint8_t  flags;
    // Flags:
    // 0b 0 0 0 0 0 0 0 0
    //    | | | | | | | |
    //    | | | | | | | +-- - Transparent
    //    | | | | | | +---- - Dynamic position (position not applicable)
    //    | | | | | +------ - Reserved (not used)
    //    | | | | +-------- - Reserved (not used)
    //    | | | +---------- - Reserved (not used)
    //    | | +------------ - Reserved (not used)
    //    | +-------------- - Reserved (not used)
    //    +---------------- - Reserved (not used)
    uint16_t transpColor;

    ImageData()
        : id(0), count(0), x(0), y(0), width(0), height(0), offset(0), flags(0), transpColor(0) {}
    ImageData(uint16_t id, uint8_t count, uint16_t x, uint16_t y, uint16_t width, uint16_t height,
              uint32_t offset, uint8_t flags, uint16_t transpColor)
        : id(id), count(count), x(x), y(y), width(width), height(height), offset(offset), flags(flags), transpColor(transpColor) {}
};
#pragma pack(pop)

uint16_t wxColourToRGB565(const wxColour& col) {
    uint16_t r = (col.Red() >> 3) & 0x1F;
    uint16_t g = (col.Green() >> 2) & 0x3F;
    uint16_t b = (col.Blue() >> 3) & 0x1F;
    return (r << 11) | (g << 5) | b;
}

struct DraggableImage {
    wxBitmap  bitmap;
    wxPoint   pos;
    ImageData data;
};
std::vector<DraggableImage> aImages;

wxDEFINE_EVENT(EVT_COLOR_PICKED, wxCommandEvent);

const char* ImageNames[] = {
    "Default Wallpaper",                               // 0
    "StatusBar Background",                            // 1
    "Battery charge",                                  // 2
    "Signal Strength",                                 // 3
    "No Signal",                                       // 4
    "Menu Background",                                 // 5
    "Menu Disabled Icons",                             // 6
    "Menu Selected Icons",                             // 7
    "Menu Header Background",                          // 8
    "Menu Header Icons",                               // 9
    "Inner Mail Icons",                                // 10
    "List Mail Icons",                                 // 11
    "Mail Menu Large Header",                          // 12
    "Settings Menu Large Header",                      // 13
    "Menu (To use with Large Header)",                 // 14
    "Full screen Notification",                        // 15
    "Context Menu Background",                         // 16
    "Background of ListMenu",                          // 17
    "Menu Background (without Icons(To be removed))",  // 18
    "Call animation dots",                             // 19
    "Voice Only label",                                // 20
    "Phone incoming call icon",                        // 21
    "Lightning incoming call animation",               // 22
    "Folder icon",                                     // 23
    "File icon",                                       // 24
    "Boot Logo"                                        // 25
};

int ImageCount[] =
    {

        1,  // Default Wallpaper                               0
        1,  // StatusBar Background                            1
        4,  // Battery charge                                  2
        4,  // Signal Strength                                 3
        1,  // No Signal                                       4
        1,  // Menu Background                                 5
        4,  // Menu Disabled Icons                             6
        4,  // Menu Selected Icons                             7
        1,  // Menu Header Background                          8
        3,  // Menu Header Icons                               9
        4,  // Inner Mail Icons                                10
        4,  // List Mail Icons                                 11
        1,  // Mail Menu Large Header                          12
        1,  // Settings Menu Large Header                      13
        1,  // Menu (To use with Large Header)                 14
        1,  // Full screen Notification                        15
        1,  // Context Menu Background                         16
        1,  // Background of ListMenu                          17
        1,  // Menu Background (without Icons(To be removed))  18
        8,  // Call animation dots                             19
        1,  // Voice Only label                                20
        1,  // Phone incoming call icon                        21
        2,  // Lightning incoming call animation               22
        1,  // Folder icon                                     23
        1,  // File icon                                       24
        1   // Boot Logo                                       25
};

ImageData Defaults[] = {
    ImageData(0, ImageCount[0], 0, 26, 240, 294, 0, 0, 0),           // Default Wallpaper
    ImageData(1, ImageCount[1], 0, 0, 240, 26, 0, 0, 0),             // StatusBar Background
    ImageData(2, ImageCount[2], 207, 0, 33, 26, 0, 0, 0),            // Battery charge
    ImageData(3, ImageCount[3], 0, 0, 30, 26, 0, 0, 0),              // Signal Strength
    ImageData(4, ImageCount[4], 0, 0, 37, 26, 0, 0, 0),              // No Signal
    ImageData(5, ImageCount[5], 0, 26, 240, 294, 0, 0, 0),           // Menu Background
    ImageData(6, ImageCount[6], 0, 0, 55, 55, 0, 0b11, 0x07e0),      // Menu Disabled Icons
    ImageData(7, ImageCount[7], 0, 0, 55, 55, 0, 0b11, 0x07e0),      // Menu Selected Icons
    ImageData(8, ImageCount[8], 0, 26, 240, 25, 0, 0, 0),            // Menu Header Background
    ImageData(9, ImageCount[9], 0, 26, 25, 25, 0, 0, 0),             // Menu Header Icons
    ImageData(10, ImageCount[10], 0, 0, 23, 24, 0, 0b10, 0),         // Inner Mail Icons
    ImageData(11, ImageCount[11], 0, 0, 18, 21, 0, 0b11, 0),         // List Mail Icons
    ImageData(12, ImageCount[12], 0, 26, 240, 42, 0, 0, 0),          // Mail Menu Large Header
    ImageData(13, ImageCount[13], 0, 26, 240, 42, 0, 0, 0),          // Settings Menu Large Header
    ImageData(14, ImageCount[14], 0, 68, 240, 128, 0, 0, 0),         // Menu (To use with Large Header)
    ImageData(15, ImageCount[15], 0, 90, 240, 134, 0, 0, 0),         // Full screen Notification
    ImageData(16, ImageCount[16], 16, 100, 208, 123, 0, 1, 0x07e0),  // Context Menu Background
    ImageData(17, ImageCount[17], 0, 26, 240, 294, 0, 0, 0),         // Background of ListMenu
    ImageData(18, ImageCount[18], 0, 26, 240, 294, 0, 0, 0),         // Menu Background (without Icons(To be removed))
    ImageData(19, ImageCount[19], 0, 0, 7, 7, 0, 0b10, 0),           // Call animation dots
    ImageData(20, ImageCount[20], 40, 143, 160, 34, 0, 0, 0),        // Voice Only label
    ImageData(21, ImageCount[21], 45, 105, 42, 50, 0, 1, 0xD6BA),    // Phone incoming call icon
    ImageData(22, ImageCount[22], 73, 90, 13, 14, 0, 1, 0xD6BA),     // Lightning incoming call animation
    ImageData(23, ImageCount[23], 0, 0, 18, 18, 0, 0b10, 0),         // Folder icon //TODO CHECK TRANSPARENCY
    ImageData(24, ImageCount[24], 0, 0, 18, 18, 0, 0b10, 0),         // File icon   //TODO CHECK TRANSPARENCY
    ImageData(25, ImageCount[25], 50, 85, 140, 135, 0, 0, 0)         // Boot Logo
};

class ImageCanvas : public wxPanel {
   public:
    ImageCanvas(wxWindow* parent)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(240, 320), wxBORDER_SUNKEN),
          dragging(false),
          draggedIndex(-1),
          selectedIndex(-1) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &ImageCanvas::OnPaint, this);
        Bind(wxEVT_LEFT_DOWN, &ImageCanvas::OnMouseDown, this);
        Bind(wxEVT_LEFT_UP, &ImageCanvas::OnMouseUp, this);
        Bind(wxEVT_MOTION, &ImageCanvas::OnMouseMove, this);
        Bind(wxEVT_RIGHT_DOWN, &ImageCanvas::OnMouseRightDown, this);
    }

    void AddImage(const ImageData& d, const wxBitmap& bmp, const wxPoint& pos) {
        images.push_back({bmp, pos, d});
        Refresh();
    }

    void RefreshSelectedImage() {
        Refresh();
    }

    DraggableImage* GetSelectedImage() {
        if (selectedIndex >= 0 && selectedIndex < (int)images.size())
            return &images[selectedIndex];
        return nullptr;
    }

    wxBitmap GetUnderlyingBitmap(const wxPoint& pos, const wxSize& size) {
        wxBitmap              result(size);
        wxMemoryDC            memDC(result);
        wxAutoBufferedPaintDC dc(this);
        memDC.Blit(0, 0, size.x, size.y, &dc, pos.x, pos.y);
        return result;
    }

   private:
    std::vector<DraggableImage> images;
    bool                        dragging;
    wxPoint                     dragStartMousePos;
    wxPoint                     dragStartImgPos;
    int                         draggedIndex;
    int                         selectedIndex;

    void OnPaint(wxPaintEvent&) {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();

        for (int i = 0; i < images.size(); ++i) {
            const auto& img = images[i];
            dc.DrawBitmap(img.bitmap, img.pos);
            if (i == selectedIndex) {
                dc.SetBrush(*wxTRANSPARENT_BRUSH);
                dc.SetPen(wxPen(*wxRED, 2));
                dc.DrawRectangle(wxRect(img.pos, img.bitmap.GetSize()));
            }
        }
    }

    void OnMouseDown(wxMouseEvent& event) {
        wxPoint pt = event.GetPosition();

        for (int i = (int)images.size() - 1; i >= 0; --i) {
            const auto& img = images[i];
            wxRect      rect(img.pos, img.bitmap.GetSize());
            if (rect.Contains(pt)) {
                dragging          = true;
                draggedIndex      = i;
                selectedIndex     = i;
                dragStartMousePos = pt;
                dragStartImgPos   = img.pos;

                if (i != (int)images.size() - 1) {
                    DraggableImage tmp = images[i];
                    images.erase(images.begin() + i);
                    images.push_back(tmp);
                    draggedIndex  = (int)images.size() - 1;
                    selectedIndex = draggedIndex;
                }

                CaptureMouse();
                Refresh();

                // Notify parent about selection
                wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
                GetParent()->GetEventHandler()->ProcessEvent(evt);

                break;
            }
        }
    }

    void OnMouseUp(wxMouseEvent& event) {
        if (dragging) {
            dragging     = false;
            draggedIndex = -1;
            ReleaseMouse();
        }
    }

    void OnMouseMove(wxMouseEvent& event) {
        if (dragging && draggedIndex != -1 && event.Dragging() && event.LeftIsDown()) {
            wxPoint pt               = event.GetPosition();
            wxPoint delta            = pt - dragStartMousePos;
            images[draggedIndex].pos = dragStartImgPos + delta;

            wxSize   sz      = GetClientSize();
            wxSize   bmpSize = images[draggedIndex].bitmap.GetSize();
            wxPoint& pos     = images[draggedIndex].pos;
            if (pos.x < 0) pos.x = 0;
            if (pos.y < 0) pos.y = 0;
            if (pos.x + bmpSize.x > sz.x) pos.x = sz.x - bmpSize.x;
            if (pos.y + bmpSize.y > sz.y) pos.y = sz.y - bmpSize.y;

            Refresh();

            // Notify the properties panel about the updated position
            wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
            GetParent()->GetEventHandler()->ProcessEvent(evt);
        }
    }

    void OnMouseRightDown(wxMouseEvent& event) {
        if (selectedIndex == -1) return;
        wxPoint pt = event.GetPosition();

        auto&  img = images[selectedIndex];
        wxRect imgRect(img.pos, img.bitmap.GetSize());
        if (imgRect.Contains(pt)) {
            wxPoint localPt = pt - img.pos;
            wxImage image   = img.bitmap.ConvertToImage();
            if (localPt.x >= 0 && localPt.y >= 0 && localPt.x < image.GetWidth() && localPt.y < image.GetHeight()) {
                wxColour col(image.GetRed(localPt.x, localPt.y),
                             image.GetGreen(localPt.x, localPt.y),
                             image.GetBlue(localPt.x, localPt.y));

                wxCommandEvent evt(EVT_COLOR_PICKED);
                evt.SetClientData(new wxColour(col));
                GetParent()->GetEventHandler()->ProcessEvent(evt);
            }
        }
    }
};

class PropertiesPanel : public wxPanel {
   public:
    PropertiesPanel(wxWindow* parent, ImageCanvas* canvas)
        : wxPanel(parent, wxID_ANY), canvas(canvas), selectedImg(nullptr) {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        idCtrl     = AddSpinCtrl(vbox, "ID", 0, 0, 65535);
        countCtrl  = AddSpinCtrl(vbox, "Count", 0, 0, 255);
        subIdCtrl  = AddSpinCtrl(vbox, "Sub-ID", 0, 0, 255);  // Add Sub-ID field
        xCtrl      = AddSpinCtrl(vbox, "X", 0, 0, 239);
        yCtrl      = AddSpinCtrl(vbox, "Y", 0, 0, 319);
        widthCtrl  = AddSpinCtrl(vbox, "Width", 1, 1, 240);
        heightCtrl = AddSpinCtrl(vbox, "Height", 1, 1, 320);

        transFlagCtrl = new wxCheckBox(this, wxID_ANY, "Transparency Flag");
        vbox->Add(transFlagCtrl, 0, wxALL, 5);
        transFlagCtrl->Bind(wxEVT_CHECKBOX, &PropertiesPanel::OnValueChanged, this);

        wxBoxSizer* hboxColor = new wxBoxSizer(wxHORIZONTAL);
        hboxColor->Add(new wxStaticText(this, wxID_ANY, "Transparency Color"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        colorPreview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(30, 20));
        colorPreview->SetBackgroundColour(*wxWHITE);
        hboxColor->Add(colorPreview, 0);
        pickColorBtn = new wxButton(this, wxID_ANY, "Pick Color");
        hboxColor->Add(pickColorBtn, 0, wxLEFT, 5);
        vbox->Add(hboxColor, 0, wxALL, 5);

        bakeButton = new wxButton(this, wxID_ANY, "Bake Transparency");
        vbox->Add(bakeButton, 0, wxALL, 5);
        bakeButton->Bind(wxEVT_BUTTON, &PropertiesPanel::OnBakeTransparency, this);

        pickColorBtn->Bind(wxEVT_BUTTON, &PropertiesPanel::OnPickColor, this);

        idCtrl->Bind(wxEVT_SPINCTRL, &PropertiesPanel::OnValueChanged, this);
        countCtrl->Bind(wxEVT_SPINCTRL, &PropertiesPanel::OnValueChanged, this);
        subIdCtrl->Bind(wxEVT_SPINCTRL, &PropertiesPanel::OnSubIdChanged, this);  // Bind Sub-ID change event
        xCtrl->Bind(wxEVT_SPINCTRL, &PropertiesPanel::OnValueChanged, this);
        yCtrl->Bind(wxEVT_SPINCTRL, &PropertiesPanel::OnValueChanged, this);
        widthCtrl->Bind(wxEVT_SPINCTRL, &PropertiesPanel::OnValueChanged, this);
        heightCtrl->Bind(wxEVT_SPINCTRL, &PropertiesPanel::OnValueChanged, this);

        SetSizer(vbox);
    }

    void SetSelectedImage(DraggableImage* img) {
        printf("SetSelectedImage called: %p\n", img);
        selectedImg = img;
        if (!img) {
            Enable(false);
            return;
        }
        Enable(true);
        UpdateProperties();
    }

    void UpdateProperties() {
        printf("UpdateProperties called\n");
        if (!selectedImg) {
            printf("UpdateProperties: selectedImg is nullptr, disabling panel\n");
            Enable(false);
            return;
        }

        idCtrl->SetValue(selectedImg->data.id);
        countCtrl->SetValue(selectedImg->data.count);
        subIdCtrl->SetRange(0, selectedImg->data.count - 1);  // Set Sub-ID range based on count
        subIdCtrl->SetValue(0);                               // Default to the first sub-image
        xCtrl->SetValue(selectedImg->pos.x);
        yCtrl->SetValue(selectedImg->pos.y + subIdCtrl->GetValue() * selectedImg->data.height);  // Adjust Y based on Sub-ID
        widthCtrl->SetValue(selectedImg->data.width);
        heightCtrl->SetValue(selectedImg->data.height);

        transFlagCtrl->SetValue((selectedImg->data.flags & 1) != 0);

        // Update color preview
        wxColour col = RGB565ToWxColour(selectedImg->data.transpColor);
        colorPreview->SetBackgroundColour(col);
        colorPreview->Refresh();
    }

    void SetTransparencyColor(const wxColour& col) {
        if (!selectedImg) return;

        selectedImg->data.transpColor = wxColourToRGB565(col);
        // Also set transparency flag bit 0 to 1 automatically here if you want:
        selectedImg->data.flags |= 1;
        colorPreview->SetBackgroundColour(col);
        colorPreview->Refresh();
        canvas->RefreshSelectedImage();
    }

   private:
    wxSpinCtrl* idCtrl;
    wxSpinCtrl* countCtrl;
    wxSpinCtrl* subIdCtrl;
    wxSpinCtrl* xCtrl;
    wxSpinCtrl* yCtrl;
    wxSpinCtrl* widthCtrl;
    wxSpinCtrl* heightCtrl;
    wxCheckBox* transFlagCtrl;
    wxPanel*    colorPreview;
    wxButton*   pickColorBtn;
    wxButton*   bakeButton;

    ImageCanvas*    canvas;
    DraggableImage* selectedImg;

    static wxColour RGB565ToWxColour(uint16_t rgb565) {
        uint8_t r = (rgb565 >> 11) & 0x1F;
        uint8_t g = (rgb565 >> 5) & 0x3F;
        uint8_t b = rgb565 & 0x1F;
        return wxColour((r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2));
    }

    wxSpinCtrl* AddSpinCtrl(wxSizer* sizer, const wxString& label, int value, int minVal, int maxVal) {
        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
        // Set a fixed width for the label to align all controls
        wxStaticText* lbl        = new wxStaticText(this, wxID_ANY, label);
        int           labelWidth = 70;  // adjust as needed for your labels
        lbl->SetMinSize(wxSize(labelWidth, -1));
        hbox->Add(lbl, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        wxSpinCtrl* ctrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, minVal, maxVal, value);
        hbox->Add(ctrl);
        sizer->Add(hbox, 0, wxLEFT | wxRIGHT | wxTOP, 5);  // Remove wxALL to avoid extra top/bottom padding
        return ctrl;
    }

    void OnPickColor(wxCommandEvent&) {
        if (!selectedImg) return;

        wxColourData data;
        data.SetColour(RGB565ToWxColour(selectedImg->data.transpColor));
        wxColourDialog dlg(this, &data);
        if (dlg.ShowModal() == wxID_OK) {
            wxColour col = dlg.GetColourData().GetColour();
            SetTransparencyColor(col);
        }
    }

    void OnBakeTransparency(wxCommandEvent&) {
        if (!selectedImg) return;

        wxBitmap& bmp = selectedImg->bitmap;
        wxImage   img = bmp.ConvertToImage();
        if (!img.HasAlpha()) return;

        wxBitmap underneathBitmap = canvas->GetUnderlyingBitmap(selectedImg->pos, img.GetSize());
        wxImage  underneathImage  = underneathBitmap.ConvertToImage();

        for (int x = 0; x < img.GetWidth(); ++x) {
            for (int y = 0; y < img.GetHeight(); ++y) {
                if (img.HasAlpha() && img.GetAlpha(x, y) == 0) {
                    img.SetRGB(x, y, underneathImage.GetRed(x, y), underneathImage.GetGreen(x, y), underneathImage.GetBlue(x, y));
                }
            }
        }

        img.ClearAlpha();
        selectedImg->bitmap = wxBitmap(img);
        canvas->RefreshSelectedImage();
    }

    void OnSubIdChanged(wxCommandEvent&) {
        if (!selectedImg) return;
        int subId = subIdCtrl->GetValue();
        canvas->RefreshSelectedImage();
    }

    void OnValueChanged(wxCommandEvent&) {
        if (!selectedImg) return;

        selectedImg->data.id     = idCtrl->GetValue();
        selectedImg->data.count  = (uint8_t)countCtrl->GetValue();
        selectedImg->pos.x       = xCtrl->GetValue();
        selectedImg->pos.y       = yCtrl->GetValue() - subIdCtrl->GetValue() * selectedImg->data.height;  // Adjust Y for Sub-ID
        selectedImg->data.width  = widthCtrl->GetValue();
        selectedImg->data.height = heightCtrl->GetValue();

        if (transFlagCtrl->GetValue())
            selectedImg->data.flags |= 1;
        else
            selectedImg->data.flags &= ~1;

        canvas->RefreshSelectedImage();
    }
};

class MyFrame : public wxFrame {
   public:
    MyFrame()
        : wxFrame(nullptr, wxID_ANY, "Image Drag & Layer Demo with Properties", wxDefaultPosition, wxSize(550, 500)) {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
        canvas           = new ImageCanvas(this);
        properties       = new PropertiesPanel(this, canvas);
        hbox->Add(canvas, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
        hbox->Add(properties, 1, wxEXPAND | wxALL, 10);
        vbox->Add(hbox, 1, wxEXPAND);
        wxBoxSizer* buttonBox     = new wxBoxSizer(wxHORIZONTAL);
        wxButton*   loadButton    = new wxButton(this, ID_LOAD_BUTTON, "Load Images");
        wxButton*   exportButton  = new wxButton(this, ID_EXPORT_BUTTON, "Export Images");
        wxButton*   parseButton   = new wxButton(this, ID_PARSE_BUTTON, "Parse Binary");
        wxButton*   newFileButton = new wxButton(this, ID_NEWFILE_BUTTON, "New File");

        buttonBox->Add(loadButton, 0, wxALL, 5);
        buttonBox->Add(exportButton, 0, wxALL, 5);
        buttonBox->Add(parseButton, 0, wxALL, 5);
        buttonBox->Add(newFileButton, 0, wxALL, 5);
        vbox->Add(buttonBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
        wxBoxSizer*   dropdownBox   = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* dropdownLabel = new wxStaticText(this, wxID_ANY, "Defaults:");
        imageDropdown               = new wxChoice(this, wxID_ANY);
        dropdownBox->Add(dropdownLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        dropdownBox->Add(imageDropdown, 1, wxEXPAND);
        vbox->Add(dropdownBox, 0, wxEXPAND | wxALL, 10);
        SetSizer(vbox);
        printf("binding events\n");
        Bind(wxEVT_BUTTON, &MyFrame::OnLoadImages, this, ID_LOAD_BUTTON);
        Bind(wxEVT_BUTTON, &MyFrame::OnExportImages, this, ID_EXPORT_BUTTON);
        Bind(wxEVT_BUTTON, &MyFrame::OnParseBinary, this, ID_PARSE_BUTTON);
        Bind(wxEVT_BUTTON, &MyFrame::OnNewFile, this, ID_NEWFILE_BUTTON);
        Bind(wxEVT_CHOICE, &MyFrame::OnImageDropdownChanged, this, imageDropdown->GetId());
        canvas->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnImageSelected, this);
        Bind(EVT_COLOR_PICKED, &MyFrame::OnColorPicked, this);
        printf("Populating image dropdown\n");
        PopulateImageDropdown();
    }

   private:
    ImageCanvas*     canvas;
    PropertiesPanel* properties;
    wxChoice*        imageDropdown;

    void PopulateImageDropdown() {
        imageDropdown->Clear();
        for (size_t i = 0; i < sizeof(ImageNames) / sizeof(ImageNames[0]); ++i) {
            imageDropdown->Append(wxString::Format("[%d] %s", (int)i, ImageNames[i]));
        }
    }

    void OnLoadImages(wxCommandEvent&) {
        printf("OnLoadImages called\n");
        wxFileDialog openFileDialog(this, "Open PNG files", "", "", "PNG files (*.png)|*.png", wxFD_OPEN | wxFD_MULTIPLE);
        if (openFileDialog.ShowModal() == wxID_CANCEL) return;

        wxArrayString filePaths;
        openFileDialog.GetPaths(filePaths);

        for (const auto& filePath : filePaths) {
            wxImage image;
            if (!image.LoadFile(filePath, wxBITMAP_TYPE_PNG)) {
                wxLogError("Failed to load image: %s", filePath);
                continue;
            }

            wxBitmap  bitmap(image);
            ImageData data = {static_cast<uint16_t>(aImages.size() + 1), 1, 0, 0, static_cast<uint16_t>(image.GetWidth()), static_cast<uint16_t>(image.GetHeight()), 0, 0, 0};
            canvas->AddImage(data, bitmap, wxPoint(0, 0));
        }
    }

    void OnImageDropdownChanged(wxCommandEvent& event) {
        int selection = imageDropdown->GetSelection();
        if (selection != wxNOT_FOUND && selection < (int)aImages.size()) {
            canvas->Refresh();
            properties->SetSelectedImage(&aImages[selection]);
        }
    }

    void OnNewFile(wxCommandEvent&) {
        printf("OnNewFile called\n");
        wxTextEntryDialog dialog(wxTheApp->GetTopWindow(), "Enter file name:", "New File");
        if (dialog.ShowModal() == wxID_OK) {
            wxString fileName = dialog.GetValue();
            if (fileName.IsEmpty()) {
                wxLogError("File name cannot be empty.");
                return;
            }

            // Create and save an empty file with the given name
            std::ofstream newFile(fileName.ToStdString());
            if (!newFile) {
                wxLogError("Failed to create file: %s", fileName);
                return;
            }
            newFile.close();
            wxLogMessage("New file '%s' created.", fileName);
        }
    }

    void OnExportImages(wxCommandEvent&) {
        printf("OnExportImages called\n");
        wxFileDialog saveFileDialog(this, "Save PNG files", "", "", "PNG files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() == wxID_CANCEL) return;

        wxString directory = wxFileName(saveFileDialog.GetPath()).GetPath();

        for (size_t i = 0; i < aImages.size(); ++i) {
            wxImage  image    = aImages[i].bitmap.ConvertToImage();
            wxString filePath = wxString::Format("%s/image_%zu.png", directory, i + 1);
            if (!image.SaveFile(filePath, wxBITMAP_TYPE_PNG)) {
                wxLogError("Failed to save image: %s", filePath);
            }
        }
    }

    void OnParseBinary(wxCommandEvent&) {
        printf("OnParseBinary called\n");
        wxMessageBox("OnParseBinary function called.", "Debug");
        wxLog::SetActiveTarget(new wxLogStderr());
        wxLogMessage("Parsing binary file...");
        wxFileDialog openFileDialog(this, "Open Binary File", "", "", "Binary files (*.bin)|*.bin", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) return;

        std::ifstream file(openFileDialog.GetPath().ToStdString(), std::ios::binary);
        if (!file.is_open()) {
            wxLogError("Failed to open binary file.");
            return;
        }

        Header header;
        file.read(reinterpret_cast<char*>(&header), sizeof(Header));
        if (strncmp(header.MAGIC, "NerPh", 5) != 0) {
            wxLogError("Invalid file format.");
            return;
        }

        std::vector<ImageData> imageData(header.imageCount);
        file.read(reinterpret_cast<char*>(imageData.data()), header.imageCount * sizeof(ImageData));

        for (const auto& data : imageData) {
            file.seekg(data.offset, std::ios::beg);
            wxImage image(data.width, data.height);
            file.read(reinterpret_cast<char*>(image.GetData()), data.width * data.height * 3);

            wxBitmap bitmap(image);
            canvas->AddImage(data, bitmap, wxPoint(data.x, data.y));
        }

        wxLogMessage("Parsed %d images from binary file.", header.imageCount);
    }

    void OnImageSelected(wxCommandEvent&) {
        DraggableImage* img = canvas->GetSelectedImage();
        properties->SetSelectedImage(img);
    }

    void OnColorPicked(wxCommandEvent& event) {
        wxColour* col = static_cast<wxColour*>(event.GetClientData());
        if (col) {
            properties->SetTransparencyColor(*col);
            delete col;
        }
    }
};

class MyApp : public wxApp {
   public:
    bool OnInit() override {
        MyFrame* frame = new MyFrame();
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
