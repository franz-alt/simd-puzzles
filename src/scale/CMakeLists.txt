set(target bench_scale)

message(STATUS "Application '${target}'")

set(sources
    main.cpp
)

add_executable(${target} ${sources} ${headers})

target_compile_options(${target}
	PRIVATE
		$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>
		$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-${OPTIMIZATION_LEVEL}>
)

if(ENABLE_SIMD_AVX2)
    target_compile_options(${target} PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-mavx2>)
    target_compile_options(${target} PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-mfma>)
endif()

target_link_libraries(${target}
    PRIVATE
        ${DEFAULT_LIBRARIES}
        ${Boost_LIBRARIES}
        libbenchmark
        libsimd
)

set_target_properties(${target}
    PROPERTIES
        ${DEFAULT_PROJECT_OPTIONS}
        ${BUILDTYPE}_POSTFIX "${TARGET_PREFIX}"
)
