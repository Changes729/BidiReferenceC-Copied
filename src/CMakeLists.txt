include_directories(.)
include_directories(./include)

file(GLOB EXAMPLES_C "*.c")
file(GLOB BIDI_SRC "shared/*.c")

# generate exec obj ############################################################
foreach(EXAMPLE_FILE ${EXAMPLES_C})
	get_filename_component(EXAMPLE ${EXAMPLE_FILE} NAME_WE)
  message("Build " ${EXAMPLE})
	set (SOURCE_FILES ${EXAMPLE_FILE})
	add_executable(${EXAMPLE} ${SOURCE_FILES} ${BIDI_SRC})
endforeach(EXAMPLE_FILE)