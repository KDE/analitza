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
functionutils.cpp
function.cpp
functiongraph.cpp
functiongraphfactory.cpp
functionsmodel.cpp

# plotter2d.cpp
# plotview2d.cpp
# plotter3d.cpp
# plotview3d.cpp

functiongraphtypes/cartesiansurface.cpp
functiongraphtypes/implicitcurve.cpp
functiongraphtypes/cartesiancurve.cpp
# functiongraphtypes/cylindricalsurface.cpp
functiongraphtypes/parametriccurve2d.cpp
# functiongraphtypes/parametriccurve3d.cpp
# functiongraphtypes/parametricsurface.cpp
functiongraphtypes/polarcurve.cpp
# functiongraphtypes/sphericalsurface.cpp

#   solvers/solvers3d/implicitsurface.cpp
#
#   #MarchingCubes
#   solvers/solvers3d/MarchingCubes/ctab.h
#   solvers/solvers3d/MarchingCubes/ctab.cpp
#   solvers/solvers3d/MarchingCubes/formulas.h
#   solvers/solvers3d/MarchingCubes/glwidget.h
#   solvers/solvers3d/MarchingCubes/glwidget.cpp
#   solvers/solvers3d/MarchingCubes/marchingcubes.cpp
#   solvers/solvers3d/MarchingCubes/marchingcubes.h
#   solvers/solvers3d/MarchingCubes/octree.h
#   solvers/solvers3d/MarchingCubes/octree.cpp
#   solvers/solvers3d/MarchingCubes/qtlogo.h
#   solvers/solvers3d/MarchingCubes/qtlogo.cpp
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

analitzaplotexport.h

functionutils.h
function.h
functiongraph.h
functiongraphfactory.h
functionsmodel.h

plotter2d.h
plotter3d.h
plotview2d.h
plotview3d.h


        DESTINATION ${INCLUDE_INSTALL_DIR}/analitzaplot/ COMPONENT Devel)
