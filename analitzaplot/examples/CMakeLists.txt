set(testLibs ${QT_QTXML_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} analitza analitzaplot analitzagui)

kde4_add_executable(plots2ddemo plots2ddemo.cpp)
target_link_libraries(plots2ddemo ${testLibs})

kde4_add_executable(plotsdictionarydemo plotsdictionarydemo.cpp)
target_link_libraries(plotsdictionarydemo ${testLibs})

if(HAVE_OPENGL)
    kde4_add_executable(plots3ddemo plots3ddemo.cpp)
    target_link_libraries(plots3ddemo  ${QT_QTOPENGL_LIBRARY} ${testLibs})
else(HAVE_OPENGL)
    message(STATUS "disabling plots3ddemo because OpenGL is not available")
endif(HAVE_OPENGL)