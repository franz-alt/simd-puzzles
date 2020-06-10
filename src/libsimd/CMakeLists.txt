set(target libsimd)

message(STATUS "Library '${target}'")

set(headers
	algorithms/sum/naive.hpp
	algorithms/sum/avx2/hadd.hpp
	algorithms/sum/avx2/sad.hpp
	utils/to_string.hpp
)

set(sources
	algorithms/sum/naive.cpp
	algorithms/sum/avx2/hadd.cpp
	algorithms/sum/avx2/sad.cpp
	utils/to_string.cpp
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

target_compile_options(${target}
	PRIVATE
		$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>
)

if(ENABLE_SIMD_AVX2)
    target_compile_options(${target} PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-mavx2>)
    target_compile_options(${target} PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-mfma>)
endif()

set_target_properties(${target}
	PROPERTIES
		${DEFAULT_PROJECT_OPTIONS}
		PREFIX ""
		${BUILDTYPE}_POSTFIX "${TARGET_PREFIX}"
)
