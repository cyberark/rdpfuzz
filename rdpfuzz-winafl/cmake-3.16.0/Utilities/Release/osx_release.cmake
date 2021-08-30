set(PROCESSORS 4)
set(CMAKE_RELEASE_DIRECTORY /Users/kitware/CMakeReleaseDirectory)
set(BOOTSTRAP_ARGS "--prefix=/ --docdir=doc/cmake")
set(HOST dragnipur)
set(MAKE_PROGRAM "make")
set(MAKE "${MAKE_PROGRAM} -j5")
set(CPACK_BINARY_GENERATORS "DragNDrop TGZ")
set(CPACK_SOURCE_GENERATORS "")
set(CPACK_DMG_FORMAT "UDBZ") #build using bzip2 for smaller package size
set(CC clang)
set(CXX clang++)
set(CFLAGS   "")
set(CXXFLAGS "-stdlib=libc++")
set(INITIAL_CACHE "
CMAKE_BUILD_TYPE:STRING=Release
CMAKE_C_STANDARD:STRING=11
CMAKE_CXX_STANDARD:STRING=14
CMAKE_OSX_ARCHITECTURES:STRING=x86_64
CMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.7
CMAKE_SKIP_BOOTSTRAP_TEST:STRING=TRUE
CPACK_SYSTEM_NAME:STRING=Darwin-x86_64
BUILD_CursesDialog:BOOL=ON
BUILD_QtDialog:BOOL=TRUE
CMake_GUI_DISTRIBUTE_WITH_Qt_LGPL:STRING=3
CMake_INSTALL_DEPENDENCIES:BOOL=ON
CMAKE_SKIP_RPATH:BOOL=TRUE
CMake_TEST_NO_FindPackageModeMakefileTest:BOOL=TRUE
")
set(ENV [[
export CMAKE_PREFIX_PATH='/Users/kitware/SDKs/qt-5.6.2-clang-x64'
]])
set(SIGN "")
get_filename_component(path "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${path}/release_cmake.cmake)
