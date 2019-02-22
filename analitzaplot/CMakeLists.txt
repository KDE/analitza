if(WIN32)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
endif(WIN32)

find_path(IEEEFP_DIR ieeefp.h)
if(IEEEFP_DIR)
    include_directories(${IEEEFP_DIR})
    add_definitions(-DHAVE_IEEEFP_H)
endif(IEEEFP_DIR)

# MSVC and MinGW don't provide M_PI and friends in math.h,
# unless the right #define is set:
if(MSVC)
    add_definitions(-D_USE_MATH_DEFINES)
elseif(MINGW)
    add_definitions(-D_BSD_SOURCE)
endif()

set ( analitzaplot_SRCS
    private/utils/mathutils.cpp
    private/utils/quadtree.cpp
    private/utils/marchingsquares.cpp
    private/utils/marchingcubes.cpp

    private/abstractplotitem.cpp
    private/abstractfunctiongraph.cpp
    private/abstractplanecurve.cpp
    private/abstractspacecurve.cpp
    private/abstractsurface.cpp
    private/functiongraphfactory.cpp
    private/export3d.cpp

    private/backends/cartesiansurface.cpp
    private/backends/implicitcurve.cpp
    private/backends/cartesiancurve.cpp
    private/backends/cylindricalsurface.cpp
    private/backends/parametriccurve2d.cpp
    private/backends/parametriccurve3d.cpp
    private/backends/parametricsurface.cpp
    private/backends/polarcurve.cpp
    private/backends/implicitpolarcurve.cpp
    private/backends/sphericalsurface.cpp
    private/backends/implicitsurface.cpp

    plotsfactory.cpp
    plotitem.cpp
    functiongraph.cpp
    planecurve.cpp
    spacecurve.cpp
    surface.cpp
    plotsmodel.cpp
    plotsdictionarymodel.cpp
    plotter2d.cpp
    plotter3d_es.cpp
)

add_library( AnalitzaPlot ${analitzaplot_SRCS} )
target_link_libraries ( AnalitzaPlot
PUBLIC
  Analitza
  Qt5::Core
  Qt5::Gui

PRIVATE
  Qt5::PrintSupport # this pulls widgets
)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    target_link_libraries(AnalitzaPlot PRIVATE GLESv2)
elseif(WIN32)
    target_link_libraries(AnalitzaPlot PRIVATE ${GLEW_LIBRARIES})
endif()

set_target_properties(AnalitzaPlot PROPERTIES VERSION ${ANALITZA_VERSION_STRING} SOVERSION ${ANALITZA_SOVERSION} )

generate_export_header(AnalitzaPlot EXPORT_FILE_NAME analitzaplotexport.h)

install(TARGETS AnalitzaPlot EXPORT AnalitzaTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/analitzaplotexport.h
    plotitem.h
    functiongraph.h
    planecurve.h
    spacecurve.h
    surface.h
    plotsmodel.h
    plotsdictionarymodel.h
    plotter2d.h
    plottingenums.h
    plotsfactory.h
    plotter3d_es.h

    DESTINATION ${ANALITZA_INCLUDE_INSTALL_DIR}/analitzaplot/ COMPONENT Devel
)

add_subdirectory(data)
add_subdirectory(tests)
add_subdirectory(examples)

