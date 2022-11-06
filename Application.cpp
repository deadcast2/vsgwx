#include "Application.h"
#include "Frame.h"

wxIMPLEMENT_APP_NO_MAIN(Application);

bool Application::OnInit() {
    if (!wxApp::OnInit())
        return false;

    Frame *frame = new Frame("vsgwx");
    frame->Show(true);

    Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(Application::OnIdle));

    viewerWindow = new vsgwx::ViewerWindow(frame);

    viewerWindow->initializeCallback = [&](vsgwx::ViewerWindow &vw, uint32_t width, uint32_t height) {
#ifdef __LINUX__
        auto vsg_scene = vsg::read_cast<vsg::Node>("../models/lz.vsgt");
#elif _WIN64
        auto vsg_scene = vsg::read_cast<vsg::Node>("..\\models\\lz.vsgt");
#endif

        if (!vsg_scene) {
            wxPrintf("Unable to load test model!\n");
            return false;
        }

        vsg::ComputeBounds computeBounds;
        vsg_scene->accept(computeBounds);
        vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
        double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
        double nearFarRatio = 0.001;
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre,
                                          vsg::dvec3(0.0, 0.0, 1.0));
        auto perspective = vsg::Perspective::create(
                30.0,
                static_cast<double>(width) /
                static_cast<double>(height),
                nearFarRatio * radius, radius * 4.5);

        auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(vw.window->extent2D()));

        vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(vsg_scene->getObject<vsg::EllipsoidModel>("EllipsoidModel"));
        vw.viewer->addEventHandler(vsg::Trackball::create(camera, ellipsoidModel));

        auto commandGraph = vsg::createCommandGraphForView(vw.window, camera, vsg_scene);
        vw.viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

        vw.viewer->compile();

        return true;
    };

    viewerWindow->frameCallback = [](vsgwx::ViewerWindow &vw) {
        if (!vw.viewer || !vw.viewer->advanceToNextFrame()) {
            return false;
        }

        vw.viewer->handleEvents();

        vw.viewer->update();

        vw.viewer->recordAndSubmit();

        vw.viewer->present();

        return true;
    };

    viewerWindow->Initialize(frame->GetSize().GetWidth(), frame->GetSize().GetHeight());

    return true;
}

void Application::OnIdle(wxIdleEvent &evt) {
    viewerWindow->PaintNow();
}
