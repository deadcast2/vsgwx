#ifndef VSGWX_WXVIEWERWINDOW_H
#define VSGWX_WXVIEWERWINDOW_H

#include "wx/wx.h"
#include <vsg/all.h>

class wxViewerWindow : public wxWindow {
public:
    wxViewerWindow(wxWindow *parent);

    virtual ~wxViewerWindow();

    void Initialize(uint32_t width, uint32_t height);

    void PaintNow();

    void Render(wxDC &dc);

    void OnMouseWheel(wxMouseEvent &e);

    void OnMouseMotion(wxMouseEvent &e);

    void OnMouseDown(wxMouseEvent &e);

    void OnMouseUp(wxMouseEvent &e);

    void OnSizing(wxSizeEvent &e);

protected:
    std::pair<vsg::ButtonMask, uint32_t> ConvertMouseButtons(wxMouseEvent &e) const;

private:
    vsg::ref_ptr<vsg::WindowTraits> traits;
    vsg::ref_ptr<vsg::Viewer> viewer;
    vsg::ref_ptr<vsg::Window> window;
};

#endif
