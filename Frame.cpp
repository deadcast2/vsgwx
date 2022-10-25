#include "Frame.h"

Frame::Frame(const wxString &title) : wxFrame(NULL, wxID_ANY, title) {
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");

    SetMenuBar(menuBar);

    SetSize(1024, 768);
}

void Frame::OnQuit(wxCommandEvent &WXUNUSED(event)) {
    Close(true);
}

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
                EVT_MENU(wxID_EXIT, Frame::OnQuit)
wxEND_EVENT_TABLE()
