FILE(GLOB datafiles *.plots)
install( FILES ${datafiles} DESTINATION  ${DATA_INSTALL_DIR}/libanalitza/data/plots/es )
# add_custom_target( libanalitza_data )


