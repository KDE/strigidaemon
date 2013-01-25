include(CheckFunctionExists)

# Not all systems have mkdtemp, and some of the tests use mkdtemp
# -- pulling in an implementation from kdefakes is an annoying workaround,
# so instead just don't do the tests here.
check_function_exists(mkdtemp HAVE_MKDTEMP)

set (TESTINGDIRS utils indextesters)
# streamanalyzer

include_directories (. ../bin/daemon ${CPPUNIT_INCLUDE_DIR})

if(BUILD_DAEMON)
    add_subdirectory(daemon)
endif()

if(NOT HAVE_MKDTEMP)
    message(WARNING " No mkdtemp found; some Strigi tests are suppressed.")
    set(TESTINGDIRS utils daemon)
endif()

add_subdirectory(${TESTINGDIRS})

# setting for using CPPUNIT

include_directories( ${strigi_SOURCE_DIR}/src/daemon
    ${strigi_BINARY_DIR}/src/streams)

# a convenience library that adds a unified way of build a test executable
# for testing
# simply link this into any unit test executable
add_library(strigi_test_runner STATIC test_runner.cpp)
target_link_libraries(strigi_test_runner ${CPPUNIT_LIBRARIES})

enable_testing()
