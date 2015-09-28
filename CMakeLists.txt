project(Analitza)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.7.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

#Checking whether we can compile calgebra

set(QT_REQUIRED_VERSION 5.2)
find_package(Qt5 ${QT_REQUIRED_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Xml Svg Test Qml Quick)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMPackageConfigHelpers)
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)
include(GenerateExportHeader)
include(CheckIncludeFiles)
include(CheckLibraryExists) #FindCurses seems to need that on the CI
include(ECMPoQmTools)
ecm_create_qm_loader(analitza_QM_LOADER analitza_qt)
set(ANALITZA_INCLUDE_INSTALL_DIR "${KDE_INSTALL_INCLUDEDIR}/Analitza5")

ecm_setup_version(7.0.0 VARIABLE_PREFIX ANALITZA
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/analitza_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/Analitza5ConfigVersion.cmake")

find_package(OpenGL)
set_package_properties(OpenGL PROPERTIES TYPE OPTIONAL
                                         PURPOSE "Support for 3D graphs in Analitza"
                                         URL "http://opengl.org")
if(OPENGL_FOUND)
    find_package(Qt5OpenGL ${QT_REQUIRED_VERSION} REQUIRED NO_MODULE)
endif()
set(HAVE_OPENGL ${Qt5OpenGL_FOUND})

find_package(Eigen3) # find and setup Eigen3 if available
set(HAVE_EIGEN3 ${EIGEN3_FOUND})
if(EIGEN3_FOUND)
    include_directories(${EIGEN3_INCLUDE_DIR})
endif()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
remove_definitions(-DQT_NO_KEYWORDS)

add_subdirectory(analitza)
add_subdirectory(analitzaplot)
add_subdirectory(analitzagui)
add_subdirectory(analitzawidgets)
add_subdirectory(declarative)

if (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Analitza5")
ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/Analitza5Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/Analitza5Config.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS  INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config-analitza.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-analitza.h)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/Analitza5Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/Analitza5ConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT AnalitzaTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE Analitza5Targets.cmake NAMESPACE KF5:: )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/analitza_version.h
  DESTINATION ${ANALITZA_INCLUDE_INSTALL_DIR} COMPONENT Devel
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
