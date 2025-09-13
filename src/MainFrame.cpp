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
            if (imgData.GetAlpha(x, y) < 64) {
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

    const wxRect reg(img->pos.x, img->pos.y, img->data.width, img->bitmap.GetHeight());
    wxBitmap     bitImageResult(reg.width, reg.height);
    wxImage      imageResult = bitImageResult.ConvertToImage();
    // imageResult.Create(reg.width,reg.height);
    imageResult.InitAlpha();

    //  //printf("out of first 'for'\n");
    for (int fr = 0; fr < img->data.count; fr++) {
        for (int i = 0; i <= img->listIndex; i++) {
            DraggableImage* currImg = static_cast<DraggableImage*>(currentImagesCheckList->GetClientData(i));
            if (!currImg) continue;

            // printf("%p\n ", currImg);
            wxRect currRect = wxRect(currImg->pos.x, currImg->pos.y, currImg->data.width, currImg->data.height);
            if (!currRect.Intersects(reg)) continue;

            wxRect intersect = reg.Intersect(currRect);
            // printf("intersect's x:%d y:%d width:%d height:%d", intersect.x, intersect.y, intersect.width, intersect.height);
            if (!currImg->enabled) continue;

            // wxMessageBox(wxString::Format("%s\n", currImg->data.name));
            wxBitmap bitIntersect;
            bool     lastImage = i == img->listIndex;
            if (lastImage) {
                bitIntersect = img->bitmap.GetSubBitmap(wxRect(0, img->data.height * fr, img->data.width, img->data.height));
            } else {
                bitIntersect = currImg->bitmap.GetSubBitmap(wxRect(intersect.x - currImg->data.x, intersect.y - currImg->data.y + currImg->data.height * currImg->index, intersect.width, intersect.height));
            }
            if (!bitIntersect.IsOk()) continue;
            wxImage wxprImage = bitIntersect.ConvertToImage();
            for (int y = 0; y < intersect.height; y++) {
                for (int x = 0; x < intersect.width; x++) {
                    int rx = x + intersect.x - img->data.x,
                        ry = y + intersect.y - img->data.y;
                    // wxMessageBox(wxString::Format("rx %d ry %d\nw %d h %d\nimg pos %d %d\nname %s\nix %d iy %d",
                    //     rx, ry,
                    //     intersect.width, intersect.height,
                    //     img->data.x, img->data.y,
                    //     img->data.name,
                    //     intersect.x,intersect.y), "poses");

                    u_char srcR = wxprImage.GetRed(x, y);
                    u_char srcG = wxprImage.GetGreen(x, y);
                    u_char srcB = wxprImage.GetBlue(x, y);
                    u_char srcA = wxprImage.HasAlpha() ? wxprImage.GetAlpha(x, y) : 0xFF;  // lastImage ? y + yfr :

                    u_char dstR = imageResult.GetRed(rx, ry);
                    u_char dstG = imageResult.GetGreen(rx, ry);
                    u_char dstB = imageResult.GetBlue(rx, ry);
                    u_char dstA = 0xFF;

                    // Normalize alpha to [0, 1]
                    float fSrcA = srcA / 255.0f;
                    float fDstA = dstA / 255.0f;

                    // Output alpha
                    float outAlpha = fSrcA + fDstA * (1.0f - fSrcA);
                    if (outAlpha == 0.0f) outAlpha = 1.0f;  // Prevent division by zero

                    // Blend channels
                    u_char outR = static_cast<u_char>((srcR * fSrcA + dstR * fDstA * (1.0f - fSrcA)) / outAlpha);
                    u_char outG = static_cast<u_char>((srcG * fSrcA + dstG * fDstA * (1.0f - fSrcA)) / outAlpha);
                    u_char outB = static_cast<u_char>((srcB * fSrcA + dstB * fDstA * (1.0f - fSrcA)) / outAlpha);

                    // std::cout
                    //     << "imgres height: " << imageResult.GetHeight()
                    //     << ", y: ry" << ry << " + imgh*fr" << img->data.height * fr << " = " << ry + img->data.height * fr
                    //     << ", fr: " << fr
                    //     << std::endl;

                    // самая тупая строка
                    if (ry + img->data.height * fr >= imageResult.GetHeight()) continue;
                    // --

                    imageResult.SetRGB(rx, ry + img->data.height * fr, outR, outG, outB);
                }
            }
        }
    }
    img->bitmap = imageResult;
}
bool MainFrame::CheckTransparentImages() {
    wxString Names = wxEmptyString;
    for (DraggableImage* img : canvas->images) {
        if (HasActualTransparency(img->bitmap) && img->includedInFile) {
            Names += wxString::Format(" [%d] %s\n", img->data.id, img->data.name);
        }
    }
    if (!Names.empty()) {
        wxMessageDialog dlg(this, wxString::Format("The following images have transparency:\n%s \n Please bake them.", Names), "Warning!", wxYES_NO | wxCENTER | wxICON_QUESTION);
        dlg.SetYesNoLabels("Ok", "Proceed Anyway");
        int result = dlg.ShowModal();
        if (result == wxID_YES) {
            return true;
        } else if (result == wxID_NO) {
            return false;
        } else {
            return true;
        }
    }

    return false;
}
bool MainFrame::HasActualTransparency(const wxBitmap& bmp) {
    if (!bmp.HasAlpha()) return false;

    wxImage img = bmp.ConvertToImage();
    if (!img.HasAlpha()) return false;

    const unsigned char* alpha     = img.GetAlpha();
    int                  numPixels = img.GetWidth() * img.GetHeight();

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
