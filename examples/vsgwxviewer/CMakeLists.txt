set(SOURCES
    Application.cpp Frame.cpp
)

set(HEADERS
        Application.h Frame.h
)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

if(WIN32)
    add_executable(vsgwxviewer WIN32 ${MODE} ${SOURCES} ${HEADERS} ${FORMS})
else()
    add_executable(vsgwxviewer ${MODE} ${SOURCES} ${HEADERS} ${FORMS})
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(vsgwxviewer vsgwx ${wxWidgets_LIBRARIES} ${GTK3_LIBRARIES})
else()
    target_link_libraries(vsgwxviewer vsgwx ${wxWidgets_LIBRARIES})
endif()

if (vsgXchange_FOUND)
    target_compile_definitions(vsgwxviewer PRIVATE vsgXchange_FOUND)
    target_link_libraries(vsgwxviewer vsgXchange::vsgXchange)
endif()

install(TARGETS vsgwxviewer DESTINATION bin)
