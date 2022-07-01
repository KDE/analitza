add_library(analitzadeclarativeplugin SHARED
    analitzadeclarativeplugin.cpp
    graph2dmobile.cpp
    graph3ditem.cpp
    analitzawrapper.cpp
)

target_link_libraries(analitzadeclarativeplugin
	Qt::Core Qt::Gui Qt::Qml Qt::Quick
	Analitza AnalitzaGui AnalitzaPlot)

install(TARGETS analitzadeclarativeplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/analitza)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/analitza)

install( FILES
        qml/Graph2D.qml
        qml/Graph3D.qml
        DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/analitza
)
