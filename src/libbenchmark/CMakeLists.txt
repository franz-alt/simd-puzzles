set(target libbenchmark)

message(STATUS "Library '${target}'")

set(headers
    result.hpp
    testrun.hpp
    testsuite.hpp
)

set(sources
    result.cpp
    testrun.cpp
    testsuite.cpp
)

add_library(${target} STATIC ${sources} ${headers})

target_include_directories(${target}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_compile_definitions(${target}
	PUBLIC
		${DEFAULT_COMPILE_DEFINITIONS}
)

set_target_properties(${target}
	PROPERTIES
		${DEFAULT_PROJECT_OPTIONS}
        PREFIX ""
        ${BUILDTYPE}_POSTFIX "${TARGET_PREFIX}"
)
