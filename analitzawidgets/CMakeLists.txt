find_path(IEEEFP_DIR ieeefp.h)
if(IEEEFP_DIR)
    include_directories(${IEEEFP_DIR})
    add_definitions(-DHAVE_IEEEFP_H)
endif(IEEEFP_DIR)

set(analitzaplot_SRCS
    plotsview2d.cpp
    plotsview3d_es.cpp
    expressionedit.cpp
)

add_library(AnalitzaWidgets ${analitzaplot_SRCS})
target_link_libraries(AnalitzaWidgets PUBLIC Qt5::Widgets Analitza AnalitzaPlot AnalitzaGui
                                      PRIVATE Qt5::Svg
)

set_target_properties(AnalitzaWidgets PROPERTIES VERSION ${ANALITZA_VERSION_STRING} SOVERSION ${ANALITZA_SOVERSION} )

generate_export_header(AnalitzaWidgets)

install(TARGETS AnalitzaWidgets EXPORT AnalitzaTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/analitzawidgets_export.h
    plotsview2d.h
    expressionedit.h
    plotsview3d_es.h

    DESTINATION ${ANALITZA_INCLUDE_INSTALL_DIR}/analitzagui/ COMPONENT Devel
)
