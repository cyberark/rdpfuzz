# Install script for directory: /home/cyber/rdp/FreeRDP/libfreerdp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibrariesx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreerdp3.so.3.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreerdp3.so.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreerdp3.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "$ORIGIN/../lib:$ORIGIN/..")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/cyber/rdp/FreeRDP/libfreerdp/libfreerdp3.so.3.0.0"
    "/home/cyber/rdp/FreeRDP/libfreerdp/libfreerdp3.so.3"
    "/home/cyber/rdp/FreeRDP/libfreerdp/libfreerdp3.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreerdp3.so.3.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreerdp3.so.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreerdp3.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/cyber/rdp/FreeRDP/winpr/libwinpr:"
           NEW_RPATH "$ORIGIN/../lib:$ORIGIN/..")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/cyber/rdp/FreeRDP/libfreerdp/freerdp3.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeRDP3" TYPE FILE FILES
    "/home/cyber/rdp/FreeRDP/libfreerdp/FreeRDPConfig.cmake"
    "/home/cyber/rdp/FreeRDP/libfreerdp/FreeRDPConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeRDP3/FreeRDPTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeRDP3/FreeRDPTargets.cmake"
         "/home/cyber/rdp/FreeRDP/libfreerdp/CMakeFiles/Export/lib/cmake/FreeRDP3/FreeRDPTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeRDP3/FreeRDPTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeRDP3/FreeRDPTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeRDP3" TYPE FILE FILES "/home/cyber/rdp/FreeRDP/libfreerdp/CMakeFiles/Export/lib/cmake/FreeRDP3/FreeRDPTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeRDP3" TYPE FILE FILES "/home/cyber/rdp/FreeRDP/libfreerdp/CMakeFiles/Export/lib/cmake/FreeRDP3/FreeRDPTargets-release.cmake")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/cyber/rdp/FreeRDP/libfreerdp/utils/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/libfreerdp/common/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/libfreerdp/gdi/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/libfreerdp/cache/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/libfreerdp/crypto/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/libfreerdp/locale/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/libfreerdp/core/cmake_install.cmake")

endif()

