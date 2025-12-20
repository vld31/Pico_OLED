cmake_minimum_required(VERSION 3.13)

project(PicoOledUnitTests LANGUAGES C CXX)

enable_testing()

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    test_notification_handler.cpp
    test_qrcodegen.cpp
    NotificationHandler_host.cpp
    qrcodegen_mock.cpp
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/mocks
)

target_compile_definitions(unit_tests PRIVATE BUILD_TESTS)

target_link_libraries(unit_tests PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)
