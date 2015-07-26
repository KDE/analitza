project(analitzaplot)

if(OPENGL_FOUND AND NOT OPENGL_GLU_FOUND)
    message(FATAL_ERROR "GLU was not found")
endif(OPENGL_FOUND AND NOT OPENGL_GLU_FOUND)

if(WIN32)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
endif(WIN32)

find_path(IEEEFP_DIR ieeefp.h)
if(IEEEFP_DIR)
    include_directories(${IEEEFP_DIR})
    add_definitions(-DHAVE_IEEEFP_H)
endif(IEEEFP_DIR)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
)

if(OPENGL_FOUND)
 include_directories (
  ${OPENGL_INCLUDE_DIR}
 )
endif(OPENGL_FOUND)

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
)
if(HAVE_OPENGL)
    list(APPEND analitzaplot_SRCS plotter3d.cpp)
endif(HAVE_OPENGL)

kde4_add_library( analitzaplot SHARED ${analitzaplot_SRCS} )
target_link_libraries ( analitzaplot
   LINK_PRIVATE
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${KDE4_KDECORE_LIBS}

  analitza
)

if(HAVE_OPENGL)
    target_link_libraries ( analitzaplot
     LINK_PRIVATE
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
    )
endif(HAVE_OPENGL)

if(WIN32)
   target_link_libraries(analitzaplot LINK_PRIVATE ${GLEW_LIBRARIES})
endif(WIN32)

set_target_properties(analitzaplot PROPERTIES VERSION ${ANALITZA_LIB_VERSION} SOVERSION ${ANALITZA_LIB_SOVERSION} )

install(TARGETS analitzaplot EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    analitzaplotexport.h
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

    DESTINATION ${INCLUDE_INSTALL_DIR}/analitzaplot/ COMPONENT Devel
)

if(HAVE_OPENGL)
    install(FILES
        plotter3d.h DESTINATION ${INCLUDE_INSTALL_DIR}/analitzaplot/ COMPONENT Devel)
endif(HAVE_OPENGL)

add_subdirectory(data)
add_subdirectory(tests)
add_subdirectory(examples)

