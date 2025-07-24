#pragma once
#ifndef IMAGE_CANVAS_H
#define IMAGE_CANVAS_H

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

#define FILE_VERSION      1
#define FILE_EXTENSION    ".nph"
#define PROJECT_EXTENSION ".nprj"

#define DEFAULTS_COUNT 28
uint16_t wxColourToRGB565(const wxColour& col);
wxColour RGB565ToWxColour(uint16_t rgb565);


#pragma pack(push, 1)
struct IndexCoords{
	int id;
	int index;
	int x;
	int y;
};
struct Header {
    const char MAGIC[6] = "NerPh";
    uint8_t    version  = 0;
    uint16_t   imageCount;
    uint16_t   colors[8];
    uint16_t   params[16];
    Header() : MAGIC{'N', 'e', 'r', 'P', 'h', '\0'}, version(0), imageCount(0) {
        for (int i = 0; i < 8; ++i) {
            colors[i] = 0;
        }
        for (int i = 0; i < 16; ++i) {
            params[i] = 0;
        }
    }
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
        : id(0), count(0), x(0), y(0), width(16), height(16), offset(0), flags(0), transpColor(0) {}
    ImageData(uint16_t id, uint8_t count, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t offset, uint8_t flags, uint16_t transpColor)
        : id(id), count(count), x(x), y(y), width(width), height(height), offset(offset), flags(flags), transpColor(transpColor) {}
};

#pragma pack(pop)


extern wxBitmap spriteSheet;
extern const char* ImageNames[DEFAULTS_COUNT];
extern ImageData Defaults[DEFAULTS_COUNT];




struct DraggableImage {
    wxBitmap  bitmap;
    wxPoint   pos;
    ImageData data;
    uint8_t   index          = 0;
    int       listIndex      = 0;
    bool      enabled        = false;
    bool      includedInFile = true;
    bool      selected       = false;
    wxSize    spriteSheetSize;
    std::vector<IndexCoords> indexCoords;
    DraggableImage(wxBitmap bitmap, wxPoint pos, ImageData data, uint8_t index = 0, int listIndex = 0, bool enabled = false, bool includedInFile = true) : bitmap(bitmap), pos(pos), data(data), index(index), listIndex(listIndex), enabled(enabled), includedInFile(includedInFile) {}
};




// The ImageCanvas class




class ImageCanvas : public wxPanel {
   public:
    ImageCanvas(wxWindow* parent);
    ~ImageCanvas();
    int                                 GetImageCount() const;
    const std::vector<DraggableImage*>& GetImages() const;
    int                                 GetSelectedImageIndex() const;
    void                                SetSelectedImageIndex(int index);
    void                                SetImagesOrder(const std::vector<int>& order);
    void                                AddImage(DraggableImage* image);
    void                                RefreshSelectedImage();
    DraggableImage*                     GetSelectedImage();
    wxBitmap                            GetUnderlyingBitmap(const wxPoint& pos, const wxSize& size);
    void                                AddDraggableImage(DraggableImage* img);
    void                                SetSelectedImage(DraggableImage* img);
    std::vector<DraggableImage*>        images;
    void                                SortImagesByListIndex();
    void                                UpdateListIndices();

   protected:
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);

   private:

    bool    dragging = false;
    wxPoint dragStartMousePos;
    wxPoint dragStartImgPos;
    int     draggedIndex  = -1;
    int     selectedIndex = -1;

    void SetImageFrameIndex(int imgIndex, uint8_t newFrameIndex);
    void AddAllFrames(const ImageData& d, const wxBitmap& fullBitmap, const wxPoint& basePos);
};

#endif // IMAGE_CANVAS_H
