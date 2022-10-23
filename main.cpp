#include "wx/wx.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString &title);

    void OnQuit(wxCommandEvent &event);

    void OnAbout(wxCommandEvent &event);

private:
wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
                EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP_NO_MAIN(MyApp);

bool MyApp::OnInit() {
    if (!wxApp::OnInit())
        return false;

    MyFrame *frame = new MyFrame("vsgwx");
    frame->Show(true);

    return true;
}

MyFrame::MyFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title) {
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");

    SetMenuBar(menuBar);
}

void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {
    Close(true);
}

int main(int argc, char **argv) {
    wxEntry(argc, argv);

    return 0;
}
