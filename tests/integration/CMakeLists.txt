find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SOTC_INTEGRATION_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SOTC_INTEGRATION_TEST_SCRIPT ${SOTC_INTEGRATION_TEST_DIR}/test_launch_options_dump.py)

add_test(
    NAME integration.launch_options_dump
    COMMAND ${Python3_EXECUTABLE} ${SOTC_INTEGRATION_TEST_SCRIPT}
            --binary $<TARGET_FILE:sotc>
)

set_tests_properties(
    integration.launch_options_dump
    PROPERTIES
        LABELS "integration"
)
