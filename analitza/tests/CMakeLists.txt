set(testLibs Qt6::Core Qt6::Xml Qt6::Test Analitza)

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
