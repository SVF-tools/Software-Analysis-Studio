set(SOURCES
        Z3Examples.cpp
        Z3Mgr.cpp
        test.cpp
)

add_executable(lab-z3 ${SOURCES})
target_link_libraries(lab-z3 ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})

set_target_properties(lab-z3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

foreach(i RANGE 0 10)
    add_test(
            NAME lab-z3-test${i}
            COMMAND lab-z3 test${i}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()
