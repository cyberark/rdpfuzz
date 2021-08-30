# Install script for directory: C:/Users/orbp/source/repos/rdpfuzz-dynamorio

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/cmake/DynamoRIOConfig.cmake"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/cmake/DynamoRIOConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/suite/runsuite_common_pre.cmake"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/suite/runsuite_common_post.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/suite/lookup_visualstudio.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/make/cpp2asm_support.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE RENAME "cpp2asm_add_newlines.cmake" FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/make/CMake_asm.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(READ "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/core/arch/asm_defines.asm" str)
  string(REPLACE "#include \"configure.h\"" "" str "${str}")
  file(WRITE "${CMAKE_INSTALL_PREFIX}/cmake/cpp2asm_defines.h" "${str}")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE RENAME "docs_rundoxygen.cmake" FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/docs/CMake_rundoxygen.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE RENAME "docs_doxyutils.cmake" FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/api/docs/CMake_doxyutils.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/include" REGEX "annotations/valgrind.h|annotations/memcheck.h" EXCLUDE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/README"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/License.txt"
    "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/ACKNOWLEDGEMENTS"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE DIRECTORY PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/lib64/" FILES_MATCHING REGEX "/[^/]*\\.debug$" REGEX "/[^/]*\\.pdb$" REGEX ".*.dsym/.*dwarf/.*" REGEX "/dynamorio\\.pdb$" EXCLUDE REGEX "/libdynamorio\\.so\\.[^/]*debug$" EXCLUDE REGEX "/libdrpreload\\.so\\.debug$" EXCLUDE REGEX "/policy\\_static\\.pdb$" EXCLUDE REGEX ".*dynamorio.*.dsym/.*" EXCLUDE REGEX "libdrpreload.*.dsym/.*" EXCLUDE REGEX "policy_static.*.dsym/.*" EXCLUDE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/release" TYPE DIRECTORY PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/lib64/release/" FILES_MATCHING REGEX "/dynamorio\\.pdb$" REGEX "/libdynamorio\\.so\\.[^/]*debug$" REGEX "/libdrpreload\\.so\\.debug$" REGEX ".*dynamorio.*.dsym/.*dwarf/.*" REGEX "libdrpreload.*.dsym/.*dwarf/.*")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin64" TYPE DIRECTORY PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/bin64/" FILES_MATCHING REGEX "/[^/]*\\.debug$" REGEX "/[^/]*\\.pdb$" REGEX ".*.dsym/.*dwarf/.*" REGEX "/runstats\\.debug$" EXCLUDE REGEX "/run\\_in\\_bg\\.debug$" EXCLUDE REGEX "/[^/]*\\_exe\\.debug$" EXCLUDE REGEX "runstats.dsym/.*" EXCLUDE REGEX "run_in_bg.dsym/.*" EXCLUDE REGEX ".*_exe.dsym/.*" EXCLUDE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/logs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(WRITE "${CMAKE_INSTALL_PREFIX}/logs/README" "Empty dir for debug-build log files.
")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(APPEND "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/CMakeFiles/Export/cmake/DynamoRIOTarget64.cmake" "
SET_PROPERTY(TARGET ntdll_imports PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET ntdll_imports PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET dynamorio PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET dynamorio PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET dynamorio_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET dynamorio_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drinjectlib PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drinjectlib PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drdecode PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drdecode PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drlibc PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drlibc PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drconfiglib PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drconfiglib PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drfrontendlib PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drfrontendlib PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_reuse_distance PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_reuse_distance PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_histogram PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_histogram PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_reuse_time PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_reuse_time PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_basic_counts PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_basic_counts PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_opcode_mix PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_opcode_mix PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_view PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_view PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_func_view PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_func_view PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_simulator PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_simulator PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET directory_iterator PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET directory_iterator PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_raw2trace PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_raw2trace PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmemtrace_analyzer PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmemtrace_analyzer PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drcontainers PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drcontainers PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmgr PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmgr PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drmgr_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drmgr_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drx PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drx PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drx_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drx_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drwrap PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drwrap PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drwrap_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drwrap_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drreg PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drreg PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drreg_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drreg_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drbbdup PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drbbdup PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drbbdup_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drbbdup_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drsyms PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drsyms PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drsyms_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drsyms_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drutil PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drutil PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drutil_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drutil_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drcovlib PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drcovlib PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)

SET_PROPERTY(TARGET drcovlib_static PROPERTY MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
SET_PROPERTY(TARGET drcovlib_static PROPERTY MAP_IMPORTED_CONFIG_RELMINSIZE RelWithDebInfo)
")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/DynamoRIOTarget64.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/DynamoRIOTarget64.cmake"
         "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/CMakeFiles/Export/cmake/DynamoRIOTarget64.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/DynamoRIOTarget64-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/DynamoRIOTarget64.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/CMakeFiles/Export/cmake/DynamoRIOTarget64.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/CMakeFiles/Export/cmake/DynamoRIOTarget64-relwithdebinfo.cmake")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/core/cmake_install.cmake")
  include("C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/libutil/cmake_install.cmake")
  include("C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/tools/cmake_install.cmake")
  include("C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/clients/cmake_install.cmake")
  include("C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/ext/cmake_install.cmake")
  include("C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/api/samples/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/orbp/source/repos/rdpfuzz-dynamorio/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
