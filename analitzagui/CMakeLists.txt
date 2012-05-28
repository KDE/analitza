project(analitzagui)

add_subdirectory(tests)

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

	functionsview.cpp
)

kde4_add_library(analitzagui SHARED ${analitzagui_SRCS})
target_link_libraries(analitzagui ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDEUI_LIBS} ${QT_QTSVG_LIBRARY} analitza analitzaplot)

set_target_properties(analitzagui PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )

install(TARGETS analitzagui EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
	algebrahighlighter.h
	analitzaguiexport.h
	operatorsmodel.h
	variablesmodel.h
	functionsview.h
	expressionedit.h

        DESTINATION ${INCLUDE_INSTALL_DIR}/analitzagui/ COMPONENT Devel)
