set(testLibs Qt5::Xml Qt5::Test Qt5::Widgets Analitza AnalitzaPlot AnalitzaWidgets)

add_executable(plots2ddemo plots2ddemo.cpp)
target_link_libraries(plots2ddemo ${testLibs})

add_executable(plotsdictionarydemo plotsdictionarydemo.cpp)
target_link_libraries(plotsdictionarydemo ${testLibs})

add_executable(plots3ddemo plots3ddemo.cpp)
target_link_libraries(plots3ddemo ${testLibs})

add_executable(export3dplotdemo export3dplotdemo.cpp)
target_link_libraries(export3dplotdemo ${testLibs})
