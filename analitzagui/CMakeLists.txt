add_subdirectory(tests)

add_library(AnalitzaGui operatorsmodel.cpp algebrahighlighter.cpp variablesmodel.cpp)
target_link_libraries(AnalitzaGui Qt5::Core Qt5::Gui Analitza)

set_target_properties(AnalitzaGui PROPERTIES VERSION ${ANALITZA_VERSION_STRING} SOVERSION ${ANALITZA_SOVERSION} )

generate_export_header(AnalitzaGui EXPORT_FILE_NAME analitzaguiexport.h)

install(TARGETS AnalitzaGui EXPORT AnalitzaTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/analitzaguiexport.h
	algebrahighlighter.h
	operatorsmodel.h
	variablesmodel.h

    DESTINATION ${ANALITZA_INCLUDE_INSTALL_DIR}/analitzagui/ COMPONENT Devel
)
