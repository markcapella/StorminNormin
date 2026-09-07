# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles/StorminNormin_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/StorminNormin_autogen.dir/ParseCache.txt"
  "StorminNormin_autogen"
  )
endif()
