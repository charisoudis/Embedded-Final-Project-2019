include_directories(../lib/cmocka/include ../lib/cmocka/src)

add_executable(FinalTests utils.c)
target_link_libraries(FinalTests ${CMOCKA_LIBRARY})