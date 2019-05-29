include(addapp)
option(HH_TEST "Generate HH tests" ON)
if (HH_TEST)
	add_executable(HH-tests
		test-hh-errors.cpp
		test-hh-files.cpp
		test-hh-funcs.cpp
		test-hh-types.cpp
		IntegratedTesting.hpp IntegratedTesting.cpp)
	addapp_test(HH-tests Testing/HH)
	target_link_libraries(HH-tests HH Boost::unit_test_framework)
	add_test (NAME HH-tests
		COMMAND HH-tests
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)

endif()
