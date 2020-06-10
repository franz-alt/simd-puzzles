set(target tests)

message(STATUS "Test application '${target}'")

set(sources
    main.cpp
    sum.cpp
)

add_executable(${target} ${sources})

target_compile_options(${target}
	PRIVATE
		$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>
		$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-${OPTIMIZATION_LEVEL}>
)

target_link_libraries(${target}
    PRIVATE
        ${DEFAULT_LIBRARIES}
        gtest_main
        libsimd
)

set_target_properties(${target}
    PROPERTIES
        ${DEFAULT_PROJECT_OPTIONS}
        ${BUILDTYPE}_POSTFIX "${TARGET_PREFIX}"
)

add_test(NAME test_sum COMMAND tests)
