#include "wxViewerWindow.h"

#ifdef __LINUX__
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkx.h>
#include <xcb/xcb.h>
#endif

wxViewerWindow::wxViewerWindow(wxWindow *parent) : wxWindow(parent, wxID_ANY) {
    Connect(wxEVT_MOTION, wxMouseEventHandler(wxViewerWindow::OnMouseMotion));
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxViewerWindow::OnMouseDown));
    Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(wxViewerWindow::OnMouseDown));
    Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(wxViewerWindow::OnMouseDown));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(wxViewerWindow::OnMouseUp));
    Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(wxViewerWindow::OnMouseUp));
    Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(wxViewerWindow::OnMouseUp));
    Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(wxViewerWindow::OnMouseWheel));
    Connect(wxEVT_SIZE, wxSizeEventHandler(wxViewerWindow::OnSizing));
}

wxViewerWindow::~wxViewerWindow() {
    if (window) {
        window->releaseWindow();
    }

    window = {};
    viewer = {};
}

void wxViewerWindow::Initialize(uint32_t width, uint32_t height) {
#ifdef __LINUX__
    auto gtkWidget = GetHandle();
    gtk_widget_realize(gtkWidget);

    auto gtkWindow = gtk_widget_get_window(gtkWidget);
    auto native = gdk_x11_window_get_xid(gtkWindow);
#endif

    traits = vsg::WindowTraits::create();

#ifdef __LINUX__
    traits->nativeWindow = static_cast<xcb_window_t>(native);
#elif _WIN64
    traits->nativeWindow = reinterpret_cast<HWND>(GetHandle());
#endif

    traits->width = width;
    traits->height = height;

    window = vsg::Window::create(traits);
    viewer = vsg::Viewer::create();
    viewer->addWindow(window);

#ifdef __LINUX__
    auto vsg_scene = vsg::read_cast<vsg::Node>("../models/lz.vsgt");
#elif _WIN64
    auto vsg_scene = vsg::read_cast<vsg::Node>("..\\models\\lz.vsgt");
#endif

    if (!vsg_scene) {
        wxPrintf("Unable to load test model!\n");
        return;
    }

    vsg::ComputeBounds computeBounds;
    vsg_scene->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
    double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
    double nearFarRatio = 0.001;
    auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
    auto perspective = vsg::Perspective::create(
            30.0,
            static_cast<double>(width) /
            static_cast<double>(height),
            nearFarRatio * radius, radius * 4.5);

    auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));

    vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(vsg_scene->getObject<vsg::EllipsoidModel>("EllipsoidModel"));
    viewer->addEventHandler(vsg::Trackball::create(camera, ellipsoidModel));

    auto commandGraph = vsg::createCommandGraphForView(window, camera, vsg_scene);
    viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    viewer->compile();

    PaintNow();
}

void wxViewerWindow::PaintNow() {
    wxClientDC dc(this);
    Render(dc);
}

void wxViewerWindow::Render(wxDC &dc) {
    if (viewer->advanceToNextFrame()) {
        viewer->handleEvents();
        viewer->update();
        viewer->recordAndSubmit();
        viewer->present();
    }
}

void wxViewerWindow::OnMouseMotion(wxMouseEvent &e) {
    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = ConvertMouseButtons(e);

    window->bufferedEvents.push_back(vsg::MoveEvent::create(window,
                                                            event_time, e.m_x, e.m_y, mask));
}

void wxViewerWindow::OnMouseDown(wxMouseEvent &e) {
    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = ConvertMouseButtons(e);

    window->bufferedEvents.push_back(vsg::ButtonPressEvent::create(window,
                                                                   event_time, e.m_x, e.m_y,
                                                                   mask,
                                                                   button));
}

void wxViewerWindow::OnMouseUp(wxMouseEvent &e) {
    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = ConvertMouseButtons(e);

    window->bufferedEvents.push_back(vsg::ButtonReleaseEvent::create(window,
                                                                     event_time, e.m_x, e.m_y,
                                                                     mask,
                                                                     button));
}

void wxViewerWindow::OnMouseWheel(wxMouseEvent &event) {
    vsg::clock::time_point event_time = vsg::clock::now();

    window->bufferedEvents.push_back(vsg::ScrollWheelEvent::create(window, event_time,
                                                                   event.GetWheelRotation() < 0 ? vsg::vec3(
                                                                           0.0f, -1.0f, 0.0f) : vsg::vec3(
                                                                           0.0f, 1.0f, 0.0f)));
}

void wxViewerWindow::OnSizing(wxSizeEvent &e) {
    vsg::clock::time_point event_time = vsg::clock::now();

    window->bufferedEvents.push_back(vsg::ConfigureWindowEvent::create(window, event_time, e.GetRect().GetPosition().x,
                                                                       e.GetRect().GetPosition().y,
                                                                       static_cast<uint32_t>(e.GetSize().GetWidth()),
                                                                       static_cast<uint32_t>(e.GetSize().GetHeight())));
}

std::pair<vsg::ButtonMask, uint32_t> wxViewerWindow::ConvertMouseButtons(wxMouseEvent &e) const {
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
