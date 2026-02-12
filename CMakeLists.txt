cmake_minimum_required(VERSION 3.16)
project(llu)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif ()

find_package(Eigen3 CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(llu INTERFACE)
add_library(llu::llu ALIAS llu)
target_include_directories(llu INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(llu INTERFACE Eigen3::Eigen yaml-cpp fmt::fmt)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  option(LLU_BUILD_TESTS "Build tests" ON)
else ()
  option(LLU_BUILD_TESTS "Build tests" OFF)
endif ()

if (LLU_BUILD_TESTS)
  enable_testing()
  find_package(GTest REQUIRED)
  function(llu_add_test name)
    add_executable(llu_${name} llu_tests/${name}.cpp)
    target_link_libraries(llu_${name} llu GTest::gtest GTest::gtest_main)
    add_test(NAME llu_${name} COMMAND llu_${name})
  endfunction()

  llu_add_test(chrono_test)
  llu_add_test(eigen_test)
  llu_add_test(env_test)
  llu_add_test(error_test)
  llu_add_test(geometry_test)
  llu_add_test(macro_test)
  llu_add_test(math_test)
  llu_add_test(range_test)
  llu_add_test(ring_test)
  llu_add_test(typename_test)
  llu_add_test(yaml_test)
endif ()

install(TARGETS llu EXPORT lluTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT lluTargets
    FILE lluTargets.cmake
    NAMESPACE llu::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/llu
)
install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/llu
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/lluConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lluConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/llu"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lluConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/llu"
)
