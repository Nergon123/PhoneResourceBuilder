#include "MainFrame.h"

MainFrame::MainFrame(wxWindow* parent) : MyFrame1(parent) {
    // Replace the FormBuilder-generated panel with our custom canvas
    wxSizer*  sizer     = m_canvasPanel->GetContainingSizer();
    wxWindow* parentWin = m_canvasPanel->GetParent();

    // Remove and destroy original panel
    sizer->Detach(m_canvasPanel);
    m_canvasPanel->Destroy();

    canvas = new ImageCanvas(parentWin);
    canvas->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    canvas->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));
    canvas->SetMinSize(wxSize(240, 320));
    canvas->SetMaxSize(wxSize(240, 320));
    DisableControls();
    sizer->Add(canvas, 1, wxEXPAND | wxALL, 20);

    Layout();  // Refresh layout
}

DraggableImage* MainFrame::GetSelectedImage() {
    if (!currentImagesCheckList || currentImagesCheckList->GetCount() == 0) {
        return nullptr;
    }
    int sel = currentImagesCheckList->GetSelection();
    if (sel == wxNOT_FOUND)
        return nullptr;
    DraggableImage* img = static_cast<DraggableImage*>(currentImagesCheckList->GetClientData(sel));
    return img;
}

void MainFrame::UnpackImageData(DraggableImage* img) {
    ImageData* data = &img->data;
    int        id   = data->id;
    Description->SetLabelText(id >= 0 && id < (int)(sizeof(ImageNames) / sizeof(ImageNames[0]))
                                  ? ImageNames[id]
                                  : "Unknown/Custom");
    IDCtrl->SetValue(id);
    CountControl->SetValue(data->count);
    IndexControl->SetValue(0);
    IndexControl->SetRange(0, data->count - 1);
    XposControl->SetValue(data->x);
    YposControl->SetValue(data->y);
    WidthControl->SetValue(data->width);
    HeightControl->SetValue(data->height);
    TransCBox->SetValue(data->flags & 1);
    transpSlider->Enable(!img->includedInFile);
    includeInExport->SetValue(img->includedInFile);
    nameTextCtrl->SetValue(GetImageName(img));
    RGBStatus->SetLabelText(wxString::Format("RGB565 0x%04x", data->transpColor));
    ColorPicker->SetColour(RGB565ToWxColour(data->transpColor));
}

wxCheckListBox* MainFrame::GetList() {
    return currentImagesCheckList;
}

void MainFrame::EnableControls() {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    EnableControlsInSizer(bSizer5);
    nameTextCtrl->Enable();
}

void MainFrame::DisableControls() {
    DisableControlsInSizer(bSizer5);
    nameTextCtrl->Disable();
}

void MainFrame::OnImagesListToggle(wxCommandEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    img->enabled = currentImagesCheckList->IsChecked(img->listIndex);
    Refresh();
}

void MainFrame::OnClkUpList(wxCommandEvent& event) {
    MoveSelectedItemUp(currentImagesCheckList);
    Refresh();
}

void MainFrame::OnClkDownList(wxCommandEvent& event) {
    MoveSelectedItemDown(currentImagesCheckList);
    Refresh();
}

void MainFrame::OnClkBindList(wxCommandEvent& event) {
    bool            isShifted = wxGetKeyState(WXK_SHIFT);
    DraggableImage* img       = GetSelectedImage();
    if (!img) return;

    if (isShifted) {
        int      id        = img->data.id;
        wxBitmap newBitmap = img->bitmap;
        int      answer    = wxID_NO;
        if (img->data.count > 1) {
            wxMessageDialog dlg(
                this,
                wxString::Format("Seems that image have %d subimages.\n\nDo you want to load every image separately or one vertically stacked spritesheet?\n\nYou need %d images with %dx%d resolution of or one spritesheet with %dx%d resolution?\n\nResizing also supported. ", img->data.count, img->data.count, img->data.width, img->data.height, img->data.width, img->data.height * img->data.count),
                "Confirm spritesheet",
                wxYES_NO | wxCANCEL | wxICON_QUESTION);

            dlg.SetYesNoCancelLabels("Separately", "Spritesheet", "Cancel");

            int answer = dlg.ShowModal();
            if (answer == wxID_YES) {
                newBitmap = AskAndStackImages(img->data.count, wxSize(img->data.width, img->data.height));
            } else if (answer == wxID_CANCEL) {
                return;
            }
        }
        if (answer == wxID_NO) {
            newBitmap = LoadBitmap(wxString::Format("Load image for [%d] %s", id,
                                                    (id >= 0 && id < (int)(sizeof(ImageNames) / sizeof(ImageNames[0]))) ? ImageNames[id] : "New Image"),
                                   img->bitmap, wxSize(img->bitmap.GetWidth(), img->bitmap.GetHeight()));
        }
        if (!newBitmap.IsOk()) {
            wxMessageBox("Invalid Image.");
            return;
        }
        img->data.width  = newBitmap.GetWidth();
        img->data.height = newBitmap.GetHeight() / img->data.count;
        img->bitmap      = newBitmap;
    } else {
        SpriteForm* spriteForm = new SpriteForm(this, spriteSheet, img, this /*bruh ("-_-)*/);
        if (spriteForm) {
            spriteForm->Show();
        } else {
            wxLogError("Failed to create SpriteForm.");
        }
    }
    img->enabled = currentImagesCheckList->IsChecked(img->listIndex);
    Refresh();
}

void MainFrame::OnExportAllPNG(wxCommandEvent& event) {
    fileProcessor.ExportAllImages(canvas->images);
}

void MainFrame::OnExport(wxCommandEvent& event) {
    fileProcessor.ExportFile(canvas->images);
}

void MainFrame::OnExportPNG(wxCommandEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    fileProcessor.ExportImage(img);
}

void MainFrame::OnClkNewList(wxCommandEvent& event) {
    if (wxGetKeyState(WXK_SHIFT)) {
        // If Shift is pressed, create a new image with the same properties as the selected one
        DraggableImage* img = GetSelectedImage();
        if (!img) return;
        DraggableImage* newImg = new DraggableImage(*img);
        int             id     = newImg->data.id;
        currentImagesCheckList->Append(wxString::Format("[%d] %s", id, GetImageName(newImg)), newImg);  // Attach client data
        currentImagesCheckList->Check(currentImagesCheckList->GetCount() - 1, img->enabled);
        canvas->AddDraggableImage(newImg);
    } else {
        int             id     = currentImagesCheckList->GetCount();
        ImageData       data   = ImageData((id >= 0 && id < (int)(sizeof(ImageNames) / sizeof(ImageNames[0]))) ? Defaults[id] : ImageData(id));
        DraggableImage* idData = new DraggableImage(wxBitmap(data.width, data.height * data.count), wxPoint(0, 0), data, 0, id);
        currentImagesCheckList->Append(wxString::Format("[%d] %s", id, GetImageName(idData)), idData);  // Attach client data
        canvas->AddDraggableImage(idData);
    }
    Refresh();
}

void MainFrame::OnTextChange(wxCommandEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) {
        nameTextCtrl->SetValue(wxEmptyString);
        return;
    }
    if (event.GetString().IsEmpty()) {
        strncpy(img->data.name, img->data.id > 0 && img->data.id < (int)(sizeof(ImageNames) / sizeof(ImageNames[0])) ? ImageNames[img->data.id] : "Unnamed/Custom", sizeof(img->data.name) - 1);
        return;
    }
    strncpy(img->data.name, event.GetString().ToStdString().c_str(), sizeof(img->data.name) - 1);

    RenameSelectedItem(currentImagesCheckList, wxString::Format("[%d] %s", img->data.id, GetImageName(img)));
    canvas->Refresh();
    Refresh();
}

void MainFrame::OnImagesListSelect(wxCommandEvent& event) {
    int sel = currentImagesCheckList->GetSelection();
    if (sel != wxNOT_FOUND) {
        DraggableImage* img = static_cast<DraggableImage*>(currentImagesCheckList->GetClientData(sel));
        if (img) {
            EnableControls();
        } else {
            return;
        }
        img->listIndex = sel;
        canvas->SetSelectedImage(img);
        UnpackImageData(img);

    } else {
        DisableControls();
    }
    Refresh();
}

void MainFrame::OnClkDelList(wxCommandEvent& event) {
    int sel = currentImagesCheckList->GetSelection();
    if (sel != wxNOT_FOUND) {
        auto            it   = canvas->images.begin() + sel;
        DraggableImage* data = *it;

        canvas->images.erase(it);
        delete data;  // free memory

        currentImagesCheckList->Delete(sel);  // update checklistbox UI
    }
    sel = currentImagesCheckList->GetSelection();
    if (sel == wxNOT_FOUND) {
        DisableControls();
    }
    canvas->Refresh();  // repaint images without the deleted one
}

void MainFrame::OnUpdateID(wxSpinEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    int id       = IDCtrl->GetValue();
    img->data.id = id;
    if (wxGetKeyState(WXK_SHIFT)) {
        if (id >= 0 && (ulong)id < sizeof(ImageNames) / sizeof(ImageNames[0])) {
            Defaults[img->data.id] = img->data;
            img->data              = Defaults[img->data.id];
        };

        img->data   = (id >= 0 && id < (int)(sizeof(ImageNames) / sizeof(ImageNames[0]))) ? Defaults[id] : ImageData(id, 1, 0, 0, 16, 16, 0, 0, 0);
        img->pos    = wxPoint(img->data.x, img->data.y);
        img->bitmap = wxBitmap(img->data.width, img->data.height);
    }
    UnpackImageData(img);
    Description->SetLabelText(GetImageName(nullptr, id));
    RenameSelectedItem(currentImagesCheckList, wxString::Format("[%d] %s", img->data.id, GetImageName(img)));
    Refresh();
}

wxString MainFrame::GetImageName(DraggableImage* img /* = nullptr */, int id) {
    if (!img && id == -1) img = GetSelectedImage();
    if (!img && id == -1) return "Unknown/Custom";
    if (img && img->data.name[0] != '\0') {
        return wxString::FromUTF8(img->data.name);
    }
    int index = img ? img->data.id : id;
    if (index >= 0 && index < (int)(sizeof(ImageNames) / sizeof(ImageNames[0]))) {
        return ImageNames[index];
    }
    return "Unknown/Custom";
}

void MainFrame::OnUpdateCount(wxSpinEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    img->data.count = CountControl->GetValue();
    if (img->data.id >= 0 && img->data.id < sizeof(ImageNames) / sizeof(ImageNames[0])) {
        Defaults[img->data.id] = img->data;
    };

    Refresh();
}

void MainFrame::OnIncludeInFileBox(wxCommandEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    img->includedInFile = event.IsChecked();
    transpSlider->Enable(!img->includedInFile);
    if (img->includedInFile) {
        SetAlphaOfImage(img->bitmap, 255);
    }
    Refresh();
}

void MainFrame::SetAlphaOfImage(wxBitmap& image, uint8_t transpValue) {
    if (!image.IsOk() || transpValue < 5 /*For some reason its becomes fully black when at 0*/) return;
    wxImage Eimage = image.ConvertToImage();
    if (!Eimage.HasAlpha()) {
        Eimage.InitAlpha();
    }
    for (int y = 0; y < Eimage.GetHeight(); ++y) {
        for (int x = 0; x < Eimage.GetWidth(); ++x) {
            // This will destroy alpha channel...
            // Actually for some reason it slowly destroys whole image lol
            // Use this only to compare positions with original image
            Eimage.SetAlpha(x, y, transpValue);
        }
    }
    image = wxBitmap(Eimage);
}

void MainFrame::OnTranspSlider(wxScrollEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img || img->includedInFile) return;
    SetAlphaOfImage(img->bitmap, event.GetInt());
    canvas->Refresh();
}

void MainFrame::OnUpdateIndex(wxSpinEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    img->index = IndexControl->GetValue();
    if (img->data.id >= 0 && img->data.id < sizeof(ImageNames) / sizeof(ImageNames[0])) {
        Defaults[img->data.id] = img->data;
    };
    Refresh();
}

void MainFrame::OnUpdateX(wxSpinEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    img->data.x = XposControl->GetValue();
    if (img->data.id >= 0 && img->data.id < sizeof(ImageNames) / sizeof(ImageNames[0])) {
        Defaults[img->data.id] = img->data;
    };
    Refresh();
}

void MainFrame::OnUpdateY(wxSpinEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    img->data.y = YposControl->GetValue();
    if (img->data.id >= 0 && img->data.id < sizeof(ImageNames) / sizeof(ImageNames[0])) {
        Defaults[img->data.id] = img->data;
    };
    Refresh();
}

void MainFrame::OnUpdateWidth(wxSpinEvent& event) {
    DraggableImage* img = GetSelectedImage();

    if (!img) return;

    img->data.width = WidthControl->GetValue();
    if (img->data.id >= 0 && img->data.id < sizeof(ImageNames) / sizeof(ImageNames[0])) {
        Defaults[img->data.id] = img->data;
    };

    int frameCount = img->data.count > 0 ? img->data.count : 1;
    int newWidth   = img->data.width;
    int newHeight  = img->data.height * frameCount;

    // Rescale the entire bitmap
    // TODO: This has the power to destroy the image

    wxImage scaled = img->bitmap.ConvertToImage().Rescale(newWidth, newHeight, wxIMAGE_QUALITY_HIGH);
    img->bitmap    = wxBitmap(scaled);

    Refresh();  // Refresh canvas or relevant control
}

void MainFrame::OnUpdateHeight(wxSpinEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;

    img->data.height = HeightControl->GetValue();
    if (img->data.id >= 0 && img->data.id < sizeof(ImageNames) / sizeof(ImageNames[0])) {
        Defaults[img->data.id] = img->data;
    };

    int frameCount = img->data.count > 0 ? img->data.count : 1;
    int newWidth   = img->data.width;
    int newHeight  = img->data.height * frameCount;

    // Rescale the entire bitmap (all frames vertically)
    // TODO: This has the power to destroy the image

    wxImage scaled = img->bitmap.ConvertToImage().Rescale(newWidth, newHeight, wxIMAGE_QUALITY_HIGH);
    img->bitmap    = wxBitmap(scaled);

    Refresh();  // Refresh canvas or relevant control
}

void MainFrame::OnHardBake(wxCommandEvent& event) {
    // this will replace all pixels with alpha < 128 with transparent color
    DraggableImage* img = GetSelectedImage();

    if (!img || !img->enabled) return;
    if (!(img->data.flags & 1)) {
        wxMessageBox("Selected image is not marked as transparent.");
        return;
    }
    wxImage imgData = img->bitmap.ConvertToImage();
    if (!imgData.HasAlpha()) {
        wxMessageBox("Selected image has no transparency to bake.");
        return;
    }

    const ImageData& region = img->data;
    const int        width  = region.width;
    const int        height = region.height * region.count;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (x < 0 || y < 0 || x >= imgData.GetWidth() || y >= imgData.GetHeight())
                continue;
            if (imgData.GetAlpha(x, y) < 128) {
                wxColor transparentColor = ColorPicker->GetColour();
                imgData.SetRGB(x, y, transparentColor.Red(), transparentColor.Green(), transparentColor.Blue());
            }
            imgData.SetAlpha(x, y, 255);  // Set all pixels to fully opaque. I don't think that phone will support alpha channel in near future
        }
    }
    img->bitmap = wxBitmap(imgData);
    Refresh();
};

void MainFrame::OnBakeImage(wxCommandEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img || !img->enabled) return;

    const ImageData& region = img->data;
    wxImage          imgData;
    if (imgData.IsOk()) {
        imgData.Clear();  // For some reason this thing without this line is taking previous image data
    }
    wxBitmap bmp = img->bitmap.GetSubBitmap(wxRect(0, region.height * IndexControl->GetValue(), region.width, region.height));
    imgData      = bmp.ConvertToImage();

    if (!imgData.HasAlpha()) {
        wxMessageBox("Selected image has no transparency to bake.");
        return;
    }

    const int width  = region.width;
    const int height = region.height;

    std::vector<DraggableImage*> underImages;
    for (DraggableImage* otherImg : canvas->images) {
        if (!otherImg || otherImg == img || !otherImg->enabled) continue;
        if (otherImg->listIndex < img->listIndex &&
            img->pos.x < otherImg->pos.x + otherImg->data.width &&
            img->pos.x + region.width > otherImg->pos.x &&
            img->pos.y < otherImg->pos.y + otherImg->data.height &&
            img->pos.y + region.height > otherImg->pos.y) {
            underImages.push_back(otherImg);
        }
    }
    // Sort by listIndex ascending (bottom to top)
    std::sort(underImages.begin(), underImages.end(),
              [](DraggableImage* a, DraggableImage* b) { return a->listIndex < b->listIndex; });
    bool empty = false;
    if (underImages.empty()) {
        empty = true;
    }

    wxImage bakedBackground(width, height, true);
    bakedBackground.SetRGB(wxRect(0, 0, width, height), 0, 0, 0);
    bakedBackground.InitAlpha();
    unsigned char* alpha = bakedBackground.GetAlpha();
    unsigned char  a     = empty ? 255 : 0;
    if (alpha) {
        for (int i = 0; i < width * height; ++i) {
            alpha[i] = a;
        }
    }

    for (DraggableImage* underImg : underImages) {
        wxImage          srcImage  = underImg->bitmap.ConvertToImage();
        const ImageData& srcRegion = underImg->data;
        wxPoint          offset    = underImg->pos - img->pos;  // Relative position

        for (int y = 0; y < srcRegion.height; ++y) {
            for (int x = 0; x < srcRegion.width; ++x) {
                int srcX = srcRegion.x + x;
                int srcY = srcRegion.y + y;
                int dstX = offset.x + x + srcRegion.x;
                int dstY = offset.y + y + srcRegion.y;

                if (srcX < 0 || srcY < 0 || srcX >= srcImage.GetWidth() || srcY >= srcImage.GetHeight())
                    continue;
                if (dstX < 0 || dstY < 0 || dstX >= width || dstY >= height)
                    continue;

                unsigned char srcR = srcImage.GetRed(srcX, srcY);
                unsigned char srcG = srcImage.GetGreen(srcX, srcY);
                unsigned char srcB = srcImage.GetBlue(srcX, srcY);
                unsigned char srcA = srcImage.HasAlpha() ? srcImage.GetAlpha(srcX, srcY) : 255;

                unsigned char dstR = bakedBackground.GetRed(dstX, dstY);
                unsigned char dstG = bakedBackground.GetGreen(dstX, dstY);
                unsigned char dstB = bakedBackground.GetBlue(dstX, dstY);
                unsigned char dstA = bakedBackground.GetAlpha(dstX, dstY);

                float srcAlpha = srcA / 255.0f;
                float dstAlpha = dstA / 255.0f;
                float outAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);

                unsigned char outR, outG, outB, outA;
                if (outAlpha > 0.0f) {
                    outR = static_cast<unsigned char>((srcR * srcAlpha + dstR * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    outG = static_cast<unsigned char>((srcG * srcAlpha + dstG * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    outB = static_cast<unsigned char>((srcB * srcAlpha + dstB * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    outA = static_cast<unsigned char>(outAlpha * 255);
                } else {
                    outR = outG = outB = 0;
                    outA               = 0;
                }

                bakedBackground.SetRGB(dstX, dstY, outR, outG, outB);
                bakedBackground.SetAlpha(dstX, dstY, outA);
            }
        }
    }

    // 4. Blend bakedBackground into all non-fully-opaque pixels of selected image
    for (int y = 0; y < region.height; ++y) {
        for (int x = 0; x < region.width; ++x) {
            int imgX = x;
            int imgY = y;
            if (imgX < 0 || imgY < 0 || imgX >= imgData.GetWidth() || imgY >= imgData.GetHeight())
                continue;
            unsigned char fgR = imgData.GetRed(imgX, imgY);
            unsigned char fgG = imgData.GetGreen(imgX, imgY);
            unsigned char fgB = imgData.GetBlue(imgX, imgY);
            unsigned char fgA = imgData.HasAlpha() ? imgData.GetAlpha(imgX, imgY) : 255;
            if (fgA == 255) continue;  // fully opaque, leave as is
            unsigned char bgR      = bakedBackground.GetRed(x, y);
            unsigned char bgG      = bakedBackground.GetGreen(x, y);
            unsigned char bgB      = bakedBackground.GetBlue(x, y);
            unsigned char bgA      = bakedBackground.GetAlpha(x, y);
            float         fgAlpha  = fgA / 255.0f;
            float         bgAlpha  = bgA / 255.0f;
            float         outAlpha = fgAlpha + bgAlpha * (1.0f - fgAlpha);
            unsigned char outR, outG, outB, outA;
            if (outAlpha > 0.0f) {
                outR = static_cast<unsigned char>((fgR * fgAlpha + bgR * bgAlpha * (1.0f - fgAlpha)) / outAlpha);
                outG = static_cast<unsigned char>((fgG * fgAlpha + bgG * bgAlpha * (1.0f - fgAlpha)) / outAlpha);
                outB = static_cast<unsigned char>((fgB * fgAlpha + bgB * bgAlpha * (1.0f - fgAlpha)) / outAlpha);
                outA = static_cast<unsigned char>(outAlpha * 255);
            } else {
                outR = outG = outB = 0;
                outA               = 0;
            }
            imgData.SetRGB(imgX, imgY, outR, outG, outB);
            imgData.SetAlpha(imgX, imgY, outA);
        }
    }

    // Update bitmap and refresh canvas
    img->bitmap = wxBitmap(imgData);
    canvas->Refresh();

    wxMessageBox("Image baked successfully.");
}
bool MainFrame::CheckTransparentImages() {
    wxString Names = wxEmptyString;
    for (DraggableImage* img : canvas->images) {
        if (HasActualTransparency(img->bitmap) && img->includedInFile) {
            Names += wxString::Format(" [%d] %s\n", img->data.id, img->data.name);
        }
    }
    if (!Names.empty()) {
        wxMessageDialog dlg(this, wxString::Format("The following images have transparency:\n%s \n Please bake them.",Names), "Warning!", wxYES_NO | wxCENTER | wxICON_QUESTION);
        dlg.SetYesNoLabels("Ok", "Proceed Anyway");
        int result = dlg.ShowModal();
        if (result == wxID_YES) {
            return true;
        } else if (result == wxID_NO) {
            return false;
        }else {
            return true;
        }
    }

    return false; 
}
bool MainFrame::HasActualTransparency(const wxBitmap& bmp) {
    if (!bmp.HasAlpha()) return false;

    wxImage img = bmp.ConvertToImage();
    if (!img.HasAlpha()) return false;

    const unsigned char* alpha = img.GetAlpha();
    int numPixels = img.GetWidth() * img.GetHeight();

    for (int i = 0; i < numPixels; ++i) {
        if (alpha[i] < 255) {
            return true;
        }
    }

    return false;
}
void MainFrame::OnTransColorChanged(wxColourPickerEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    img->data.transpColor = wxColourToRGB565(ColorPicker->GetColour());
}

void MainFrame::OnUpdateTrans(wxCommandEvent& event) {
    DraggableImage* img = GetSelectedImage();
    if (!img) return;
    TransCBox->IsChecked() ? img->data.flags |= 1 : img->data.flags &= ~1;
}

void MainFrame::OnClickPICK(wxCommandEvent& event) { event.Skip(); }

void MainFrame::OnOpen(wxCommandEvent& event) {
    fileProcessor.LoadFromFile(canvas->images);
    printf("Loaded %zu images from file.\n", canvas->images.size());
    PopulateList(currentImagesCheckList, canvas->images);
    canvas->Refresh();
}

void MainFrame::OnSave(wxCommandEvent& event) {
    if (fileProcessor.SaveToFile(canvas->images)) {
        wxLogMessage("Project saved successfully.");
    } else {
        wxLogError("Failed to save project.");
    }
}

void MainFrame::OnSaveAs(wxCommandEvent& event) {
    if (fileProcessor.SaveToFile(canvas->images, true)) {
        wxLogMessage("Project saved successfully.");
    } else {
        wxLogError("Failed to save project.");
    }
}

void MainFrame::OnImport(wxCommandEvent& event) {
    fileProcessor.ImportFile(canvas->images);
    PopulateList(currentImagesCheckList, canvas->images);
    canvas->Refresh();
}

void MainFrame::OnClose(wxCloseEvent& event) {
    Exit();
}

void MainFrame::OnExitMenu(wxCommandEvent& event) {
    Exit();
}

void MainFrame::Exit() {
    if (wxMessageBox("Are you sure you want to exit?\nAll unsaved changes will be lost.", "Confirm Exit", wxYES_NO | wxICON_QUESTION) == wxYES) {
        exit(0);
    }
}

void MainFrame::PopulateList(wxCheckListBox* list, const std::vector<DraggableImage*>& images) {
    ClearList(list);  // Clear existing items

    for (size_t i = 0; i < images.size(); ++i) {
        DraggableImage* img = images[i];
        img->listIndex      = i;  // Set listIndex to match the order in the list
        list->Append(wxString::Format("[%d] %s", img->data.id, (img->data.id >= 0 && img->data.id < (int)(sizeof(ImageNames) / sizeof(ImageNames[0]))) ? ImageNames[img->data.id] : "New Image"), img);
        list->Check(i, img->enabled);  // Set checked state based on enabled field
    }
}

void MainFrame::SwapItems(wxCheckListBox* list, int i, int j) {
    wxString label_i = list->GetString(i);
    wxString label_j = list->GetString(j);

    void* data_i = list->GetClientData(i);
    void* data_j = list->GetClientData(j);

    bool checked_i = list->IsChecked(i);
    bool checked_j = list->IsChecked(j);
    list->Check(i, checked_j);
    list->Check(j, checked_i);

    list->SetString(i, label_j);
    list->SetString(j, label_i);

    list->SetClientData(i, data_j);
    list->SetClientData(j, data_i);
}

void MainFrame::MoveSelectedItemUp(wxCheckListBox* list) {
    int sel = list->GetSelection();
    if (sel > 0) {
        SwapItems(list, sel, sel - 1);
        list->SetSelection(sel - 1);

        // Update both swapped items' listIndex fields
        DraggableImage* img1 = static_cast<DraggableImage*>(list->GetClientData(sel - 1));
        DraggableImage* img2 = static_cast<DraggableImage*>(list->GetClientData(sel));
        if (img1) img1->listIndex = sel - 1;
        if (img2) img2->listIndex = sel;
        canvas->SetSelectedImageIndex(img1->listIndex);
    }
}

void MainFrame::MoveSelectedItemDown(wxCheckListBox* list) {
    int sel = list->GetSelection();
    if (sel != wxNOT_FOUND && sel < (int)list->GetCount() - 1) {
        SwapItems(list, sel, sel + 1);
        list->SetSelection(sel + 1);

        DraggableImage* img1 = static_cast<DraggableImage*>(list->GetClientData(sel));
        DraggableImage* img2 = static_cast<DraggableImage*>(list->GetClientData(sel + 1));
        if (img1) img1->listIndex = sel;
        if (img2) img2->listIndex = sel + 1;
        canvas->SetSelectedImageIndex(img2->listIndex);
    }
}

void MainFrame::ClearList(wxCheckListBox* list) {
    for (unsigned int i = 0; i < list->GetCount(); ++i) {
        DraggableImage* img = static_cast<DraggableImage*>(list->GetClientData(i));
        if (img) {
            delete img;
        }
    }
    list->Clear();
}

void MainFrame::RenameSelectedItem(wxCheckListBox* list, const wxString& newLabel) {
    int sel = list->GetSelection();
    if (sel == wxNOT_FOUND) return;

    // Preserve state
    bool  checked = list->IsChecked(sel);
    void* data    = list->GetClientData(sel);  // Or GetClientData(sel) if using void*

    // Delete and re-insert
    list->Delete(sel);
    list->Insert(newLabel, sel, data);
    list->Check(sel, checked);
    list->SetSelection(sel);
}

wxBitmap MainFrame::AskAndStackImages(int n, wxSize targetSize) {
    std::vector<wxBitmap> images;
    wxBitmap              lastBitmap;
    bool                  forceResize = false;
    for (int i = 0; i < n; ++i) {
        wxString title     = wxString::Format("Select image %d of %d", i + 1, n);
        wxBitmap newBitmap = LoadBitmap(title.c_str(), lastBitmap, targetSize, n == 0 ? false : true, &forceResize);

        if (!newBitmap.IsOk()) {
            wxLogWarning("Image load canceled or failed. Stopping.");
            return wxBitmap();
            break;
        }

        images.push_back(newBitmap);
        lastBitmap = newBitmap;
    }

    if (images.empty()) return wxBitmap();  // No images loaded

    // Calculate combined size
    int totalHeight = 0;
    int maxWidth    = 0;
    for (const auto& bmp : images) {
        totalHeight += bmp.GetHeight();
        if (bmp.GetWidth() > maxWidth)
            maxWidth = bmp.GetWidth();
    }

    wxBitmap   combined(maxWidth, totalHeight);
    wxMemoryDC dc(combined);
    dc.Clear();

    int y = 0;
    for (const auto& bmp : images) {
        dc.DrawBitmap(bmp, 0, y, true);
        y += bmp.GetHeight();
    }

    dc.SelectObject(wxNullBitmap);  // Detach

    return combined;
}

wxBitmap MainFrame::LoadBitmap(const char* title, const wxBitmap& bitmapBefore, wxSize targetSize, bool forceContinue, bool* forceResize) {
    wxFileDialog openFileDialog(
        this,   // parent
        title,  // title
        "",     // default dir
        "",     // default file
        "Image files (*.png;*.jpg)|*.png;*.jpg|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return bitmapBefore;  // User canceled

    wxString path = openFileDialog.GetPath();

    if (path.EndsWith(".png") || path.EndsWith(".jpg")) {
        wxBitmap bitmap;
        if (!bitmap.LoadFile(path, wxBITMAP_TYPE_ANY)) {
            wxLogError("Failed to load image.");
            return bitmapBefore;
        }

        wxLogMessage("Loaded image: %s", path);

        if (bitmapBefore.IsOk() && bitmap.GetSize() != targetSize) {
            wxSize before = targetSize;
            wxSize after  = bitmap.GetSize();
            int    answer = wxID_CANCEL;
            if (!forceContinue) {
                wxMessageDialog dlg(
                    this,
                    wxString::Format("Size differs!\n\nBefore: %dx%d\nAfter: %dx%d\n\nResize to %dx%d?",
                                     before.x, before.y, after.x, after.y, before.x, before.y),
                    "Confirm Resize",
                    wxYES_NO | wxCANCEL | wxICON_QUESTION);

                dlg.SetYesNoCancelLabels("Resize", "Keep Original", "Cancel");

                answer = dlg.ShowModal();
            } else {
                if (forceResize) {
                    answer = *forceResize ? wxID_YES : wxID_NO;
                } else {
                    answer = wxID_NO;
                }
            }
            if (answer == wxID_YES) {
                if (forceResize) {
                    *forceResize = true;
                }
                wxImage img = bitmap.ConvertToImage();
                bitmap      = wxBitmap(img.Rescale(before.x, before.y, wxIMAGE_QUALITY_HIGH));
                return bitmap;
            } else if (answer == wxID_NO) {
                if (bitmap.GetHeight() > 320 || bitmap.GetWidth() > 240) {
                    wxMessageBox("Image exceeds size of display.");
                    return bitmapBefore;
                }
                if (forceResize) {
                    *forceResize = false;
                }
                return bitmap;
            } else {
                return bitmapBefore;  // Cancel load
            }
        }

        return bitmap;  // Sizes are same
    }

    return bitmapBefore;  // Unsupported extension
}

wxBitmap MainFrame::CombineBitmapsVertically(const std::vector<wxBitmap>& bitmaps) {
    if (bitmaps.empty()) return wxBitmap();

    // Calculate total height and max width
    int totalHeight = 0;
    int maxWidth    = 0;
    for (const auto& bmp : bitmaps) {
        if (!bmp.IsOk()) continue;
        totalHeight += bmp.GetHeight();
        if (bmp.GetWidth() > maxWidth)
            maxWidth = bmp.GetWidth();
    }

    // Create a new bitmap with the combined size
    wxBitmap   combined(maxWidth, totalHeight);
    wxMemoryDC memDC(combined);
    memDC.Clear();  // Optional: clear background to white/black

    // Draw each bitmap one below another
    int y = 0;
    for (const auto& bmp : bitmaps) {
        if (!bmp.IsOk()) continue;
        memDC.DrawBitmap(bmp, 0, y, true);  // Draw with transparency
        y += bmp.GetHeight();
    }

    memDC.SelectObject(wxNullBitmap);  // Detach
    return combined;
}

void MainFrame::DisableControlsInSizer(wxSizer* sizer) {
    if (!sizer) return;
    for (auto& item : sizer->GetChildren()) {
        if (wxWindow* window = item->GetWindow()) {
            window->Disable();
        }
        if (wxSizer* childSizer = item->GetSizer()) {
            DisableControlsInSizer(childSizer);
        }
    }
}

void MainFrame::EnableControlsInSizer(wxSizer* sizer) {
    if (!sizer) return;
    for (auto& item : sizer->GetChildren()) {
        if (wxWindow* window = item->GetWindow()) {
            window->Enable();
        }
        if (wxSizer* childSizer = item->GetSizer()) {
            EnableControlsInSizer(childSizer);
        }
    }
}
