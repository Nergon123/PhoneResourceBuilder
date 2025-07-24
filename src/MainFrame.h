#pragma once
#include <wx/wx.h>
#include <vector>
#include "SpriteForm.h"
#include "ImageCanvas.h"
#include "MyProjectBase.h"
#include "FileProcessor.h"

class MainFrame : public MyFrame1 {
public:
    ImageCanvas*    canvas;
    FileProcessing  fileProcessor;

    MainFrame* mFrame;

    MainFrame(wxWindow* parent);
    DraggableImage* GetSelectedImage();
    void UnpackImageData(DraggableImage* img);
    wxCheckListBox* GetList();
    void EnableControls();
    void DisableControls();

protected:
    void SetAlphaOfImage(wxBitmap& image, uint8_t transpValue);
    void OnIncludeInFileBox(wxCommandEvent& event) override;
    void OnTranspSlider(wxScrollEvent& event ) override;
    void OnImagesListToggle(wxCommandEvent& event) override;
    void OnClkUpList(wxCommandEvent& event) override;
    void OnClkDownList(wxCommandEvent& event) override;
    void OnClkBindList(wxCommandEvent& event) override;
    void OnExportAllPNG(wxCommandEvent& event) override;
    void OnExport(wxCommandEvent& event) override;
    void OnExportPNG(wxCommandEvent& event) override;
    void OnClkNewList(wxCommandEvent& event) override;
    void OnImagesListSelect(wxCommandEvent& event) override;
    void OnClkDelList(wxCommandEvent& event) override;
    void OnUpdateID(wxSpinEvent& event) override;
    void OnUpdateCount(wxSpinEvent& event) override;
    void OnUpdateIndex(wxSpinEvent& event) override;
    void OnUpdateX(wxSpinEvent& event) override;
    void OnUpdateY(wxSpinEvent& event) override;
    void OnUpdateWidth(wxSpinEvent& event) override;
    void OnUpdateHeight(wxSpinEvent& event) override;
    void OnHardBake(wxCommandEvent& event) override;
    void OnBakeImage(wxCommandEvent& event) override;
    void OnTransColorChanged(wxColourPickerEvent& event) override;
    void OnUpdateTrans(wxCommandEvent& event) override;
    void OnClickPICK(wxCommandEvent& event) override;
    void OnOpen(wxCommandEvent& event) override;
    void OnSave(wxCommandEvent& event) override;
    void OnSaveAs(wxCommandEvent& event) override;
    void OnImport(wxCommandEvent& event) override;
    void OnClose(wxCloseEvent& event) override;
    void OnExitMenu(wxCommandEvent& event) override;
    void Exit();
    void PopulateList(wxCheckListBox* list, const std::vector<DraggableImage*>& images);
    void SwapItems(wxCheckListBox* list, int i, int j);
    void MoveSelectedItemUp(wxCheckListBox* list);
    void MoveSelectedItemDown(wxCheckListBox* list);
    void ClearList(wxCheckListBox* list);
    void RenameSelectedItem(wxCheckListBox* list, const wxString& newLabel);
    wxBitmap AskAndStackImages(int n, wxSize targetSize);
    wxBitmap LoadBitmap(const char* title, const wxBitmap& bitmapBefore, wxSize targetSize, bool forceContinue = false, bool* forceResize = nullptr);
    wxBitmap CombineBitmapsVertically(const std::vector<wxBitmap>& bitmaps);
    void DisableControlsInSizer(wxSizer* sizer);
    void EnableControlsInSizer(wxSizer* sizer);
};
