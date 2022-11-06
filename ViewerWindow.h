#pragma once

#include "wx/wx.h"
#include <vsg/all.h>

namespace vsgwx {

    class ViewerWindow : public wxWindow {
    public:
        ViewerWindow(wxWindow *parent);

        virtual ~ViewerWindow();

        using InitializeCallback = std::function<void(ViewerWindow&, uint32_t width, uint32_t height)>;
        InitializeCallback initializeCallback;

        using FrameCallback = std::function<bool(ViewerWindow&)>;
        FrameCallback frameCallback;

        void Initialize(uint32_t width, uint32_t height);

        void PaintNow();

        void Render(wxDC &dc);

        void OnMouseWheel(wxMouseEvent &e);

        void OnMouseMotion(wxMouseEvent &e);

        void OnMouseDown(wxMouseEvent &e);

        void OnMouseUp(wxMouseEvent &e);

        void OnSizing(wxSizeEvent &e);

        vsg::ref_ptr<vsg::WindowTraits> traits;
        vsg::ref_ptr<vsg::Viewer> viewer;
        vsg::ref_ptr<vsg::Window> window;

    protected:
        std::pair<vsg::ButtonMask, uint32_t> ConvertMouseButtons(wxMouseEvent &e) const;
    };

}
