include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runFinalTests UtilsTest.cpp ServerTest.cpp)

target_link_libraries(runFinalTests gtest gtest_main)
target_link_libraries(runFinalTests FINAL_LIB pthread)