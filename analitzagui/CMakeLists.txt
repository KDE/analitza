project(AnalitzaGui)

add_subdirectory(tests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(analitzagui_SRCS
	operatorsmodel.cpp
	algebrahighlighter.cpp
	variablesmodel.cpp
)
add_library(AnalitzaGui ${analitzagui_SRCS})
target_link_libraries(AnalitzaGui Qt5::Core Qt5::Gui Analitza)

set_target_properties(AnalitzaGui PROPERTIES VERSION ${ANALITZA_VERSION_STRING} SOVERSION ${ANALITZA_SOVERSION} )

install(TARGETS AnalitzaGui EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
	algebrahighlighter.h
	analitzaguiexport.h
	operatorsmodel.h
	variablesmodel.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/analitzagui/ COMPONENT Devel
)
