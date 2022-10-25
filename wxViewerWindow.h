#ifndef VSGWX_WXVIEWERWINDOW_H
#define VSGWX_WXVIEWERWINDOW_H

#include "wx/wx.h"
#include <vsg/all.h>

class wxViewerWindow : public wxWindow {
public:
    wxViewerWindow(wxWindow *parent);

    virtual ~wxViewerWindow();

    void initialize(uint32_t width, uint32_t height);

    void paintEvent(wxPaintEvent &evt);

    void paintNow();

    void render(wxDC &dc);

    void onMouseEvents(wxMouseEvent &event);

private:
    vsg::ref_ptr<vsg::WindowTraits> traits;
    vsg::ref_ptr<vsg::Viewer> viewer;
    vsg::ref_ptr<vsg::Window> window;

    wxDECLARE_EVENT_TABLE();
};

#endif
