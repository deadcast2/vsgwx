#ifndef VSGWX_APPLICATION_H
#define VSGWX_APPLICATION_H

#include "ViewerWindow.h"

class Application : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;

    void OnIdle(wxIdleEvent &evt);

private:
    vsgwx::ViewerWindow *viewerWindow;
};

#endif
