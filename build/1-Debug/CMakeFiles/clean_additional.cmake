# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/control_conditioning_system_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/control_conditioning_system_autogen.dir/ParseCache.txt"
  "control_conditioning_system_autogen"
  )
endif()
