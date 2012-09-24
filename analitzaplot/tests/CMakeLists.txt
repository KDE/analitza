set(testLibs ${QT_QTXML_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} analitza analitzaplot analitzagui)

kde4_add_unit_test(planecurvetest planecurvetest.cpp)
target_link_libraries(planecurvetest ${testLibs})

kde4_add_unit_test(plotsmodeltest plotsmodeltest.cpp)
target_link_libraries(plotsmodeltest ${testLibs})

kde4_add_unit_test(plotsdictionarymodeltest plotsdictionarymodeltest.cpp)
target_link_libraries(plotsdictionarymodeltest ${testLibs})
