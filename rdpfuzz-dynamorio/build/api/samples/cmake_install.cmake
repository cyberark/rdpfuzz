# Install script for directory: C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/exports")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/bbbuf.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/bbbuf.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/bbcount.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/bbcount.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/bbsize.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/bbsize.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/div.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/div.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/empty.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/empty.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/memtrace_simple.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/memtrace_simple.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/memval_simple.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/memval_simple.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/instrace_simple.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/instrace_simple.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/opcode_count.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/opcode_count.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/cbr.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/cbr.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/countcalls.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/countcalls.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/inc2add.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/inc2add.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/memtrace_x86_binary.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/memtrace_x86_binary.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/memtrace_x86_text.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/memtrace_x86_text.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/instrace_x86_binary.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/instrace_x86_binary.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/instrace_x86_text.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/instrace_x86_text.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/prefetch.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/prefetch.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/modxfer.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/modxfer.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/modxfer_app2lib.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/modxfer_app2lib.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/instrcalls.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/instrcalls.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/cbrtrace.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/cbrtrace.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/hot_bbcount.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/hot_bbcount.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/wrap.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/wrap.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/signal.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/signal.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/syscall.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/syscall.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/inline.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/inline.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/inscount.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/inscount.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/opcodes.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/opcodes.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/stl_test.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/stl_test.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/stats.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE SHARED_LIBRARY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/stats.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE EXECUTABLE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/bin/tracedump.exe")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples" TYPE FILE FILES
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/bbbuf.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/bbcount.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/bbsize.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/div.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/empty.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/memtrace_simple.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/memval_simple.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/instrace_simple.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/opcode_count.cpp"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/cbr.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/countcalls.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/inc2add.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/memtrace_x86.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/memtrace_x86.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/instrace_x86.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/instrace_x86.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/prefetch.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/modxfer.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/modxfer_app2lib.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/instrcalls.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/cbrtrace.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/hot_bbcount.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/wrap.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/signal.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/syscall.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/inline.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/inscount.cpp"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/opcodes.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/stl_test.cpp"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.h"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/stats.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/utils.c"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/samples/tracedump.c"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples" TYPE FILE RENAME "CMakeLists.txt" FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/samples/CMakeLists.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE FILE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/lib64/release/dynamorio.dll")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples/bin64" TYPE DIRECTORY PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/samples/../bin/" FILES_MATCHING REGEX "/[^/]*\\.debug$" REGEX "/[^/]*\\.pdb$" REGEX ".*.dsym/.*dwarf/.*")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(WRITE "${CMAKE_INSTALL_PREFIX}/samples/README" "This directory contains sample clients that show how to use the DR API.  If this is an official release package that includes DRMF, for samples that use the DRMF API, see ../drmemory/drmf/samples/.
")
endif()

