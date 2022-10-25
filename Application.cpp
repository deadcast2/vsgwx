#include "Application.h"
#include "Frame.h"

bool Application::OnInit() {
    if (!wxApp::OnInit())
        return false;

    Frame *frame = new Frame("vsgwx");
    frame->Show(true);

    wxViewer = new wxViewerWindow(frame);
    wxViewer->initialize(frame->m_width, frame->m_height);

    Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(Application::OnIdle));

    return true;
}

void Application::OnIdle(wxIdleEvent &evt) {
    wxViewer->paintNow();
}

wxIMPLEMENT_APP_NO_MAIN(Application);
