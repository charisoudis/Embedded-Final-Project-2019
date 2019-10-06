# CMake generated Testfile for 
# Source directory: /home/achariso/Desktop/Embedded Systems/Projects/Final/tests/lib/cmocka/example
# Build directory: /home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/example
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(simple_test "simple_test")
add_test(allocate_module_test "allocate_module_test")
set_tests_properties(allocate_module_test PROPERTIES  WILL_FAIL "1")
add_test(assert_macro_test "assert_macro_test")
set_tests_properties(assert_macro_test PROPERTIES  WILL_FAIL "1")
add_test(assert_module_test "assert_module_test")
set_tests_properties(assert_module_test PROPERTIES  WILL_FAIL "1")
subdirs("mock")
