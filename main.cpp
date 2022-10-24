#include "wx/wx.h"
#include <vsg/all.h>
#include <vsg/viewer/Window.h>
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkx.h>
#include <xcb/xcb.h>

class wxViewerWindow : public wxPanel {
public:
    wxViewerWindow(wxWindow *parent);

    virtual ~wxViewerWindow();

    void intialize(uint32_t width, uint32_t height);

    void paintEvent(wxPaintEvent &evt);

    void paintNow();

    void render(wxDC &dc);

    vsg::ref_ptr<vsg::WindowTraits> traits;
    vsg::ref_ptr<vsg::Instance> instance;
    vsg::ref_ptr<vsg::Viewer> viewer;
    vsg::ref_ptr<vsg::Window> windowAdapter;

    DECLARE_EVENT_TABLE()

private:
    bool _initialized = false;
};

wxViewerWindow::wxViewerWindow(wxWindow *parent) : wxPanel(parent, wxID_ANY) {
}

wxViewerWindow::~wxViewerWindow() noexcept {}

void wxViewerWindow::intialize(uint32_t width, uint32_t height) {
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
        std::cout << "Failed to load a valid scenene graph, Please specify a 3d "
                     "model or image file on the command line."
                  << std::endl;
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

    auto commandGraph = vsg::createCommandGraphForView(windowAdapter, camera, vsg_scene);
    viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    viewer->compile();
}

void wxViewerWindow::paintEvent(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void wxViewerWindow::paintNow()
{
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

class MyApp : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;

    void onIdle(wxIdleEvent &evt);

    wxViewerWindow *wxViewer;

    void activateRenderLoop(bool on);

    bool render_loop_on;
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
    wxViewer->intialize(frame->m_width, frame->m_height);

    activateRenderLoop(true);

    return true;
}

void MyApp::activateRenderLoop(bool on) {
    if (on && !render_loop_on) {
        Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MyApp::onIdle));
        render_loop_on = true;
    } else if (!on && render_loop_on) {
        Disconnect(wxEVT_IDLE, wxIdleEventHandler(MyApp::onIdle));
        render_loop_on = false;
    }
}

void MyApp::onIdle(wxIdleEvent &evt) {
    if (render_loop_on) {
        wxViewer->paintNow();
        evt.RequestMore(); // render continuously, not only once on idle
    }
}

BEGIN_EVENT_TABLE(wxViewerWindow, wxPanel)
                EVT_PAINT(wxViewerWindow::paintEvent)
END_EVENT_TABLE()

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
