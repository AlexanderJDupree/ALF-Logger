file(GLOB_RECURSE libalf_sources *)

# TODO add option to build Static or Shared library
add_library(libalf STATIC ${libalf_sources})

set_target_properties(
    PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Wno-unused-function"
)
