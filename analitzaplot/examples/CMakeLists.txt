

set(testLibs ${QT_QTXML_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} analitza analitzaplot analitzagui)

set(plotview2dtest_SRCS plots2ddemo.cpp)
kde4_add_executable(plotview2dtest ${plotview2dtest_SRCS})
target_link_libraries(plotview2dtest ${testLibs} ${KDE4_KDEUI_LIBS})


set(plotview3dtest_SRCS plots3ddemo.cpp)
kde4_add_executable(plotview3dtest ${plotview3dtest_SRCS})
target_link_libraries(plotview3dtest ${testLibs} ${KDE4_KDEUI_LIBS})




