#include "SpriteForm.h"

SpriteForm::SpriteForm(wxWindow* parent, wxBitmap& bitmap, DraggableImage* img, MainFrame* mainFrame)
    : SpriteFormFrame(parent), bitmap(bitmap), dragImage(img), mFrame(mainFrame) {
    SpriteCanvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    SpriteCanvas->SetBackgroundColour(*wxBLACK);

    if (dragImage->indexCoords.size() != dragImage->data.count) {
        // Ensure indexCoords matches the count in ImageData
        dragImage->indexCoords.clear();
    }
    if (dragImage->indexCoords.empty()) {
        // Initialize indexCoords if empty
        for (int i = 0; i < dragImage->data.count; ++i) {
            dragImage->indexCoords.push_back(IndexCoords{dragImage->data.id, i, 0, 0});
        }
        for (IndexCoords& coord : dragImage->indexCoords) {
            coord.id = dragImage->data.id;  // Set the id to the data id
        }
    }
    if (dragImage->data.count <= 1) {
        SelectedIndexCtrl->Disable();
    }  // Disable index selection if only one sprite
    else {
        SelectedIndexCtrl->Enable();
    }  // Enable index selection if multiple sprites
    if (dragImage->spriteSheetSize.GetWidth() == 0 || dragImage->spriteSheetSize.GetHeight() == 0) {
        // Set default sprite sheet size if not set
        dragImage->spriteSheetSize = wxSize(dragImage->data.width, dragImage->data.height);
    }
    selectionSize = dragImage->spriteSheetSize;
    offset        = dragImage->indexCoords.empty() ? wxPoint(0, 0) : -wxPoint(dragImage->indexCoords[0].x, dragImage->indexCoords[0].y);
    SelectedIndexCtrl->SetRange(0, dragImage->data.count - 1);
    goalWidth->SetValue(dragImage->data.width);
    goalHeight->SetValue(dragImage->data.height);

    if (!bitmap.IsOk()) {
        DisableFields();
    } else {
        EnableFields();
    }
    currentIndex = 0;  // Initialize current index
    UpdateFields();    // Update fields based on initial state
    Layout();          // Ensure the layout is updated
    Refresh();         // Trigger a repaint to show the initial state
}

void SpriteForm::EnableFields() {
    XCtrl->Enable();
    YCtrl->Enable();
    WidthCtrl->Enable();
    HeightCtrl->Enable();
    goalWidth->Enable();
    goalHeight->Enable();
    SelectedIndexCtrl->Enable();
    ContinueButton->Enable();
}

void SpriteForm::DisableFields() {
    XCtrl->Disable();
    YCtrl->Disable();
    WidthCtrl->Disable();
    HeightCtrl->Disable();
    goalWidth->Disable();
    goalHeight->Disable();
    SelectedIndexCtrl->Disable();
    ContinueButton->Disable();
}

void SpriteForm::OnSize(wxSizeEvent& event) {
    // Handle resizing of the SpriteCanvas
    wxSize newSize = event.GetSize();
    newSize.y -= 50;
    newSize.x -= 280;
    SpriteCanvas->SetMinSize(newSize);
    SpriteCanvas->SetMaxSize(newSize);
    Layout();   // Update layout after resizing
    Refresh();  // Trigger a repaint
}

void SpriteForm::OnEnterWindow(wxMouseEvent& event) {
    inWindow = true;
}

void SpriteForm::OnLeaveWindow(wxMouseEvent& event) {
    inWindow = false;
}

IndexCoords SpriteForm::GetCoords(int index) {
    for (const auto& coord : dragImage->indexCoords) {
        if (coord.index == index) {
            return coord;
        }
    }
    return IndexCoords{-1, -1, -1, -1};  // Not found
}

void SpriteForm::SetCoords(IndexCoords coord) {
    LimitCoord(coord);
    for (auto& c : dragImage->indexCoords) {
        if (c.index == coord.index) {
            c.x = coord.x;
            c.y = coord.y;
            return;
        }
    }

    dragImage->indexCoords.push_back(coord);
    UpdateFields();
}

void SpriteForm::OnMotion(wxMouseEvent& event) {
    // Track mouse position for preview
    if (inWindow) {
        mousePos = event.GetPosition();
        Refresh();
    }
    if (event.Dragging() && isDragging && event.RightIsDown()) {
        wxPoint pos = event.GetPosition();

        wxPoint drag       = pos - dragStartPos;
        wxSize  bitmapSize = wxSize(0, 0);
        if (bitmap.IsOk()) {
            bitmapSize = bitmap.GetSize();
        }

        if (drag.x < 0 && offset.x < -(bitmapSize.GetWidth() + SpriteCanvas->GetSize().GetWidth())) {
            drag.x = 0;
        }
        if (drag.y < 0 && offset.y < -(bitmapSize.GetHeight() + SpriteCanvas->GetSize().GetHeight())) {
            drag.y = 0;
        }
        if (drag.x > 0 && offset.x - SpriteCanvas->GetSize().GetWidth() > 0) {
            drag.x = 0;
        }
        if (drag.y > 0 && offset.y - SpriteCanvas->GetSize().GetHeight() > 0) {
            drag.y = 0;
        }

        offset += drag;
        dragStartPos = pos;

        Refresh();
    } else if (event.Dragging() && isDragging && event.LeftIsDown()) {
        IndexCoords coord = GetCoords(currentIndex);
        if (wxGetKeyState(WXK_SHIFT)) {
            wxPoint pos = event.GetPosition();
            pos += -offset;
            wxSize size = wxSize(pos.x - coord.x, pos.y - coord.y);
            if (size.GetWidth() < 1) {
                size.SetWidth(1);
            }
            if (size.GetHeight() < 1) {
                size.SetHeight(1);
            }
            if (size.GetWidth() > bitmap.GetWidth() - coord.x) {
                size.SetWidth(bitmap.GetWidth() - coord.x);
            }
            if (size.GetHeight() > bitmap.GetHeight() - coord.y) {
                size.SetHeight(bitmap.GetHeight() - coord.y);
            }
            selectionSize = size;
            dragStartPos  = pos;
            UpdateReal();
            Refresh();
        } else {
            // change selectionStartPos to current position

            coord.x = event.GetPosition().x - offset.x;
            coord.y = event.GetPosition().y - offset.y;
            if (wxGetKeyState(WXK_ALT)) {
                for (IndexCoords& _coord : dragImage->indexCoords) {
                    _coord = coord;
                    SetCoords(_coord);
                }
            }

            Refresh();
        }
        UpdateFields();
    }
}

void SpriteForm::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(SpriteCanvas);

    if (bitmap.IsOk()) {
        wxBitmap semiTransparent;
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawRectangle(wxPoint(0, 0), SpriteCanvas->GetSize());
        if (bitmap.IsOk()) {
            dc.DrawBitmap(bitmap, offset);
            dc.DrawRectangle(wxRect(offset, bitmap.GetSize()));
        }
        if (selectionSize.GetWidth() > 0 && selectionSize.GetHeight() > 0) {
            if (!(dragImage->data.count <= 1)) {

                IndexCoords coord        = GetCoords(currentIndex > 0 ? currentIndex - 1 : 1);
                IndexCoords currentCoord = GetCoords(currentIndex);
                wxRect      selectionRect(wxPoint(coord.x, coord.y), selectionSize);

                wxBitmap resized = bitmap.GetSubBitmap(selectionRect);
                if (resized.IsOk() && coord.id != -1) {
                    wxImage img = resized.ConvertToImage();
                    if (!img.HasAlpha()) {
                        img.InitAlpha();
                    }

                    int w = img.GetWidth();
                    int h = img.GetHeight();
                    for (int y = 0; y < h; ++y) {
                        for (int x = 0; x < w; ++x) {
                            // TODO
                            // This can be optimized i think...
                            // For example change image if source rect is changed...
                            // But it still works well so I will leave it like this for now
                            // Set alpha to 25% for semi-transparency for every pixel
                            img.SetAlpha(x, y, img.GetAlpha(x, y) * 0.25);  // 25% alpha
                        }
                    }
                    semiTransparent = wxBitmap(img);
                    if (currentCoord.index == currentIndex) {
                        // Draw the semi-transparent bitmap at the selection start position
                        dc.DrawBitmap(semiTransparent, wxPoint(currentCoord.x + offset.x, currentCoord.y + offset.y), true);
                    }
                }
            }
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(wxPen(*wxRED, 1));
            for (const auto& coord : dragImage->indexCoords) {
                wxPoint pos(coord.x + offset.x, coord.y + offset.y);
                if (coord.index == currentIndex) {
                    dc.SetPen(wxPen(*wxRED, 1));  // Highlight current index
                } else {
                    dc.SetPen(wxPen(*wxWHITE, 1));  // Default pen for other indices
                }

                dc.DrawRectangle(pos, selectionSize);
                dc.DrawText(wxString::Format("%d", coord.index), pos + wxPoint(2, 2));
            }
        }
        if (bitmap.IsOk() && inWindow) {
            wxPoint mouse      = mousePos - offset;
            wxSize  windowSize = wxSize(16, 16);
            wxRect  srcRect(mouse.x - windowSize.GetWidth() / 2, mouse.y - windowSize.GetHeight() / 2, windowSize.GetWidth(), windowSize.GetHeight());
            srcRect.Intersect(wxRect(0, 0, bitmap.GetWidth(), bitmap.GetHeight()));
            if (srcRect.width > 0 && srcRect.height > 0) {
                // TODO : Draw "semiTransparent" overlay

                IndexCoords coord      = GetCoords(currentIndex);
                wxBitmap    preview    = bitmap.GetSubBitmap(srcRect);
                wxImage     previewImg = preview.ConvertToImage();

                wxImage  scaledImg = previewImg.Scale(128, 128, wxIMAGE_QUALITY_NEAREST);  // No smoothing, pure pixels
                wxBitmap scaledBmp(scaledImg);

                wxSize canvasSize = SpriteCanvas->GetSize();

                wxPoint previewPos(0, canvasSize.y - 128);

                dc.DrawBitmap(scaledBmp, previewPos, false);  // No transparency blending
                // Draw selection rectangle if it overlaps the preview
                if (dragImage && selectionSize.GetWidth() > 0 && selectionSize.GetHeight() > 0) {
                    wxRect selRect(coord.x, coord.y, selectionSize.GetWidth(), selectionSize.GetHeight());
                    wxRect overlap = selRect;
                    overlap.Intersect(srcRect);
                    if (overlap.width > 0 && overlap.height > 0) {
                        // Map overlap rect to preview coordinates
                        int px = (int)(((overlap.x - srcRect.x) / (float)srcRect.width) * 128.0f);
                        int py = (int)(((overlap.y - srcRect.y) / (float)srcRect.height) * 128.0f);
                        int pw = (int)((overlap.width / (float)srcRect.width) * 128.0f);
                        int ph = (int)((overlap.height / (float)srcRect.height) * 128.0f);
                        dc.SetBrush(*wxTRANSPARENT_BRUSH);
                        dc.SetPen(wxPen(*wxRED, 2));
                        dc.DrawRectangle(previewPos + wxPoint(px, py), wxSize(pw, ph));
                    }
                }
            }
        }
        // Display current mouse position relative to canvas, including offset

        dc.DrawText(wxString::Format("OFFSET: X: %d Y: %d", -offset.x, -offset.y), wxPoint(0, 0));
    } else {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawRectangle(wxPoint(0, 0), SpriteCanvas->GetSize());
        dc.DrawText("No Spritesheet loaded", wxPoint(10, 10));
    }
}

void SpriteForm::OnOpen(wxCommandEvent& event) {
    // Open File to work with
    wxFileDialog openFileDialog(this, _("Open Image"), "", "",
                                "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK) {
        wxString filePath = openFileDialog.GetPath();
        bitmap.LoadFile(filePath, wxBITMAP_TYPE_ANY);
        Refresh();  // Trigger a repaint
    }
    if (bitmap.IsOk()) {
        EnableFields();
        UpdateFields();  // Update fields with the new bitmap
    } else {
        DisableFields();
        wxMessageBox("Failed to load image. Please try again.", "Error", wxOK | wxICON_ERROR);
    }
}

void SpriteForm::OnXChange(wxSpinEvent& event) {
    IndexCoords coord = GetCoords(currentIndex);
    if (wxGetKeyState(WXK_SHIFT)) {
        // More shifty thingy
        // move every cell by the same amount
        for (IndexCoords coordr : dragImage->indexCoords) {
            coordr.x -= coord.x - event.GetValue();
            SetCoords(coordr);
        }
    }

    coord.x = event.GetValue();
    SetCoords(coord);
    Refresh();
}
void SpriteForm::LimitCoord(IndexCoords& coord) {
    if (coord.x < 0) {
        coord.x = 0;
    }
    if (coord.y < 0) {
        coord.y = 0;
    }
    if (coord.x + selectionSize.GetWidth() > bitmap.GetWidth()) {
        coord.x = bitmap.GetWidth() - selectionSize.GetWidth();
    }
    if (coord.y + selectionSize.GetHeight() > bitmap.GetHeight()) {
        coord.y = bitmap.GetHeight() - selectionSize.GetHeight();
    }
}
void SpriteForm::OnYChange(wxSpinEvent& event) {
    IndexCoords coord = GetCoords(currentIndex);
    coord.y           = event.GetValue();
    SetCoords(coord);
    Refresh();
}

void SpriteForm::OnGWidth(wxSpinEvent& event) {
    UpdateReal();
}

void SpriteForm::OnGHeight(wxSpinEvent& event) {
    UpdateReal(true);
}

void SpriteForm::UpdateReal(bool height) {
    if (wxGetKeyState(WXK_CONTROL)) {
        float w      = static_cast<float>(WidthCtrl->GetValue());
        float h      = static_cast<float>(HeightCtrl->GetValue());
        float aspect = w / (h == 0 ? 1 : h);
        if (height) {
            goalWidth->SetValue(static_cast<int>(goalHeight->GetValue() * (aspect == 0.00f ? 1 : aspect)));
        } else {
            goalHeight->SetValue(static_cast<int>(goalWidth->GetValue() / (aspect == 0.00f ? 1 : aspect)));
        }

        Refresh();
        Layout();
    }
    realSize.SetWidth(goalWidth->GetValue());
    realSize.SetHeight(goalHeight->GetValue());
}

void SpriteForm::OnWidthChange(wxSpinEvent& event) {
    selectionSize.SetWidth(event.GetValue());
    IndexCoords currentIndexCoords = GetCoords(currentIndex);
    LimitCoord(currentIndexCoords);
    SetCoords(currentIndexCoords);
    UpdateReal();
    Refresh();
}

void SpriteForm::OnHeightChange(wxSpinEvent& event) {
    selectionSize.SetHeight(event.GetValue());
    IndexCoords currentIndexCoords = GetCoords(currentIndex);
    LimitCoord(currentIndexCoords);
    SetCoords(currentIndexCoords);
    UpdateReal(true);
    Refresh();
}

void SpriteForm::OnCancel(wxCommandEvent& event) {
    Close();
}

void SpriteForm::OnLeftMDown(wxMouseEvent& event) {
    if (inWindow && !wxGetKeyState(WXK_SHIFT)) {
        IndexCoords coord = GetCoords(currentIndex);
        if (dragImage) {
            coord.x = event.GetPosition().x - offset.x;
            coord.y = event.GetPosition().y - offset.y;
            SetCoords(coord);

            Refresh();
        }
    }
}

void SpriteForm::OnRightMDown(wxMouseEvent& event) {
    if (inWindow) {
        isDragging        = true;
        dragStartPos      = event.GetPosition();
        selectionStartPos = dragStartPos;  // Start selection at drag start
    }
}

void SpriteForm::OnSelectedIndex(wxSpinEvent& event) {
    // Handle selection index change
    int selectedIndex = event.GetValue();
    if (dragImage && selectedIndex >= 0 && selectedIndex < dragImage->data.count) {
        currentIndex = selectedIndex;  // Update current index
        UpdateFields();                // Update fields based on the new index
        Refresh();                     // Trigger a repaint to reflect the change
    }
}

void SpriteForm::UpdateFields() {
    // Update the fields based on the current index
    if (dragImage && currentIndex >= 0 && currentIndex < dragImage->data.count) {
        IndexCoords coords = GetCoords(currentIndex);
        if (bitmap.IsOk()) {
            if (coords.x < 0) {
                coords.x = 0;
            }
            if (coords.y < 0) {
                coords.y = 0;
            }
            if (coords.x + selectionSize.GetWidth() > bitmap.GetWidth()) {
                coords.x = bitmap.GetWidth() - selectionSize.GetWidth();
            }
            if (coords.y + selectionSize.GetHeight() > bitmap.GetHeight()) {
                coords.y = bitmap.GetHeight() - selectionSize.GetHeight();
            }
        }
        XCtrl->SetValue(coords.x);
        YCtrl->SetValue(coords.y);
        WidthCtrl->SetValue(selectionSize.GetWidth());
        HeightCtrl->SetValue(selectionSize.GetHeight());
        SelectedIndexCtrl->SetValue(currentIndex);
        realSize = wxSize(goalWidth->GetValue(), goalHeight->GetValue());
        if (bitmap.IsOk()) {
            XCtrl->SetRange(0, bitmap.GetWidth() - selectionSize.GetWidth());
            YCtrl->SetRange(0, bitmap.GetHeight() - selectionSize.GetHeight());
            WidthCtrl->SetRange(0, bitmap.GetWidth());
            HeightCtrl->SetRange(0, bitmap.GetHeight());
        }
    }
}

void SpriteForm::OnContinue(wxCommandEvent& event) {
    // Save changes and close the form
    if (dragImage) {
        int      w           = dragImage->spriteSheetSize.GetWidth();
        int      h           = dragImage->spriteSheetSize.GetHeight();
        int      totalHeight = dragImage->data.count * h;
        wxBitmap stacked(w, totalHeight, 32);

        wxMemoryDC memDC(stacked);

        // stack vertically images

        memDC.SetBackground(*wxTRANSPARENT_BRUSH);

        memDC.Clear();
        for (int i = 0; i < dragImage->data.count; ++i) {
            IndexCoords coord = GetCoords(i);
            printf("Index %d: x=%d, y=%d cindex=%d\n", i, coord.x, coord.y, coord.index);
            printf("Bitmap size: %d x %d\n", bitmap.GetWidth(), bitmap.GetHeight());
            wxBitmap subBitmap = bitmap.GetSubBitmap(wxRect(coord.x, coord.y, w, h));
            memDC.DrawBitmap(subBitmap, 0, i * h);
        }
        memDC.SelectObject(wxNullBitmap);

        stacked.SaveFile(wxString(get_current_dir_name()) + "/sprite_sheet_.png", wxBITMAP_TYPE_PNG);

        // Apply the resized bitmap to dragImage->bitmap with the specified dimensions
        if (stacked.IsOk()) {
            dragImage->spriteSheetSize = selectionSize;

            // Those images and scales are so real...
            wxImage realImage = stacked.ConvertToImage();

            // I hope this thing will work better than rescale
            realImage = realImage.Scale(realSize.GetWidth(), realSize.GetHeight() * dragImage->data.count, wxIMAGE_QUALITY_HIGH);
            stacked   = wxBitmap(realImage);

            printf("Resized bitmap size: %d x %d. IS OK %d\n", stacked.GetWidth(), stacked.GetHeight(), stacked.IsOk());
            dragImage->bitmap      = stacked;
            dragImage->data.width  = realSize.GetWidth();
            dragImage->data.height = realSize.GetHeight();
            mFrame->UnpackImageData(dragImage);
            printf("Unpacked image data: %d x %d\n", dragImage->data.width, dragImage->data.height);
            mFrame->Refresh();
        }

        Close();
    } else {
        wxMessageBox("No image selected to continue with.", "Error", wxOK | wxICON_ERROR);
    }
}
