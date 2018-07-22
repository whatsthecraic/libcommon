file(GLOB SOURCES *.cpp)
add_executable(test_driver ${SOURCES})
target_link_libraries(test_driver libcommon gtest_main)
add_test(NAME test_driver COMMAND test_driver)