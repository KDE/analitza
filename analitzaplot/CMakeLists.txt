project(analitzaplot)



add_subdirectory(tests)

find_package ( OpenGL REQUIRED )

if(NOT OPENGL_GLU_FOUND)
    message(FATAL_ERROR "GLU was not found")
endif(NOT OPENGL_GLU_FOUND)

find_library(QGLVIEWER_LIBRARY NAMES qglviewer-qt4 QGLViewer)

find_path(IEEEFP_DIR ieeefp.h)
if(IEEEFP_DIR)
    include_directories(${IEEEFP_DIR})
    add_definitions(-DHAVE_IEEEFP_H)
endif(IEEEFP_DIR)


include_directories (
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  .
  ..
)

set ( keomath_SRCS
private/utils/mathutils.cpp

private/abstractfunctiongraph.cpp
private/functiongraphfactory.cpp
private/abstractmappinggraph.cpp
private/abstractplanecurve.cpp
private/abstractspacecurve.cpp
private/abstractsurface.cpp
private/plotitem.cpp
private/functiongraph.cpp

private/backends/cartesiansurface.cpp
private/backends/implicitcurve.cpp
private/backends/cartesiancurve.cpp
private/backends/cylindricalsurface.cpp
private/backends/parametriccurve2d.cpp
private/backends/parametriccurve3d.cpp
private/backends/parametricsurface.cpp
private/backends/polarcurve.cpp
private/backends/sphericalsurface.cpp
private/backends/implicitsurface.cpp

#public API
planecurve.cpp
spacecurve.cpp
surface.cpp
plotsmodel.cpp
plotter2d.cpp
plotsview3d.cpp
plotter3d.cpp
plotsview2d.cpp

)

kde4_add_library( analitzaplot SHARED ${keomath_SRCS} )
target_link_libraries ( analitzaplot
  #OpenGL
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  #${GLEW_GLEW_LIBRARY}
  #Qt
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  ${QT_QTXML_LIBRARY}
  #KDE
  #${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  analitza
  ${QGLVIEWER_LIBRARY}
)
set_target_properties(analitzaplot PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )

install(TARGETS analitzaplot EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES


#public api
analitzaplotexport.h
mathutils.h
planecurve.h
planecurvesmodel.h
spacecurve.h
plotter2d.h
plotter3d.h
plotview2d.h
plotview3d.h
surface.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/analitzaplot/ COMPONENT Devel)


install(FILES

#private api/files only for compile by using the headers

private/functiongraphfactory.h
private/abstractfunctiongraph.h
private/abstractmappinggraph.h
private/abstractplanecurve.h
private/abstractsurface.h
private/functiongraph.h
private/functiongraphsmodel.h
private/mappinggraph.h

        DESTINATION ${INCLUDE_INSTALL_DIR}/analitzaplot/private COMPONENT Devel)






# analitzaplotexport.h
# 
# functionutils.h
# function.h
# functiongraph.h
# functiongraphfactory.h
# functionsmodel.h
# 
# plotter2d.h
# plotter3d.h
# plotview2d.h
# plotview3d.h


