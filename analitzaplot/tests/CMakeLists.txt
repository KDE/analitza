set(testLibs ${QT_QTXML_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} analitza analitzaplot analitzagui)

set(functiontest_SRCS planecurvetest.cpp)
kde4_add_unit_test(planecurvetest ${functiontest_SRCS})
target_link_libraries(planecurvetest ${testLibs})

set(functionsmodeltest_SRCS plotsmodeltest.cpp)
kde4_add_unit_test(plotsmodeltest ${functionsmodeltest_SRCS})
target_link_libraries(plotsmodeltest ${testLibs})
