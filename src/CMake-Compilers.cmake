include_guard()

# Enable C++17 support:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For Windows, set up Visual Studio:
if(" ${CMAKE_CXX_COMPILER_ID}" STREQUAL " MSVC")
  # Statically link to the CRT, so the binary is self-contained.
  foreach(LANG C CXX)
    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
      string(TOUPPER "CMAKE_${LANG}_FLAGS_${CONFIG}" VAR)
      string(REGEX REPLACE "/MD" "/MT" ${VAR} "${${VAR}}")
    endforeach()
  endforeach()

  # Add /await to enable C++20 coroutines.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await"
      CACHE STRING "Set by CMake-Compiler.cmake" FORCE)
endif()

# For Linux or Windows with Clang, enable C++20 coroutines.
if(" ${CMAKE_CXX_COMPILER_ID}" STREQUAL " Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts -stdlib=libc++")
endif()
