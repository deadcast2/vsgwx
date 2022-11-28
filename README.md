# vsgwx

Open Source, cross platform C++ library providing integration of VulkanSceneGraph with wxWidgets. Supports Windows, Linux and macOS (coming soon). 

wxWidgets, and hence vsgwx, is suited for interactive, event driven applications. For real-time applications we recommend that you use the VulkanSceneGraph's native windowing support and the standard VulkanSceneGraph frame driven shown in examples found in [vsgExamples](https://github.com/vsg-dev/vsgExamples/examples/viewer/vsgviewer/vsgviewer.cpp).

![Screenshot from 2022-10-24 21-50-23](https://user-images.githubusercontent.com/45521946/197671323-25e43552-f255-4ef2-94c8-c4de4271d2f9.png)

## Checking out vsgwx

    git clone https://github.com/deadcast2/vsgwx.git

## Dependencies:

* [VulkanSDK](https://www.lunarg.com/vulkan-sdk/) version 1.2.162 or later
* [VulkanSceneGraph](https://github.com/vsg-dev/VulkanSceneGraph) master recommended.
* [CMake](https://cmake.org/) version 3.7 or later
* [wxWidgets](https://www.wxwidgets.org/) version 3.2.1 or later
* C++17 capable compiler

## Building vsgwx

    cd vsgwx
    cmake .
    make -j 8
    make install

## Example

* [vsgwxviewer](examples/vsgwxviewer/Application.cpp) - example of wxWidgets usage with vsgwx::ViewerWindow.
