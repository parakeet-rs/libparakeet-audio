cmake_minimum_required(VERSION 3.21)
project(parakeet_audio VERSION 0.1.1 LANGUAGES CXX)

find_program(CLANG_TIDY NAMES "clang-tidy")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PARAKEET_AUDIO_BUILD_TESTING "Build library tests" ON)

include(cmake/CPM-Loader.cmake)
include(cmake/git-info.cmake)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/version.cpp.in"
  "${PROJECT_BINARY_DIR}/src/version.cpp"
  @ONLY
)

file(GLOB_RECURSE SOURCES
  "src/*.h"
  "src/*.cpp"
  "${PROJECT_BINARY_DIR}/src/*.h"
  "${PROJECT_BINARY_DIR}/src/*.cpp"
)

# Add source to this project's executable.
add_library(parakeet_audio STATIC ${SOURCES})
add_library(parakeet::audio ALIAS parakeet_audio)
set_target_properties(parakeet_audio PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON EXPORT_COMPILE_COMMANDS ON)
# Clang-tidy is optional.
if(CLANG_TIDY)
  set_target_properties(parakeet_audio PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY}")
endif()

target_include_directories(parakeet_audio
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    src
    "${PROJECT_BINARY_DIR}/src"
)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/parakeet_audio)

install(TARGETS parakeet_audio
  EXPORT parakeet_audio-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
set_target_properties(parakeet_audio PROPERTIES EXPORT_NAME ParakeetAudio)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT parakeet_audio-targets
  FILE
    ParakeetAudioTargets.cmake
  NAMESPACE
    ParakeetAudio::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/parakeet_audio
)

# Tests!
if(PARAKEET_AUDIO_BUILD_TESTING)
  enable_testing()
  CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.12.1
    VERSION 1.12.1
    OPTIONS
    "INSTALL_GTEST OFF"
    "gtest_force_shared_crt ON"
  )

  file(GLOB_RECURSE TESTS_SOURCE src/*.test.cxx src/*.test.hh)
  add_executable(parakeet_audio_test ${TESTS_SOURCE})
  if(CLANG_TIDY)
    set_target_properties(parakeet_audio_test PROPERTIES CXX_CLANG_TIDY "true")
  endif()
  target_include_directories(parakeet_audio_test PRIVATE src include)
  set_target_properties(parakeet_audio_test PROPERTIES
      CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON EXPORT_COMPILE_COMMANDS ON)
  target_link_libraries(parakeet_audio_test
    GTest::gmock
    GTest::gtest
    GTest::gmock_main
    GTest::gtest_main
    ${PROJECT_NAME}
  )

  include(GoogleTest)
  gtest_discover_tests(parakeet_audio_test)
  include(CTest)
endif()
