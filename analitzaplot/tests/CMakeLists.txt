add_definitions("-DSOURCE_DIR=\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\"")

ecm_add_tests(
    planecurvetest.cpp
    spacecurvetest.cpp
    surfacetest.cpp
    plotsmodeltest.cpp
    plotsdictionarymodeltest.cpp

    LINK_LIBRARIES Qt5::Xml Qt5::Test Qt5::Gui  analitza analitzaplot analitzagui
)
