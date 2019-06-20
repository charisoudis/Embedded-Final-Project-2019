include_directories(../lib/cmocka/include)

add_executable(FinalTests test_utils.h test_utils.c run.c)

target_link_libraries(FinalTests ${CMOCKA_LIBRARY} "-nostartfiles")