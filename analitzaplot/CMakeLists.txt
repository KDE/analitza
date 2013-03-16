project(analitzaplot)

if(OPENGL_FOUND AND NOT OPENGL_GLU_FOUND)
    message(FATAL_ERROR "GLU was not found")
endif(OPENGL_FOUND AND NOT OPENGL_GLU_FOUND)

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


if(Boost_FOUND)
 include_directories (
   ${Boost_INCLUDE_DIRS}
 )
endif(Boost_FOUND)

if(Boost_FOUND)
 include_directories (
   ${EIGEN3_INCLUDE_DIR}
 )
endif(Boost_FOUND)

# set ( analitzaplot_SRCS
#     private/utils/mathutils.cpp
#     private/utils/quadtree.cpp
#     private/utils/marchingsquares.cpp
#     private/utils/octree.cpp
#     private/utils/marchingcubes.cpp
# 
#     private/abstractplotitem.cpp
#     private/abstractfunctiongraph.cpp
#     private/abstractplanecurve.cpp
#     private/abstractspacecurve.cpp
#     private/abstractsurface.cpp
#     private/functiongraphfactory.cpp
# 
#     private/backends/cartesiansurface.cpp
#     private/backends/implicitcurve.cpp
#     private/backends/cartesiancurve.cpp
#     private/backends/cylindricalsurface.cpp
#     private/backends/parametriccurve2d.cpp
#     private/backends/parametriccurve3d.cpp
#     private/backends/parametricsurface.cpp
#     private/backends/polarcurve.cpp
#     private/backends/implicitpolarcurve.cpp
#     private/backends/sphericalsurface.cpp
#     private/backends/implicitsurface.cpp
# 
#     plotsfactory.cpp
#     shapeinterface.cpp
#     vectorfield.cpp
#     curve.cpp
#     phaseportrait.cpp
#     surface.cpp
#     curvesmodel.cpp
#     plotsdictionarymodel.cpp
#     plotter2d.cpp
# )
set ( analitzaplot_SRCS
curve.cpp
shapesmodel.cpp
)

# if(OPENGL_FOUND)
    list(APPEND analitzaplot_SRCS plotter.cpp)
# endif(OPENGL_FOUND)

# if(Boost_FOUND)
#     list(APPEND analitzaplot_SRCS private/backends/integralcurve.cpp)
# endif(Boost_FOUND)

kde4_add_library( analitzaplot SHARED ${analitzaplot_SRCS} )
target_link_libraries ( analitzaplot
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  ${KDE4_KDECORE_LIBS}

  analitza
)

# if(OPENGL_FOUND)
    target_link_libraries ( analitzaplot

    )
# endif(OPENGL_FOUND)

set_target_properties(analitzaplot PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )

install(TARGETS analitzaplot EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

# install(FILES
#     analitzaplotexport.h
#     plotitem.h
#     functiongraph.h
#     planecurve.h
#     spacecurve.h
#     surface.h
#     plotsmodel.h
#     plotsdictionarymodel.h
#     plotter2d.h
#     plottingenums.h
#     plotsfactory.h
# 
#     DESTINATION ${INCLUDE_INSTALL_DIR}/analitzaplot/ COMPONENT Devel
# )

install(FILES
    analitzaplotexport.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/analitzaplot/ COMPONENT Devel
)


# if(OPENGL_FOUND)
#     install(FILES
#         plotter3d.h DESTINATION ${INCLUDE_INSTALL_DIR}/analitzaplot/ COMPONENT Devel)
# endif(OPENGL_FOUND)

add_subdirectory(data)
add_subdirectory(tests)
add_subdirectory(examples)

