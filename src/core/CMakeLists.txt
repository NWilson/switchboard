project(core)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SRCS
  impl/CoarseClock.cpp
  impl/ExecutionContext.cpp
  impl/Executor.cpp
  impl/IoContext.cpp
  impl/Scheduler.cpp
  )

file(GLOB HDRS include/core/*.h impl/*.h detail/*.h)

#add_library(core STATIC ${SRCS})
add_executable(core ${SRCS} ${HDRS} test.cpp)
#target_link_libraries(coreTest core)
