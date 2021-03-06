cmake_minimum_required(VERSION 3.0)
project(StateTests)

enable_testing()
find_package(Qt5 5.3 REQUIRED COMPONENTS Core Test)

function(addStateTest TESTNAME TESTSRCS)
    add_executable(State_${TESTNAME} ${TESTSRCS})
    setup_iscore_common_test_features(State_${TESTNAME})
    target_link_libraries(State_${TESTNAME} PRIVATE Qt5::Core Qt5::Test iscore_lib_base iscore_lib_state)
    add_test(State_${TESTNAME}_target State_${TESTNAME})
endFunction()


# Commands
addStateTest(SerializationTest
             "${CMAKE_CURRENT_SOURCE_DIR}/SerializationTest.cpp")

addStateTest(EqualityTest
             "${CMAKE_CURRENT_SOURCE_DIR}/EqualityTest.cpp")
addStateTest(ExpressionTest
             "${CMAKE_CURRENT_SOURCE_DIR}/ExpressionParsingTests.cpp")
addStateTest(UnitTest
             "${CMAKE_CURRENT_SOURCE_DIR}/UnitTests.cpp")
