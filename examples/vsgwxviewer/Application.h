#pragma once

#include <vsgwx/ViewerWindow.h>

class Application : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;

    void OnIdle(wxIdleEvent &evt);

private:
    vsgwx::ViewerWindow *viewerWindow;
};
