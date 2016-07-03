ecm_add_tests(
    planecurvetest.cpp
    spacecurvetest.cpp
    surfacetest.cpp
    plotsmodeltest.cpp
    plotsdictionarymodeltest.cpp

    LINK_LIBRARIES Qt5::Xml Qt5::Test Qt5::Gui Analitza AnalitzaPlot AnalitzaGui
)

function(test_export name expression)
    add_test(NAME "export-test-${name}-stl" COMMAND export3dplotdemo "${expression}" "output-${name}.stl" ${ARGN})
    add_test(NAME "export-test-${name}-x3d" COMMAND export3dplotdemo "${expression}" "output-${name}.x3d" ${ARGN})
endfunction()

test_export(sin "sin x*sin y")
test_export(fancy "piecewise { x^2+y^2+z^2<35 ? 2-(cos(x+(1+5^0.5)/2*y)+cos(x-(1+5^0.5)/2*y)+cos(y+(1+5^0.5)/2*z)+cos(y-(1+5^0.5)/2*z)+cos(z-(1+5^0.5)/2*x)+cos(z+(1+5^0.5)/2*x)), ? 1 }=0")
