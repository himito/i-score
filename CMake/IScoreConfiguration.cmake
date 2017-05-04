include(CheckCXXCompilerFlag)

# Options
CHECK_CXX_COMPILER_FLAG("-Wl,-z,defs" WL_ZDEFS_SUPPORTED)
CHECK_CXX_COMPILER_FLAG("-fuse-ld=gold" GOLD_LINKER_SUPPORTED)


option(ISCORE_ENABLE_LTO "Enable link-time optimization. Won't work on Travis." OFF)
option(ISCORE_ENABLE_OPTIMIZE_CUSTOM "Enable -march=native." OFF)

option(OSSIA_NO_EXAMPLES "Don't build OSSIA examples" True)
option(OSSIA_NO_TESTS "Don't build OSSIA tests" True)

option(ISCORE_COTIRE "Use cotire. Will make the build faster." OFF)
option(ISCORE_COTIRE_ALL_HEADERS "All headers will be put in prefix headers. Faster for CI but slower for development" OFF)

option(ISCORE_STATIC_QT "Try to link with a static Qt" OFF)
option(ISCORE_STATIC_EVERYTHING "Try to link with everything static" OFF)
option(ISCORE_USE_DEV_PLUGINS "Build the prototypal plugins" OFF)
option(ISCORE_SANITIZE "Build with sanitizers and debug glibc" OFF)
option(INTEGRATION_TESTING "Run integration tests" OFF)

option(ISCORE_BUILD_FOR_PACKAGE_MANAGER "Set FHS-friendly install paths" OFF)

option(ISCORE_OPENGL "Use OpenGL for rendering" OFF)
option(ISCORE_IEEE "Use a graphical skin adapted to publication" OFF)
option(ISCORE_WEBSOCKETS "Run a websocket server in the scenario" OFF)
option(ISCORE_TESTBED "Enable the testbed. See Tests/testbed/README" OFF)
option(ISCORE_PLAYER "Build standalone player" OFF)
option(DEFINE_ISCORE_SCENARIO_DEBUG_RECTS "Enable to have debug rects around elements of a scenario" OFF)

option(ISCORE_SPLIT_DEBUG "Split debug information" ON)
option(ISCORE_COVERAGE "Enable coverage" OFF)

include("${ISCORE_CONFIGURATION}")

set(CMAKE_DEBUG_POSTFIX "")
if(APPLE)
    set(ISCORE_ADDON_PLATFORM "darwin-amd64")
    set(ISCORE_ADDON_SUFFIX "amd64.dylib")
    set(ISCORE_OPENGL ON)
elseif(WIN32)
    set(ISCORE_ADDON_PLATFORM "windows-x86")
    set(ISCORE_ADDON_SUFFIX "x86.dll")
elseif(UNIX)
    set(ISCORE_ADDON_PLATFORM "linux-amd64")
    set(ISCORE_ADDON_SUFFIX "amd64.so")
endif()

if(ISCORE_STATIC_EVERYTHING)
  set(ISCORE_STATIC_QT True)
  if(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
  endif()
endif()

if(NACL)
  set(ISCORE_STATIC_QT True)
endif()

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050800)
if(ISCORE_STATIC_QT)
  if(UNIX AND NOT APPLE)
    set(ISCORE_STATIC_PLUGINS True)
    add_definitions(-DQT_STATIC)
    add_definitions(-DISCORE_STATIC_QT)
  endif()
endif()

if(ANDROID)
  set(ISCORE_STATIC_PLUGINS True)
  set(Boost_FOUND True)
  include_directories("/opt/android-toolchain/arm-linux-androideabi/include")
else()
  find_package(Boost REQUIRED)
  include_directories(SYSTEM "${Boost_INCLUDE_DIRS}")
endif()

if(UNIX AND NOT APPLE AND DEPLOYMENT_BUILD)
  set(ISCORE_BUILD_FOR_PACKAGE_MANAGER ON)
endif()

if(INTEGRATION_TESTING)
  set(ISCORE_STATIC_PLUGINS True)
endif()

if(ISCORE_STATIC_PLUGINS)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

if(ISCORE_COVERAGE)
  include("${CMAKE_CURRENT_LIST_DIR}/modules/CodeCoverage.cmake")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
  set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_COVERAGE}")
  set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_COVERAGE}")
endif()

# Note : if building with a Qt installed in e.g. /home/myuser/Qt/ or /Users/Qt or c:\Qt\
# keep in mind that you have to call CMake with :
# $ cmake -DCMAKE_MODULE_PATH={path/to/qt/5.3}/{gcc64,clang,msvc2013...}/lib/cmake/Qt5

# Settings
include(ProcessorCount)
include(GenerateExportHeader)

if(UNIX AND NOT APPLE AND NOT ISCORE_STATIC_PLUGINS AND DEPLOYMENT_BUILD)
  set(CMAKE_INSTALL_RPATH "plugins")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)
set(ISCORE_ROOT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CTEST_OUTPUT_ON_FAILURE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_ANDROID_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Android")
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CXX_IS_CLANG True)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(CXX_IS_MSVC True)
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} runtimeobject.lib")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CXX_IS_GCC True)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                  OUTPUT_VARIABLE GCC_VERSION)

  if (GCC_VERSION VERSION_LESS 5.1)
    message(FATAL_ERROR "i-score requires at least g++-5.1 to build. ")
  endif()
endif()

if(ISCORE_ENABLE_LTO)
  setup_lto()
endif()

# Useful header files
include(WriteCompilerDetectionHeader)
write_compiler_detection_header(
  FILE iscore_compiler_detection.hpp
  PREFIX ISCORE
  COMPILERS GNU Clang AppleClang MSVC
  FEATURES cxx_relaxed_constexpr
  VERSION 3.1
)

# Commit and version information
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  include(GetGitRevisionDescription)
  get_git_head_revision(GIT_COMMIT_REFSPEC GIT_COMMIT_HASH)
else()
  set(GIT_COMMIT_HASH "")
endif()
iscore_write_file("${CMAKE_CURRENT_BINARY_DIR}/iscore_git_info.hpp"
"#pragma once
#define GIT_COMMIT \"${GIT_COMMIT_HASH}\"
#define ISCORE_VERSION_MAJOR ${ISCORE_VERSION_MAJOR}
#define ISCORE_VERSION_MINOR ${ISCORE_VERSION_MINOR}
#define ISCORE_VERSION_PATCH ${ISCORE_VERSION_PATCH}
#define ISCORE_VERSION_EXTRA \"${ISCORE_VERSION_EXTRA}\"
#define ISCORE_CODENAME \"${ISCORE_CODENAME}\"
")

set(COTIRE_UNITY_SOURCE_MAXIMUM_NUMBER_OF_INCLUDES "-j")
include(cotire)
