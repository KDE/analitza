FILE(GLOB datafiles *.plots)
install( FILES ${datafiles} DESTINATION  ${DATA_INSTALL_DIR}/libanalitza/plots )
