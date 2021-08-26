# Install script for directory: /home/cyber/rdp/FreeRDP/channels

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/cyber/rdp/FreeRDP/channels/drdynvc/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/video/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/urbdrc/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/smartcard/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/serial/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/remdesk/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/rdpsnd/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/rdpgfx/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/rdpei/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/rdpdr/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/rdp2tcp/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/rail/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/printer/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/parallel/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/geometry/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/generic/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/encomsp/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/echo/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/drive/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/disp/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/cliprdr/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/audin/cmake_install.cmake")
  include("/home/cyber/rdp/FreeRDP/channels/client/cmake_install.cmake")

endif()

