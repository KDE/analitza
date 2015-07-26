project(analitzagui)

add_subdirectory(tests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_path(IEEEFP_DIR ieeefp.h)
if(IEEEFP_DIR)
    include_directories(${IEEEFP_DIR})
    add_definitions(-DHAVE_IEEEFP_H)
endif(IEEEFP_DIR)

set(analitzagui_SRCS
	operatorsmodel.cpp
	algebrahighlighter.cpp
	variablesmodel.cpp
	expressionedit.cpp
    plotsview2d.cpp
)
if(HAVE_OPENGL)
    list(APPEND analitzagui_SRCS plotsview3d.cpp)
endif(HAVE_OPENGL)

kde4_add_library(analitzagui SHARED ${analitzagui_SRCS})
target_link_libraries(analitzagui LINK_PRIVATE ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDEUI_LIBS} ${QT_QTSVG_LIBRARY} ${QT_QTOPENGL_LIBRARY} analitza analitzaplot)

set_target_properties(analitzagui PROPERTIES VERSION ${ANALITZA_LIB_VERSION} SOVERSION ${ANALITZA_LIB_SOVERSION} )

install(TARGETS analitzagui EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
	algebrahighlighter.h
	analitzaguiexport.h
	operatorsmodel.h
	variablesmodel.h
	expressionedit.h
	plotsview2d.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/analitzagui/ COMPONENT Devel
)

if(HAVE_OPENGL)
    install(FILES
        plotsview3d.h

        DESTINATION ${INCLUDE_INSTALL_DIR}/analitzagui/ COMPONENT Devel
    )
endif(HAVE_OPENGL)
