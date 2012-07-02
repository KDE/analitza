

set(testLibs ${QT_QTXML_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} analitza analitzaplot analitzagui)

set(functiontest_SRCS planecurvetest.cpp)
kde4_add_unit_test(planecurvetest ${functiontest_SRCS})
target_link_libraries(planecurvetest ${testLibs})

set(functionsmodeltest_SRCS planecurvesmodeltest.cpp)
kde4_add_unit_test(planecurvesmodeltest ${functionsmodeltest_SRCS})
target_link_libraries(planecurvesmodeltest ${testLibs})

set(plotview2dtest_SRCS plotview2dtest.cpp)
kde4_add_executable(plotview2dtest ${plotview2dtest_SRCS})
target_link_libraries(plotview2dtest ${testLibs} ${KDE4_KDEUI_LIBS})


set(plotview3dtest_SRCS plotview3dtest.cpp)
kde4_add_executable(plotview3dtest ${plotview3dtest_SRCS})
target_link_libraries(plotview3dtest ${testLibs} ${KDE4_KDEUI_LIBS})




