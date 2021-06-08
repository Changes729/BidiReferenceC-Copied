set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/test)

set(SPIKE_DIR ../shared)

include_directories(.)
include_directories(${SPIKE_DIR})
include_directories(${SPIKE_DIR}/shared/lib)

file(GLOB EXAMPLES_C "./example/*.c" "./example/*.cpp")
file(GLOB LIB_SRC "${SPIKE_DIR}/shared/lib/*.c")

foreach(EXAMPLE_FILE ${EXAMPLES_C})
	get_filename_component(EXAMPLE ${EXAMPLE_FILE} NAME_WE)
	set (SOURCE_FILES ${EXAMPLE_FILE})
	add_executable(${EXAMPLE} "test-entry.cpp" ${SOURCE_FILES} ${LIB_SRC})
  target_link_libraries(${EXAMPLE} gtest)
endforeach(EXAMPLE_FILE)
