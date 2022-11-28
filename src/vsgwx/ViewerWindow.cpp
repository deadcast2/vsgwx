#include "../../include/vsgwx/ViewerWindow.h"

#ifdef __LINUX__

#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkx.h>
#include <xcb/xcb.h>

#endif

using namespace vsgwx;

ViewerWindow::ViewerWindow(wxWindow *parent) : wxWindow(parent, wxID_ANY, wxDefaultPosition,
                                                        wxSize(parent->GetSize())) {
    Connect(wxEVT_MOTION, wxMouseEventHandler(ViewerWindow::OnMouseMotion));
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(ViewerWindow::OnMouseDown));
    Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(ViewerWindow::OnMouseDown));
    Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(ViewerWindow::OnMouseDown));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(ViewerWindow::OnMouseUp));
    Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(ViewerWindow::OnMouseUp));
    Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(ViewerWindow::OnMouseUp));
    Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(ViewerWindow::OnMouseWheel));
    Connect(wxEVT_SIZE, wxSizeEventHandler(ViewerWindow::OnSizing));

    traits = vsg::WindowTraits::create();
}

ViewerWindow::~ViewerWindow() {
    if (window) {
        window->releaseWindow();
    }

    window = {};
    viewer = {};
}

void ViewerWindow::Initialize(uint32_t width, uint32_t height) {
#ifdef __LINUX__
    auto gtkWidget = GetHandle();
    gtk_widget_realize(gtkWidget);

    auto gtkWindow = gtk_widget_get_window(gtkWidget);
    auto native = gdk_x11_window_get_xid(gtkWindow);

    traits->nativeWindow = static_cast<xcb_window_t>(native);
#elif _WIN64
    traits->nativeWindow = reinterpret_cast<HWND>(GetHandle());
#endif

    traits->width = width;
    traits->height = height;

    window = vsg::Window::create(traits);
    viewer = vsg::Viewer::create();

    viewer->addWindow(window);

    if (initializeCallback) {
        initializeCallback(*this, width, height);
    }

    PaintNow();
}

void ViewerWindow::PaintNow() {
    Render();
}

void ViewerWindow::Render() {
    if (frameCallback) {
        frameCallback(*this);
    }
}

void ViewerWindow::OnMouseMotion(wxMouseEvent &e) {
    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = ConvertMouseButtons(e);

    window->bufferedEvents.push_back(vsg::MoveEvent::create(window,
                                                            event_time, e.m_x, e.m_y, mask));
}

void ViewerWindow::OnMouseDown(wxMouseEvent &e) {
    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = ConvertMouseButtons(e);

    window->bufferedEvents.push_back(vsg::ButtonPressEvent::create(window,
                                                                   event_time, e.m_x, e.m_y,
                                                                   mask,
                                                                   button));
}

void ViewerWindow::OnMouseUp(wxMouseEvent &e) {
    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = ConvertMouseButtons(e);

    window->bufferedEvents.push_back(vsg::ButtonReleaseEvent::create(window,
                                                                     event_time, e.m_x, e.m_y,
                                                                     mask,
                                                                     button));
}

void ViewerWindow::OnMouseWheel(wxMouseEvent &event) {
    vsg::clock::time_point event_time = vsg::clock::now();

    window->bufferedEvents.push_back(vsg::ScrollWheelEvent::create(window, event_time,
                                                                   event.GetWheelRotation() < 0 ? vsg::vec3(
                                                                           0.0f, -1.0f, 0.0f) : vsg::vec3(
                                                                           0.0f, 1.0f, 0.0f)));
}

void ViewerWindow::OnSizing(wxSizeEvent &e) {
    vsg::clock::time_point event_time = vsg::clock::now();

    window->bufferedEvents.push_back(vsg::ConfigureWindowEvent::create(window, event_time, e.GetRect().GetPosition().x,
                                                                       e.GetRect().GetPosition().y,
                                                                       static_cast<uint32_t>(e.GetSize().GetWidth()),
                                                                       static_cast<uint32_t>(e.GetSize().GetHeight())));
}

std::pair<vsg::ButtonMask, uint32_t> ViewerWindow::ConvertMouseButtons(wxMouseEvent &e) const {
    uint16_t mask{0};
    uint32_t button = 0;

    if (e.LeftIsDown()) {
        mask = mask | vsg::BUTTON_MASK_1;
        button = 1;
    }

    if (e.MiddleIsDown()) {
        mask = mask | vsg::BUTTON_MASK_2;
        button = 3;
    }

    if (e.RightIsDown()) {
        mask = mask | vsg::BUTTON_MASK_3;
        button = 2;
    }

    return {static_cast<vsg::ButtonMask>(mask), button};
}
