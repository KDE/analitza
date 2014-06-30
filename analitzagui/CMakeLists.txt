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

generate_export_header(AnalitzaGui EXPORT_FILE_NAME analitzaguiexport.h)

install(TARGETS AnalitzaGui EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/analitzaguiexport.h
	algebrahighlighter.h
	operatorsmodel.h
	variablesmodel.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/analitzagui/ COMPONENT Devel
)
