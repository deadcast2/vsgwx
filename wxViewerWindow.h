#ifndef VSGWX_WXVIEWERWINDOW_H
#define VSGWX_WXVIEWERWINDOW_H

#include "wx/wx.h"
#include <vsg/all.h>

class wxViewerWindow : public wxWindow {
public:
    wxViewerWindow(wxWindow *parent);

    virtual ~wxViewerWindow();

    void Initialize(uint32_t width, uint32_t height);

    void OnPaintEvent(wxPaintEvent &evt);

    void PaintNow();

    void Render(wxDC &dc);

    void OnMouseWheel(wxMouseEvent &event);

    void OnMouseMotion(wxMouseEvent &event);

    void OnMouseDown(wxMouseEvent &event);

    void OnMouseUp(wxMouseEvent &event);

private:
    vsg::ref_ptr<vsg::WindowTraits> traits;
    vsg::ref_ptr<vsg::Viewer> viewer;
    vsg::ref_ptr<vsg::Window> window;
};

#endif
