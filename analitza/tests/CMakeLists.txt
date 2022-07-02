set(testLibs Qt::Core Qt::Xml Qt::Test Analitza)
if (QT_MAJOR_VERSION STREQUAL "6")
    set (testLibs ${testLibs} Qt6::Core5Compat)
endif()

ecm_add_tests(
    analitzatest.cpp
    expressiontest.cpp
    exptest.cpp
    mathmlpresentationtest.cpp
    typechecktest.cpp
    builtintest.cpp
    commandstest.cpp
    
    LINK_LIBRARIES ${testLibs}
)

ecm_add_test(
    TEST_NAME matchingtest
        matchingtest.cpp ../substituteexpression.cpp ../abstractexpressiontransformer.cpp
    LINK_LIBRARIES ${testLibs}
)
