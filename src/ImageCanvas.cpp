#include "ImageCanvas.h"
#include "MainFrame.h"
wxDEFINE_EVENT(EVT_COLOR_PICKED, wxCommandEvent);
wxBitmap spriteSheet = wxBitmap();

uint16_t wxColourToRGB565(const wxColour& col) {
    uint16_t color = ((col.Red() >> 3) << 11) |
                     ((col.Green() >> 2) << 5) |
                     (col.Blue() >> 3);
                     
    return (color << 8) | (color >> 8);
}

wxColour RGB565ToWxColour(uint16_t rgb565) {
    rgb565 = (rgb565 >> 8) | (rgb565 << 8);

    uint8_t r5 = (rgb565 >> 11) & 0x1F;
    uint8_t g6 = (rgb565 >> 5) & 0x3F;
    uint8_t b5 = rgb565 & 0x1F;

    uint8_t r8 = (r5 << 3) | (r5 >> 2);
    uint8_t g8 = (g6 << 2) | (g6 >> 4);
    uint8_t b8 = (b5 << 3) | (b5 >> 2);

    return wxColour(r8, g8, b8);
}

const char ImageNames[32][DEFAULTS_COUNT] = {
    "Default Wallpaper",            // 0
    "StatusBar Background",         // 1
    "Battery charge",               // 2
    "Signal Strength",              // 3
    "No Signal",                    // 4
    "Menu Background",              // 5
    "Menu Mail Icon",               // 6
    "Menu Contacts Icon",           // 7
    "Menu \"e\" Icon",              // 8
    "Menu Settings Icon",           // 9
    "List Header Background",       // 10
    "List Header Icons",            // 11
    "Inner Mail Icons",             // 12
    "List Mail Icons",              // 13
    "Mail Menu Large Header",       // 14
    "Settings Menu Large Header",   // 15
    "Menu Large Header",            // 16
    "Full screen Notification",     // 17
    "Context Menu Background",      // 18
    "Background of ListMenu",       // 19
    "Menu Background (Unused)",     // 20
    "Call animation dots",          // 21
    "Voice Only label",             // 22
    "Phone incoming call icon",     // 23
    "Lightning for incoming call",  // 24
    "Folder icon",                  // 25
    "File icon",                    // 26
    "Boot Logo"                     // 27
};

// clang-format off
ImageData Defaults[DEFAULTS_COUNT] = {
    //       ( id | count |  x  |  y  | width | height | offset | flags | transpColor |    name     )
    ImageData( 0,     1,    0,     26,   240,    294,      0,     0b00,      0x0000   ,ImageNames[0]),       // Default Wallpaper
    ImageData( 1,     1,    0,     0,    240,    26,       0,     0b00,      0x0000   ,ImageNames[1]),       // StatusBar Background
    ImageData( 2,     4,    207,   0,    33,     26,       0,     0b00,      0x0000   ,ImageNames[2]),       // Battery charge
    ImageData( 3,     4,    0,     0,    30,     26,       0,     0b00,      0x0000   ,ImageNames[3]),       // Signal Strength
    ImageData( 4,     1,    0,     0,    37,     26,       0,     0b00,      0x0000   ,ImageNames[4]),       // No Signal
    ImageData( 5,     1,    0,     26,   240,    294,      0,     0b00,      0x0000   ,ImageNames[5]),       // Menu Background
    ImageData( 6,     2,    49,    69,   55,     55,       0,     0b00,      0x07E0   ,ImageNames[6]),       // Menu Mail Icon
    ImageData( 7,     2,    138,   69,   55,     55,       0,     0b00,      0x07E0   ,ImageNames[7]),       // Menu Contacts Icon
    ImageData( 8,     2,    49,    148,  55,     55,       0,     0b00,      0x07E0   ,ImageNames[8]),       // Menu "e" Icon
    ImageData( 9,     2,    138,   148,  55,     55,       0,     0b00,      0x07E0   ,ImageNames[9]),       // Menu Settings Icon
    ImageData( 10,    1,    0,     26,   240,    25,       0,     0b00,      0x0000   ,ImageNames[10]),      // List Header Background
    ImageData( 11,    3,    0,     26,   25,     25,       0,     0b00,      0x0000   ,ImageNames[11]),      // List Header Icons
    ImageData( 12,    4,    0,     0,    23,     24,       0,     0b10,      0x0000   ,ImageNames[12]),      // In-message Mail Icons
    ImageData( 13,    4,    0,     0,    18,     21,       0,     0b11,      0x0000   ,ImageNames[13]),      // List Mail Icons
    ImageData( 14,    1,    0,     26,   240,    42,       0,     0b00,      0x0000   ,ImageNames[14]),      // Mail Menu Large Header
    ImageData( 15,    1,    0,     26,   240,    42,       0,     0b00,      0x0000   ,ImageNames[15]),      // Settings Menu Large Header
    ImageData( 16,    1,    0,     68,   240,    128,      0,     0b00,      0x0000   ,ImageNames[16]),      // Menu (To use with Large Header)
    ImageData( 17,    1,    0,     90,   240,    134,      0,     0b00,      0x0000   ,ImageNames[17]),      // Full screen Notification
    ImageData( 18,    1,    16,    100,  208,    123,      0,     0b01,      0x07E0   ,ImageNames[18]),      // Context Menu Background
    ImageData( 19,    1,    0,     26,   240,    294,      0,     0b00,      0x0000   ,ImageNames[19]),      // Background of ListMenu
    ImageData( 20,    1,    0,     26,   240,    294,      0,     0b00,      0x0000   ,ImageNames[20]),      // Menu Background (without Icons(To be removed))
    ImageData( 21,    8,    0,     0,    7,      7,        0,     0b10,      0x0000   ,ImageNames[21]),      // Call animation dots
    ImageData( 22,    1,    40,    143,  160,    34,       0,     0b00,      0x0000   ,ImageNames[22]),      // Voice Only label
    ImageData( 23,    1,    45,    105,  42,     50,       0,     0b01,      0xD6BA   ,ImageNames[23]),      // Phone incoming call icon
    ImageData( 24,    2,    73,    90,   13,     14,       0,     0b01,      0xD6BA   ,ImageNames[24]),      // Lightning incoming call animation
    ImageData( 25,    1,    0,     0,    18,     18,       0,     0b11,      0x0000   ,ImageNames[25]),      // Folder icon //TODO CHECK TRANSPARENCY
    ImageData( 26,    1,    0,     0,    18,     18,       0,     0b11,      0x0000   ,ImageNames[26]),      // File icon   //TODO CHECK TRANSPARENCY
    ImageData( 27,    1,    50,    85,   140,    135,      0,     0b00,      0x0000   ,ImageNames[27])       // Boot Logo
};

ImageCanvas::ImageCanvas(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(240, 320)),
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

int                                 ImageCanvas::GetImageCount() const { return (int)images.size(); }
const std::vector<DraggableImage*>& ImageCanvas::GetImages() const {
    return images;
}

void ImageCanvas::SetImagesOrder(const std::vector<int>& order) {
    std::vector<DraggableImage*> newOrder;
    for (int i : order) {
        if (i >= 0 && i < (int)images.size())
            newOrder.push_back(images[i]);
    }
    images = newOrder;
    Refresh();
}

int ImageCanvas::GetSelectedImageIndex() const { return selectedIndex; }

void ImageCanvas::SetSelectedImageIndex(int index) {
    if (index >= 0 && index < (int)images.size()) {
        selectedIndex = index;
        Refresh();
    }
}

void ImageCanvas::AddImage(DraggableImage* image) {
    if (!image) return;

    // Compute vertical stacking position
    int offsetY = 0;
    for (const auto& img : images) {
        if (img && img->enabled)
            offsetY += img->bitmap.GetHeight() + 4;  // vertical spacing
    }

    image->pos.y = offsetY;
    images.push_back(image);
    Refresh();
}

void ImageCanvas::RefreshSelectedImage() {
    Refresh();
}

ImageCanvas::~ImageCanvas() {
    for (DraggableImage* img : images)
        delete img;
    images.clear();
}

DraggableImage* ImageCanvas::GetSelectedImage() {
    if (selectedIndex >= 0 && selectedIndex < (int)images.size())
        return images[selectedIndex];
    return nullptr;
}

wxBitmap ImageCanvas::GetUnderlyingBitmap(const wxPoint& pos, const wxSize& size) {
    wxBitmap              result(size);
    wxMemoryDC            memDC(result);
    wxAutoBufferedPaintDC dc(this);
    memDC.Blit(0, 0, size.x, size.y, &dc, pos.x, pos.y);
    return result;
}

void ImageCanvas::SortImagesByListIndex() {
    std::sort(images.begin(), images.end(), [](DraggableImage* a, DraggableImage* b) {
        if (!a) return false;
        if (!b) return true;
        return a->listIndex < b->listIndex;
    });
    UpdateListIndices();
    Refresh();
}

void ImageCanvas::UpdateListIndices() {
    for (size_t i = 0; i < images.size(); ++i) {
        if (images[i]) images[i]->listIndex = (int)i;
    }
}

void ImageCanvas::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    // printf("\n");
    for (size_t i = 0; i < images.size(); ++i) {
        DraggableImage* img = images[i];
        if (!img || !img->enabled || !img->bitmap.IsOk()) continue;
        img->pos        = wxPoint(img->data.x, img->data.y);
        wxMemoryDC wxDC = wxMemoryDC(img->bitmap);
        dc.Blit(img->pos, wxSize(img->data.width, img->data.height), &wxDC, wxPoint(0, img->data.height * img->index));
        // dc.DrawBitmap(img->bitmap, img->pos);
        // printf("%d %d\n", images[i]->listIndex,i);
        if ((int)img->listIndex == selectedIndex) {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(wxPen(*wxRED, 2));
            dc.DrawRectangle(wxRect(img->pos, wxSize(img->data.width, img->data.height)));
        }
    }
}

void ImageCanvas::SetImageFrameIndex(int imgIndex, uint8_t newFrameIndex) {
    if (imgIndex < 0 || imgIndex >= (int)images.size()) return;

    DraggableImage& img = *images[imgIndex];
    if (newFrameIndex >= img.data.count) return;

    img.index = newFrameIndex;

    // Refresh sub-bitmap
    int frameHeight = img.data.height;
    int frameWidth  = img.data.width;
    int srcY        = newFrameIndex * frameHeight;
    img.bitmap      = img.bitmap.GetSubBitmap(wxRect(0, srcY, frameWidth, frameHeight));

    Refresh();
}

// void ImageCanvas::AddAllFrames(const ImageData& d, const wxBitmap& fullBitmap, const wxPoint& basePos) {
//     for (uint8_t i = 0; i < d.count; ++i) {
//         wxPoint pos = basePos + wxPoint(0, i * (d.height + 4));  // Add spacing
//         AddImage(d);
//     }
// }

void ImageCanvas::OnMouseDown(wxMouseEvent& event) {
    wxPoint pt = event.GetPosition();

    for (int i = (int)images.size() - 1; i >= 0; --i) {
        DraggableImage* img = images[i];
        if (!img->enabled) continue;
        wxRect rect(img->pos, img->bitmap.GetSize());
        if (rect.Contains(pt)) {
            img->selected     = true;
            dragging          = true;
            draggedIndex      = img->listIndex;
            selectedIndex     = img->listIndex;
            dragStartMousePos = pt;
            dragStartImgPos   = img->pos;

            CaptureMouse();
            Refresh();
            MainFrame* frame = dynamic_cast<MainFrame*>(GetParent());
            if (frame) {
                frame->GetList()->SetSelection(img->listIndex);
                frame->UnpackImageData(img);
                frame->EnableControls();

            }

            // Notify parent about selection
            wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
            GetParent()->GetEventHandler()->ProcessEvent(evt);

            break;
        }
    }
}

void ImageCanvas::AddDraggableImage(DraggableImage* img) {
    images.push_back(img);
    Refresh();
}

void ImageCanvas::SetSelectedImage(DraggableImage* img) {
    for (size_t i = 0; i < images.size(); ++i) {
        if (images[i] == img) {
            selectedIndex = img->listIndex;
            Refresh();
            return;
        }
    }
}

void ImageCanvas::OnMouseUp(wxMouseEvent& event) {
    if (dragging) {
        dragging     = false;
        draggedIndex = -1;

        ReleaseMouse();
    }
    for (DraggableImage* aImg : images) {
        aImg->selected = false;
    }
    SortImagesByListIndex();
}

void ImageCanvas::OnMouseMove(wxMouseEvent& event) {
    if (dragging && draggedIndex != -1 && event.Dragging() && event.LeftIsDown()) {
        wxPoint         pt    = event.GetPosition();
        wxPoint         delta = pt - dragStartMousePos;
        DraggableImage* img   = nullptr;
        for (DraggableImage* aImg : images) {
            if (aImg->selected) {
                img = aImg;
                break;
            }
        }
        if (!img) return;
        img->pos         = dragStartImgPos + delta;
        wxSize   sz      = GetClientSize();
        wxSize   bmpSize = wxSize(img->data.width, img->data.height);
        wxPoint& pos     = img->pos;
        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;
        if (pos.x + bmpSize.x > sz.x) pos.x = sz.x - bmpSize.x;
        if (pos.y + bmpSize.y > sz.y) pos.y = sz.y - bmpSize.y;

        img->data.x = pos.x;
        img->data.y = pos.y;

        MainFrame* frame = dynamic_cast<MainFrame*>(GetParent());
        if (frame) {
            frame->UnpackImageData(img);
        }
        Refresh();

        // Notify the properties panel about the updated position
        wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
        GetParent()->GetEventHandler()->ProcessEvent(evt);
    }
}

void ImageCanvas::OnMouseRightDown(wxMouseEvent& event) {
    if (selectedIndex == -1) return;
    wxPoint pt = event.GetPosition();

    auto&  img = images[selectedIndex];
    wxRect imgRect(img->pos, img->bitmap.GetSize());
    if (imgRect.Contains(pt)) {
        wxPoint localPt = pt - img->pos;
        wxImage image   = img->bitmap.ConvertToImage();
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