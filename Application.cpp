#include "Application.h"
#include "Frame.h"

wxIMPLEMENT_APP_NO_MAIN(Application);

bool Application::OnInit() {
    if (!wxApp::OnInit())
        return false;

    Frame *frame = new Frame("vsgwx");
    frame->Show(true);

    wxViewer = new wxViewerWindow(frame);
    wxViewer->Initialize(frame->GetSize().GetWidth(), frame->GetSize().GetHeight());

    Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(Application::OnIdle));

    return true;
}

void Application::OnIdle(wxIdleEvent &evt) {
    wxViewer->PaintNow();
}
