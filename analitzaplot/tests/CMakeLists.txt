ecm_add_tests(
    planecurvetest.cpp
    spacecurvetest.cpp
    surfacetest.cpp
    plotsmodeltest.cpp
    plotsdictionarymodeltest.cpp

    LINK_LIBRARIES Qt5::Xml Qt5::Test Qt5::Gui Analitza AnalitzaPlot AnalitzaGui
)
