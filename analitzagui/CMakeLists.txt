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

add_library(analitzagui SHARED ${analitzagui_SRCS})
target_link_libraries(analitzagui Qt5::Core Qt5::Widgets ${KDE4_KDEUI_LIBS} Qt5::Svg Qt5::OpenGL analitza analitzaplot)

set_target_properties(analitzagui PROPERTIES VERSION ${ANALITZA_VERSION_STRING} SOVERSION ${ANALITZA_SOVERSION} )

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
