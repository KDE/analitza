set(testLibs ${QT_QTXML_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} analitza analitzaplot analitzagui)

set(plotview2dtest_SRCS plots2ddemo.cpp)
kde4_add_executable(plots2ddemo ${plotview2dtest_SRCS})
target_link_libraries(plots2ddemo ${testLibs} ${KDE4_KDEUI_LIBS})

set(plotview3dtest_SRCS plots3ddemo.cpp)
kde4_add_executable(plots3ddemo ${plotview3dtest_SRCS})
target_link_libraries(plots3ddemo ${testLibs} ${KDE4_KDEUI_LIBS})
