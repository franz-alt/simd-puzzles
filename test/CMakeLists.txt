set(target tests)

message(STATUS "Test application '${target}'")

set(sources
    main.cpp
    sum.cpp
)

add_executable(${target} ${sources})

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
