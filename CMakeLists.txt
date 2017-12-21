cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(SBD_MAJOR "0")
set(SBD_MINOR "0")
set(SBD_PATCH "1")
set(SBD_VERSION "${SBD_MAJOR}.${SBD_MINOR}.${SBD_PATCH}")

project(switchboard VERSION "${SBD_VERSION}" LANGUAGES CXX)

add_subdirectory(src)
