add_executable(plots2ddemo plots2ddemo.cpp)
target_link_libraries(plots2ddemo AnalitzaPlot AnalitzaWidgets)

add_executable(plotsdictionarydemo plotsdictionarydemo.cpp)
target_link_libraries(plotsdictionarydemo AnalitzaWidgets)

add_executable(plots3ddemo plots3ddemo.cpp)
target_link_libraries(plots3ddemo AnalitzaWidgets Qt6::OpenGLWidgets)

add_executable(export3dplotdemo export3dplotdemo.cpp)
target_link_libraries(export3dplotdemo AnalitzaWidgets Qt6::OpenGLWidgets)

find_library(SNDFILE sndfile)
if (SNDFILE_FOUND)
    add_executable(audioplots audioplots.cpp)
    target_link_libraries(audioplots Qt::Core AnalitzaPlot sndfile)
endif()
