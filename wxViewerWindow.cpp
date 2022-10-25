#include "wxViewerWindow.h"
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkx.h>
#include <xcb/xcb.h>

BEGIN_EVENT_TABLE(wxViewerWindow, wxWindow)
                EVT_MOUSE_EVENTS(wxViewerWindow::onMouseEvents)
                EVT_PAINT(wxViewerWindow::paintEvent)
END_EVENT_TABLE()

wxViewerWindow::wxViewerWindow(wxWindow *parent) : wxWindow(parent, wxID_ANY) {
}

wxViewerWindow::~wxViewerWindow() noexcept {}

void wxViewerWindow::initialize(uint32_t width, uint32_t height) {
    auto gtkWidget = GetHandle();

    gtk_widget_realize(gtkWidget);

    auto gtkWindow = gtk_widget_get_window(gtkWidget);
    auto native = gdk_x11_window_get_xid(gtkWindow);

    traits = vsg::WindowTraits::create();
    traits->nativeWindow = static_cast<xcb_window_t>(native);
    traits->width = width;
    traits->height = height;

    window = vsg::Window::create(traits);
    viewer = vsg::Viewer::create();
    viewer->addWindow(window);

    auto vsg_scene = vsg::read_cast<vsg::Node>("../models/glider.vsgt");
    if (!vsg_scene) {
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

    paintNow();
}

void wxViewerWindow::paintEvent(wxPaintEvent &evt) {
    paintNow();
}

void wxViewerWindow::paintNow() {
    wxClientDC dc(this);
    render(dc);
}

void wxViewerWindow::render(wxDC &dc) {
    if (viewer->advanceToNextFrame()) {
        viewer->handleEvents();
        viewer->update();
        viewer->recordAndSubmit();
        viewer->present();
    }
}

void wxViewerWindow::onMouseEvents(wxMouseEvent &event) {
    vsg::clock::time_point event_time = vsg::clock::now();

    auto mask = static_cast<vsg::ButtonMask>(vsg::BUTTON_MASK_1);

    if (event.Dragging()) {
        window->bufferedEvents.push_back(vsg::MoveEvent::create(window,
                                                                event_time, event.m_x, event.m_y, mask));
    }

    if (event.LeftDown()) {
        window->bufferedEvents.push_back(vsg::ButtonPressEvent::create(window,
                                                                       event_time, event.m_x, event.m_y,
                                                                       mask,
                                                                       1));
    }

    if (event.LeftUp()) {
        window->bufferedEvents.push_back(vsg::ButtonReleaseEvent::create(window,
                                                                         event_time, event.m_x, event.m_y,
                                                                         mask,
                                                                         1));
    }

    if (event.GetWheelDelta() > 0) {
        window->bufferedEvents.push_back(vsg::ScrollWheelEvent::create(window, event_time,
                                                                       event.GetWheelRotation() < 0 ? vsg::vec3(
                                                                               0.0f, -1.0f, 0.0f) : vsg::vec3(
                                                                               0.0f, 1.0f, 0.0f)));
    }
}
