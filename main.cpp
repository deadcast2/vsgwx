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

enum {
    Minimal_Quit = wxID_EXIT,

    Minimal_About = wxID_ABOUT
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
                EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
                EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    if (!wxApp::OnInit())
        return false;

    MyFrame *frame = new MyFrame("Minimal wxWidgets App");

    frame->Show(true);

    return true;
}

MyFrame::MyFrame(const wxString &title)
        : wxFrame(NULL, wxID_ANY, title) {
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
}

void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent & WXUNUSED(event)) {
    wxMessageBox(wxString::Format
                         (
                                 "Welcome to %s!\n"
                                 "\n"
                                 "This is the minimal wxWidgets sample\n"
                                 "running under %s.",
                                 wxVERSION_STRING,
                                 wxGetOsDescription()
                         ),
                 "About wxWidgets minimal sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}
