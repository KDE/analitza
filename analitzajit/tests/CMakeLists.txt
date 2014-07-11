include_directories(${analitzajit_SOURCE_DIR})

set(testLibs Qt5::Core Qt5::Xml Qt5::Test Analitza AnalitzaJIT)

ecm_add_test(
    TEST_NAME analitzajittest
    analitzajittest.cpp

    LINK_LIBRARIES ${testLibs}
)
