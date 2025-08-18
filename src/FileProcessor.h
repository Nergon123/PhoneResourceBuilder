#pragma once
#include "ImageCanvas.h"
#include <vector>
#include <fstream>
#include <wx/wx.h>

wxString SaveFileDialog(wxWindow* parent, wxString message, const char* wildcard );
wxString OpenFileDialog(wxWindow* parent, wxString message, const char* wildcard );
class FileProcessing {
public:
    std::vector<DraggableImage*> imagesToExport;
    std::vector<uint8_t> dataToExport;

    FileProcessing();

    void ExportFile(std::vector<DraggableImage*> images);
    void ExportImage(DraggableImage* img);
    void ExportAllImages(const std::vector<DraggableImage*>& images);
    void ImportFile(std::vector<DraggableImage*>& images);
    bool SaveToFile(const std::vector<DraggableImage*>& images, bool saveas = false);
    bool LoadFromFile(std::vector<DraggableImage*>& images);
    std::vector<uint8_t> ConvertWxImageToRGB565(const wxImage& image);
    private:
    uint32_t                   currentOffset = 0;
    Header                      globalHeader;
    wxString                   filePath;
};