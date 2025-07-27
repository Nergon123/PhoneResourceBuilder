#include "FileProcessor.h"
#include "ImageCanvas.h"

wxString OpenFileDialog(wxWindow* parent, wxString message = "Open Phone Resource File", const char* wildcard = "Phone Resource Files (*" FILE_EXTENSION ")|*" FILE_EXTENSION) {
    wxFileDialog openFileDialog(parent, message, "", "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return wxEmptyString;  // User cancelled the open dialog
    }
    return openFileDialog.GetPath();
}

wxString SaveFileDialog(wxWindow* parent, wxString message = "Save Phone Resource File", const char* wildcard = "Phone Resource Files (*" FILE_EXTENSION ")|*" FILE_EXTENSION) {
    wxFileDialog saveFileDialog(parent, message, "", "", wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return wxEmptyString;  // User cancelled the save dialog
    }
    return saveFileDialog.GetPath();
}

FileProcessing::FileProcessing() {
    currentOffset = 0;
}

void FileProcessing::ExportFile(std::vector<DraggableImage*> ExportImages) {
    currentOffset = 0;  // Reset offset for each export
    std::vector<DraggableImage*> images;
    for (DraggableImage* img : ExportImages) {
        if (!img || !img->includedInFile) {
            continue;
        }
        images.push_back(img);
    }

    globalHeader.imageCount = images.size();
    globalHeader.version    = FILE_VERSION;
    currentOffset += sizeof(Header) + globalHeader.imageCount * sizeof(ImageData);
    dataToExport.clear();
    for (DraggableImage* img : images) {
        img->data.offset = currentOffset;
        printf("Image %d: Offset %u\n", img->data.id, img->data.offset);
        printf("Image %d: Size %ux%u, projected size %u\n\n", img->data.id, img->data.width, img->data.height, img->data.width * img->data.height * 2);

        std::vector<uint16_t> rgb565Data = ConvertWxImageToRGB565(img->bitmap.ConvertToImage());
        currentOffset += rgb565Data.size() * sizeof(uint16_t);
        dataToExport.insert(dataToExport.end(), rgb565Data.begin(), rgb565Data.end());
    }
    wxString fileName = wxEmptyString;
    fileName          = SaveFileDialog(nullptr, "Save Phone Resource File");
    if (!fileName.EndsWith(FILE_EXTENSION)) {
        fileName += FILE_EXTENSION;  // Ensure the file has the correct extension
    }
    std::ofstream file(fileName.ToStdString(), std::ios::binary);
    if (!file) {
        wxLogError("Could not open file for writing: %s", fileName);
        return;
    }

    // Write header
    file.write(reinterpret_cast<const char*>(&globalHeader), sizeof(Header));
    if (!file) {
        wxLogError("Error writing header to file: %s", fileName);
        return;
    }

    // Write image data
    for (const DraggableImage* img : images) {
        file.write(reinterpret_cast<const char*>(&img->data), sizeof(ImageData));
        if (!file) {
            wxLogError("Error writing image data to file: %s", fileName);
            return;
        }
    }
    file.write(reinterpret_cast<const char*>(dataToExport.data()), dataToExport.size() * sizeof(uint16_t));
    if (!file) {
        wxLogError("Error writing image pixel data to file: %s", fileName);
        return;
    }
}

void FileProcessing::ExportImage(DraggableImage* img) {
    wxString filename = SaveFileDialog(nullptr, "Save Image", "Image Files (*.png)|*.png|All Files (*.*)|*.*");
    if (!filename.EndsWith(".png")) {
        filename += ".png";  // Ensure the file has the correct extension
    }
    if (img->bitmap.IsOk()) {
        if (!img->bitmap.SaveFile(filename, wxBITMAP_TYPE_PNG)) {
            wxLogError("Could not save image to file: %s", filename);
            return;
        }
    } else {
        wxLogError("No image data to save.");
        return;
    }
}

void FileProcessing::ExportAllImages(const std::vector<DraggableImage*>& images) {
    wxString filename = SaveFileDialog(nullptr, "Save All Images", "Image Files (*.png)|*.png|All Files (*.*)|*.*");
    if (filename.EndsWith(".png")) {
        filename = filename.BeforeLast('.');  // Remove the extension if it exists
    }
    for (const DraggableImage* img : images) {
        if (img->bitmap.IsOk()) {
            wxString imgFilename = filename + "_" + std::to_string(img->data.id) + ".png";
            if (!img->bitmap.SaveFile(imgFilename, wxBITMAP_TYPE_PNG)) {
                wxLogError("Could not save image to file: %s", imgFilename);
            }
        } else {
            wxLogError("No image data to save for index %d.", img->index);
        }
    }
}

void FileProcessing::ImportFile(std::vector<DraggableImage*>& images) {
    wxString filename = OpenFileDialog(nullptr, "Open Phone Resource File", "Phone Resource Files (*" FILE_EXTENSION ")|*" FILE_EXTENSION);
    if (filename.IsEmpty()) {
        wxLogError("No file selected for importing.");
        return;
    }
    std::ifstream file(filename.ToStdString(), std::ios::binary);
    if (!file) {
        wxLogError("Could not open file for reading: %s", filename);
        return;
    }

    // Read header
    Header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(Header));
    if (!file) {
        wxLogError("Error reading header from file: %s", filename);
        return;
    }

    // Read all ImageData structs first
    images.clear();
    std::vector<ImageData> imgDatas(header.imageCount);
    for (uint16_t i = 0; i < header.imageCount; ++i) {
        file.read(reinterpret_cast<char*>(&imgDatas[i]), sizeof(ImageData));
        if (!file) {
            wxLogError("Error reading image data from file: %s", filename);
            return;
        }
    }

    // Now read all pixel data blocks using offsets
    for (uint16_t i = 0; i < header.imageCount; ++i) {
        const ImageData& imgData = imgDatas[i];

        // print whole ImageData for debugging
        printf("Image %d: ID=%d, Count=%d, Position=(%d, %d), Size=(%d, %d), Offset=%u, Flags=0x%08X\n",
               imgData.id, imgData.id, imgData.count, imgData.x, imgData.y, imgData.width, imgData.height, imgData.offset, imgData.flags);

        // Seek to pixel data for this image
        std::streampos pixelDataPos = imgData.offset;
        file.seekg(pixelDataPos, std::ios::beg);
        if (!file) {
            wxLogError("Error seeking to pixel data for image %d in file: %s", i, filename);
            return;
        }
        std::vector<uint16_t> pixelData(imgData.width * imgData.height);
        file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size() * sizeof(uint16_t));
        if (!file) {
            wxLogError("Error reading pixel data for image %d from file: %s", i, filename);
            return;
        }
        // Convert RGB565 to RGB for wxImage
        std::vector<unsigned char> rgb(imgData.width * imgData.height * 3);
        for (size_t p = 0; p < pixelData.size(); ++p) {
            uint16_t      val = pixelData[p];
            unsigned char r   = ((val >> 11) & 0x1F) << 3;
            unsigned char g   = ((val >> 5) & 0x3F) << 2;
            unsigned char b   = (val & 0x1F) << 3;
            rgb[p * 3 + 0]    = r | (r >> 5);  // fill lower bits
            rgb[p * 3 + 1]    = g | (g >> 6);
            rgb[p * 3 + 2]    = b | (b >> 5);
        }
        wxImage         image(imgData.width, imgData.height, rgb.data(), true);
        wxBitmap        bitmap(image);
        DraggableImage* draggableImage = new DraggableImage(bitmap, wxPoint(imgData.x, imgData.y), imgData, i);
        draggableImage->data           = imgData;
        images.push_back(draggableImage);
    }
}

bool FileProcessing::SaveToFile(const std::vector<DraggableImage*>& images, bool saveas) {
    wxString filename = wxEmptyString;
    if (saveas || filePath.IsEmpty()) {
        filename = SaveFileDialog(nullptr, "Save Project File", "Project Files (*.nprj)|*.nprj|All Files (*.*)|*.*");
        filePath = filename;  // Store the file path for future saves
    } else {
        filename = filePath;  // Use the existing file path
    }

    if (filename.IsEmpty()) return false;
    if (!filename.EndsWith(".nprj")) {
        filename += ".nprj";  // Ensure the file has the correct extension
    }
    std::ofstream out(filename.ToStdString(), std::ios::binary);
    if (!out) return false;

    // Write header
    out.write(reinterpret_cast<const char*>(&globalHeader), sizeof(Header));

    uint32_t imageCount = images.size();
    out.write(reinterpret_cast<const char*>(&imageCount), sizeof(imageCount));

    for (const auto& img : images) {
        // Write ImageData
        out.write(reinterpret_cast<const char*>(&img->data), sizeof(ImageData));

        // Write wxPoint pos
        out.write(reinterpret_cast<const char*>(&img->pos), sizeof(wxPoint));

        // Write index, listIndex, enabled, includedInFile, selected
        out.write(reinterpret_cast<const char*>(&img->index), sizeof(img->index));
        out.write(reinterpret_cast<const char*>(&img->listIndex), sizeof(img->listIndex));
        out.write(reinterpret_cast<const char*>(&img->enabled), sizeof(img->enabled));
        out.write(reinterpret_cast<const char*>(&img->includedInFile), sizeof(img->includedInFile));
        out.write(reinterpret_cast<const char*>(&img->selected), sizeof(img->selected));

        // Write wxSize spriteSheetSize
        out.write(reinterpret_cast<const char*>(&img->spriteSheetSize), sizeof(wxSize));

        // Write indexCoords count + data
        uint32_t indexCount = img->indexCoords.size();
        out.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
        if (indexCount > 0)
            out.write(reinterpret_cast<const char*>(img->indexCoords.data()), indexCount * sizeof(IndexCoords));

        // Save bitmap as RGB image (wxImage)
        wxImage image = img->bitmap.ConvertToImage();
        if (!image.HasAlpha()) image.InitAlpha();
        const unsigned char* rgb   = image.GetData();
        const unsigned char* alpha = image.GetAlpha();

        std::vector<unsigned char> rgba(image.GetWidth() * image.GetHeight() * 4);
        // Apparently GetData has only RGB, without alpha channel
        // So we kinda need to append alpha channel to every pixel
        for (int i = 0, j = 0; i < image.GetWidth() * image.GetHeight(); ++i, j += 4) {
            rgba[j]     = rgb[i * 3 + 0];
            rgba[j + 1] = rgb[i * 3 + 1];
            rgba[j + 2] = rgb[i * 3 + 2];
            rgba[j + 3] = alpha ? alpha[i] : 255;
        }
        // Now write rgba.data(), size = width * height * 4

        uint32_t width  = image.GetWidth();
        uint32_t height = image.GetHeight();
        out.write(reinterpret_cast<const char*>(&width), sizeof(width));
        out.write(reinterpret_cast<const char*>(&height), sizeof(height));
        out.write(reinterpret_cast<const char*>(rgba.data()), width * height * 4);  // RGBA
    }

    return true;
}

bool FileProcessing::LoadFromFile(std::vector<DraggableImage*>& images) {
    wxString filename = OpenFileDialog(nullptr, "Open Project File", "Project Files (*" PROJECT_EXTENSION ")|*" PROJECT_EXTENSION);
    if (filename.IsEmpty()) return false;
    filePath = filename;  // Store the file path for future saves
    std::ifstream in(filename, std::ios::binary);
    if (!in) return false;

    // Read header
    in.read(reinterpret_cast<char*>(&globalHeader), sizeof(Header));

    uint32_t imageCount = 0;
    in.read(reinterpret_cast<char*>(&imageCount), sizeof(imageCount));

    images.clear();
    images.reserve(imageCount);

    for (uint32_t i = 0; i < imageCount; ++i) {
        ImageData data;
        wxPoint   pos;
        uint8_t   index;
        int       listIndex;
        bool      enabled;
        bool      includedInFile;
        bool      selected;
        wxSize    spriteSheetSize;

        // Read ImageData
        in.read(reinterpret_cast<char*>(&data), sizeof(ImageData));

        // Read wxPoint pos
        in.read(reinterpret_cast<char*>(&pos), sizeof(wxPoint));

        // Read index, listIndex, enabled, includedInFile, selected
        in.read(reinterpret_cast<char*>(&index), sizeof(index));
        in.read(reinterpret_cast<char*>(&listIndex), sizeof(listIndex));
        in.read(reinterpret_cast<char*>(&enabled), sizeof(enabled));
        in.read(reinterpret_cast<char*>(&includedInFile), sizeof(includedInFile));
        in.read(reinterpret_cast<char*>(&selected), sizeof(selected));

        // Read wxSize spriteSheetSize
        in.read(reinterpret_cast<char*>(&spriteSheetSize), sizeof(wxSize));

        // Read indexCoords
        uint32_t indexCount = 0;
        in.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        std::vector<IndexCoords> coords(indexCount);
        if (indexCount > 0) {
            in.read(reinterpret_cast<char*>(coords.data()), indexCount * sizeof(IndexCoords));
        }

        // Read wxImage (bitmap) - RGB format
        uint32_t width = 0, height = 0;
        in.read(reinterpret_cast<char*>(&width), sizeof(width));
        in.read(reinterpret_cast<char*>(&height), sizeof(height));

        wxImage image(width, height);  // true = has alpha
        if (!image.HasAlpha()) {
            image.InitAlpha();
        }
        if (width == 0 || height == 0 || width > 4096 || height > 4096) {
            wxLogError("Invalid image size: %ux%u", width, height);
            continue;  // or return false;
        }
        std::vector<unsigned char> rgba(width * height * 4);
        in.read(reinterpret_cast<char*>(rgba.data()), width * height * 4);
        if (!in) {
            wxLogError("Failed to read RGBA data from file.");
            continue;
        }

        for (uint i = 0, j = 0; i < width * height; ++i, j += 4) {
            int x = i % width;
            int y = i / width;
            image.SetRGB(x, y, rgba[j + 0], rgba[j + 1], rgba[j + 2]);
            image.SetAlpha(x, y, rgba[j + 3]);  // Set alpha channel
        }
        wxBitmap bitmap(image);

        // Construct the DraggableImage
        DraggableImage* img  = new DraggableImage(bitmap, pos, data, index, listIndex, enabled, includedInFile);
        img->selected        = selected;
        img->spriteSheetSize = spriteSheetSize;
        img->indexCoords     = std::move(coords);

        images.push_back(img);
    }

    return true;
}

std::vector<uint16_t> FileProcessing::ConvertWxImageToRGB565(const wxImage& image) {
    int                  width   = image.GetWidth();
    int                  height  = image.GetHeight();
    const unsigned char* rgbData = image.GetData();

    std::vector<uint16_t> rgb565Data;
    rgb565Data.reserve(width * height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int     index = (y * width + x) * 3;
            uint8_t r     = rgbData[index];
            uint8_t g     = rgbData[index + 1];
            uint8_t b     = rgbData[index + 2];

            // Convert to RGB565
            uint16_t rgb565 = wxColourToRGB565(wxColor(r, g, b)); 

            rgb565Data.push_back(rgb565);
        }
    }

    return rgb565Data;
}
