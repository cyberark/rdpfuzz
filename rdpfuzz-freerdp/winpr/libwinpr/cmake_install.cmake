# Install script for directory: /home/cyber/rdp/FreeRDP/winpr/libwinpr

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
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwinpr3.so.3.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwinpr3.so.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwinpr3.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "$ORIGIN/../lib:$ORIGIN/..")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/cyber/rdp/FreeRDP/winpr/libwinpr/libwinpr3.so.3.0.0"
    "/home/cyber/rdp/FreeRDP/winpr/libwinpr/libwinpr3.so.3"
    "/home/cyber/rdp/FreeRDP/winpr/libwinpr/libwinpr3.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwinpr3.so.3.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwinpr3.so.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwinpr3.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH ":::::::::::::::::::::::::"
           NEW_RPATH "$ORIGIN/../lib:$ORIGIN/..")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/synch/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/locale/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/library/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/file/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/comm/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/pipe/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/interlocked/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/security/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/environment/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/crypto/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/registry/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/credentials/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/path/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/io/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/memory/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/input/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/shell/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/heap/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/utils/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/error/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/timezone/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/sysinfo/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/pool/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/handle/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/thread/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/winsock/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/sspi/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/winhttp/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/asn1/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/sspicli/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/crt/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/bcrypt/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/rpc/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/credui/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/wtsapi/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/dsparse/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/wnd/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/smartcard/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/nt/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/winpr/libwinpr/clipboard/cmake_install.cmake")

endif()

