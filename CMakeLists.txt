cmake_minimum_required(VERSION 3.16)
set(ANALITZA_VERSION "8.0.0")
project(Analitza VERSION ${ANALITZA_VERSION})

set(QT_MIN_VERSION "6.5.2")
set(KF_MIN_VERSION "5.240.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

#Checking whether we can compile calgebra


include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(GenerateExportHeader)
include(CheckIncludeFiles)
include(CheckLibraryExists) #FindCurses seems to need that on the CI
include(ECMPoQmTools)
include(ECMQmlModule)
include(ECMDeprecationSettings)
find_package(Qt6 ${QT_REQUIRED_VERSION} REQUIRED NO_MODULE COMPONENTS
    Widgets
    Xml
    Svg
    Test
    Qml
    Quick
    PrintSupport
    Core5Compat
    OpenGLWidgets
)
set(ANALITZA_INCLUDE_INSTALL_DIR "${KDE_INSTALL_INCLUDEDIR}/Analitza6")

ecm_setup_version(8.0.0 VARIABLE_PREFIX ANALITZA
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/analitza_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/Analitza6ConfigVersion.cmake")

find_package(Eigen3 CONFIG) # find and setup Eigen3 if available
set(HAVE_EIGEN3 ${EIGEN3_FOUND})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
ecm_set_disabled_deprecation_versions(QT 5.12.2
    KF 5.102
)


add_subdirectory(analitza)
add_subdirectory(analitzaplot)
add_subdirectory(analitzagui)
add_subdirectory(analitzawidgets)
add_subdirectory(declarative)

ecm_install_po_files_as_qm(poqm)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Analitza6")
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/Analitza6Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/Analitza6Config.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS  KDE_INSTALL_INCLUDEDIR CMAKE_INSTALL_PREFIX
)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config-analitza.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-analitza.h)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/Analitza6Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/Analitza6ConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT AnalitzaTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE Analitza6Targets.cmake NAMESPACE KF6:: )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/analitza_version.h
  DESTINATION ${ANALITZA_INCLUDE_INSTALL_DIR} COMPONENT Devel
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
