#ifndef VSGWX_FRAME_H
#define VSGWX_FRAME_H

#include "wx/wx.h"

class Frame : public wxFrame {
public:
    Frame(const wxString &title);

    void OnQuit(wxCommandEvent &event);
};

#endif
