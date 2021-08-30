# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BINARY_7Z "")
set(CPACK_BINARY_BUNDLE "")
set(CPACK_BINARY_CYGWIN "")
set(CPACK_BINARY_DEB "")
set(CPACK_BINARY_DRAGNDROP "")
set(CPACK_BINARY_FREEBSD "")
set(CPACK_BINARY_IFW "")
set(CPACK_BINARY_NSIS "")
set(CPACK_BINARY_NUGET "")
set(CPACK_BINARY_OSXX11 "")
set(CPACK_BINARY_PACKAGEMAKER "")
set(CPACK_BINARY_PRODUCTBUILD "")
set(CPACK_BINARY_RPM "")
set(CPACK_BINARY_STGZ "")
set(CPACK_BINARY_TBZ2 "")
set(CPACK_BINARY_TGZ "")
set(CPACK_BINARY_TXZ "")
set(CPACK_BINARY_TZ "")
set(CPACK_BINARY_WIX "")
set(CPACK_BINARY_ZIP "")
set(CPACK_BUILD_SOURCE_DIRS "C:/Users/orbp/source/repos/rdpfuzz-dynamorio;C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build")
set(CPACK_CMAKE_GENERATOR "Visual Studio 15 2017 Win64")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_GENERATOR "ZIP")
set(CPACK_INSTALL_CMAKE_PROJECTS "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build;DynamoRIO;ALL;/")
set(CPACK_INSTALL_PREFIX "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/exports")
set(CPACK_MODULE_PATH "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/make/modules")
set(CPACK_NSIS_DISPLAY_NAME "dynamorio")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_PACKAGE_NAME "dynamorio")
set(CPACK_OUTPUT_CONFIG_FILE "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/README")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DynamoRIO Dynamic Instrumentation Tool Platform")
set(CPACK_PACKAGE_FILE_NAME "DynamoRIO-Windows-8.0.18639")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "dynamorio")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "DynamoRIO")
set(CPACK_PACKAGE_NAME "DynamoRIO")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "DynamoRIO community")
set(CPACK_PACKAGE_VERSION "8.0.18639")
set(CPACK_PACKAGE_VERSION_MAJOR "8")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "18639")
set(CPACK_RESOURCE_FILE_LICENSE "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/License.txt")
set(CPACK_RESOURCE_FILE_README "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/README")
set(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.12/Templates/CPack.GenericWelcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_7Z "")
set(CPACK_SOURCE_CYGWIN "")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/CPackSourceConfig.cmake")
set(CPACK_SOURCE_RPM "")
set(CPACK_SOURCE_TBZ2 "")
set(CPACK_SOURCE_TGZ "")
set(CPACK_SOURCE_TXZ "")
set(CPACK_SOURCE_TZ "")
set(CPACK_SOURCE_ZIP "")
set(CPACK_SYSTEM_NAME "Windows")
set(CPACK_TOPLEVEL_TAG "Windows")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
