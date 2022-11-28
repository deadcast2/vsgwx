#include "Application.h"
#include "Frame.h"

#ifdef vsgXchange_FOUND
#include <vsgXchange/all.h>
#endif

wxIMPLEMENT_APP(Application);

bool Application::OnInit() {
    Frame *frame = new Frame("vsgwx");
    frame->Show(true);

    Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(Application::OnIdle));

    viewerWindow = new vsgwx::ViewerWindow(frame);

    vsg::CommandLine arguments(&argc, argv);

    // set up vsg::Options to pass in file paths and ReaderWriter's and other IO
    // related options to use when reading and writing files.
    auto options = vsg::Options::create();
    options->fileCache = vsg::getEnv("VSG_FILE_CACHE");
    options->paths = vsg::getEnvPaths("VSG_FILE_PATH");

#ifdef vsgXchange_all
    // add vsgXchange's support for reading and writing 3rd party file formats
    options->add(vsgXchange::all::create());
#endif

    arguments.read(options);

    viewerWindow->traits->windowTitle = "vsgwx viewer";
    viewerWindow->traits->debugLayer = arguments.read({"--debug", "-d"});
    viewerWindow->traits->apiDumpLayer = arguments.read({"--api", "-a"});

    if (arguments.errors())
        return arguments.writeErrorMessages(std::cerr);

    if (argc <= 1)
    {
        std::cout << "Please specify a 3d model or image file on the command line."
                  << std::endl;
        return false;
    }

    vsg::Path filename = arguments[1];

    auto vsg_scene = vsg::read_cast<vsg::Node>(filename, options);
    if (!vsg_scene)
    {
        std::cout << "Failed to load a valid scene graph, Please specify a 3d "
                     "model or image file on the command line."
                  << std::endl;
        return false;
    }

    viewerWindow->initializeCallback = [&](vsgwx::ViewerWindow &vw, uint32_t width, uint32_t height) {
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

void Application::OnIdle(wxIdleEvent &WXUNUSED(evt)) {
    viewerWindow->PaintNow();
}
