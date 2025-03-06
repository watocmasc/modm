# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles/modm_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/modm_autogen.dir/ParseCache.txt"
  "modm_autogen"
  )
endif()
