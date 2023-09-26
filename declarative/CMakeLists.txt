# SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clauses

ecm_add_qml_module(analitzadeclarativeplugin URI "org.kde.analitza" VERSION 1.0 GENERATE_PLUGIN_SOURCE)

target_sources(analitzadeclarativeplugin PRIVATE
    graph2dmobile.cpp
    graph3ditem.cpp
    analitzawrapper.cpp
)

ecm_target_qml_sources(analitzadeclarativeplugin SOURCES
    qml/Graph2D.qml
    qml/Graph3D.qml
)

target_link_libraries(analitzadeclarativeplugin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Analitza
    AnalitzaGui
    AnalitzaPlot
)

ecm_finalize_qml_module(analitzadeclarativeplugin)
