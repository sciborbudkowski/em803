# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/em803_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/em803_autogen.dir/ParseCache.txt"
  "em803_autogen"
  )
endif()
