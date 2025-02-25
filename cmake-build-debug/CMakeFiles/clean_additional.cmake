# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "Babble_autogen"
  "CMakeFiles\\Babble_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Babble_autogen.dir\\ParseCache.txt"
  )
endif()
