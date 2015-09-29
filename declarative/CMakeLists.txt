project(analitza-declarative)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
)

add_library(analitzadeclarativeplugin SHARED analitzadeclarativeplugin.cpp graph2dmobile.cpp graph3ditem analitzawrapper.cpp)

target_link_libraries(analitzadeclarativeplugin
	Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick
	Analitza AnalitzaGui AnalitzaPlot)

install(TARGETS analitzadeclarativeplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/analitza)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/analitza)

install( FILES
        qml/Graph2D.qml
        DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/analitza
)
