FILE(GLOB datafiles *.plots)
install( FILES ${datafiles} DESTINATION  ${KDE_INSTALL_DATADIR}/libanalitza/plots )
