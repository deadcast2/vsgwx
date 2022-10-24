#include "wx/wx.h"
#include <vsg/all.h>
#include <vsg/viewer/Window.h>
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkx.h>
#include <xcb/xcb.h>

class wxViewerWindow : public wxWindow {
public:
    wxViewerWindow(wxWindow *parent);

    virtual ~wxViewerWindow();

    void initialize(uint32_t width, uint32_t height);

    void paintEvent(wxPaintEvent &evt);

    void paintNow();

    void render(wxDC &dc);

    void onMouseEvents(wxMouseEvent &event);

    vsg::ref_ptr<vsg::WindowTraits> traits;
    vsg::ref_ptr<vsg::Instance> instance;
    vsg::ref_ptr<vsg::Viewer> viewer;
    vsg::ref_ptr<vsg::Window> windowAdapter;

wxDECLARE_EVENT_TABLE();

private:
    bool _initialized = false;
};

BEGIN_EVENT_TABLE(wxViewerWindow, wxWindow)
                EVT_MOUSE_EVENTS(wxViewerWindow::onMouseEvents)
                EVT_PAINT(wxViewerWindow::paintEvent)
END_EVENT_TABLE()

wxViewerWindow::wxViewerWindow(wxWindow *parent) : wxWindow(parent, wxID_ANY) {
}

wxViewerWindow::~wxViewerWindow() noexcept {}

void wxViewerWindow::initialize(uint32_t width, uint32_t height) {
    _initialized = true;

    auto widget = GetHandle();
    gtk_widget_realize(widget);

    auto window = gtk_widget_get_window(widget);
    auto native = gdk_x11_window_get_xid(window);

    traits = vsg::WindowTraits::create();
    traits->nativeWindow = static_cast<xcb_window_t>(native);
    traits->width = width;
    traits->height = height;

    windowAdapter = vsg::Window::create(traits);

    viewer = vsg::Viewer::create();
    viewer->addWindow(windowAdapter);

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

    auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(windowAdapter->extent2D()));

    vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(vsg_scene->getObject<vsg::EllipsoidModel>("EllipsoidModel"));
    viewer->addEventHandler(vsg::Trackball::create(camera, ellipsoidModel));

    auto commandGraph = vsg::createCommandGraphForView(windowAdapter, camera, vsg_scene);
    viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    viewer->compile();

    paintNow();
}

void wxViewerWindow::paintEvent(wxPaintEvent &evt) {
    wxPaintDC dc(this);
    render(dc);
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
        windowAdapter->bufferedEvents.push_back(vsg::MoveEvent::create(windowAdapter,
                                                                       event_time, event.m_x, event.m_y, mask));
    }

    if (event.LeftDown()) {
        windowAdapter->bufferedEvents.push_back(vsg::ButtonPressEvent::create(windowAdapter,
                                                                              event_time, event.m_x, event.m_y,
                                                                              mask,
                                                                              1));
    }

    if (event.LeftUp()) {
        windowAdapter->bufferedEvents.push_back(vsg::ButtonReleaseEvent::create(windowAdapter,
                                                                                event_time, event.m_x, event.m_y,
                                                                                mask,
                                                                                1));
    }

    if (event.GetWheelDelta() > 0) {
        windowAdapter->bufferedEvents.push_back(vsg::ScrollWheelEvent::create(windowAdapter, event_time,
                                                                              event.GetWheelRotation() < 0 ? vsg::vec3(
                                                                                      0.0f, -1.0f, 0.0f) : vsg::vec3(
                                                                                      0.0f, 1.0f, 0.0f)));
    }
}

class MyApp : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;

    void onIdle(wxIdleEvent &evt);

    wxViewerWindow *wxViewer;
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString &title);

    void OnQuit(wxCommandEvent &event);

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

    wxViewer = new wxViewerWindow(frame);
    wxViewer->initialize(frame->m_width, frame->m_height);

    Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MyApp::onIdle));

    return true;
}

void MyApp::onIdle(wxIdleEvent &evt) {
    wxViewer->paintNow();
}

MyFrame::MyFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title) {
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");

    SetMenuBar(menuBar);

    SetSize(1024, 768);
}

void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {
    Close(true);
}

int main(int argc, char **argv) {
    wxEntry(argc, argv);

    return 0;
}
