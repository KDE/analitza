set(testLibs Qt5::Xml Qt5::Test Qt5::Widgets ${KDE4_KDEUI_LIBS} analitza analitzaplot analitzagui)

add_executable(plots2ddemo plots2ddemo.cpp)
target_link_libraries(plots2ddemo ${testLibs})

add_executable(plotsdictionarydemo plotsdictionarydemo.cpp)
target_link_libraries(plotsdictionarydemo ${testLibs})

if(HAVE_OPENGL)
    add_executable(plots3ddemo plots3ddemo.cpp)
    target_link_libraries(plots3ddemo  Qt5::OpenGL ${testLibs})
else(HAVE_OPENGL)
    message(STATUS "disabling plots3ddemo because OpenGL is not available")
endif(HAVE_OPENGL)
