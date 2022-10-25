#ifndef VSGWX_APPLICATION_H
#define VSGWX_APPLICATION_H

#include "wxViewerWindow.h"

class Application : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;

    void OnIdle(wxIdleEvent &evt);

private:
    wxViewerWindow *wxViewer;
};

#endif
