cmake_minimum_required(VERSION 2.6)

#Miscellaneous flags for development
SET(CMAKE_VERBOSE_MAKEFILE TRUE)
SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_C_COMPILER "clang")
SET(CMAKE_CXX_COMPILER "clang++")
#SET(CMAKE_CXX_COMPILER "gcc")
#SET(CMAKE_CXX_COMPILER "c++-analyzer")

project(TRADER)
include_directories(${TRADER_SOURCE_DIR}/src)

MESSAGE( STATUS )
MESSAGE( STATUS "--------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS "${PROJECT_NAME}_DEPENDS = \"${${PROJECT_NAME}_DEPENDS}\"" )
MESSAGE( STATUS "BUILD_WITH = \"${BUILD_WITH}\"" )
MESSAGE( STATUS "INSTALL_DOC = ${INSTALL_DOC}" )
MESSAGE( STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}" )
MESSAGE( STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE} )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "--------------------------------------------------------------------" )
MESSAGE( STATUS )

#Set compiler and linker settings
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -march=native -fno-rtti -fno-exceptions -fomit-frame-pointer -DENGINE_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native -fno-rtti -fno-exceptions -fomit-frame-pointer -DNDEBUG")
add_definitions(-std=c++0x)
set(CMAKE_EXE_LINKER_FLAGS "-s -lpthread")
#set(CMAKE_EXE_LINKER_FLAGS "-lpthread")

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GXX_EXPERIMENTAL_CXX0X__ -Wall -Wextra -Wno-ignored-qualifiers -pedantic -pthread")
endif()

set(CLIENT
src/client_socket_base.cpp
src/client_socket.cpp
)

#OpenGL Link Libraries
#find_library(GL GL)
#find_library(GLU GLU)

#Targets
#add_executable(./testit ${TESTIT})
add_executable(./bin/client ${CLIENT})

#target_link_libraries(./uci rt profiler)
#target_link_libraries(./testit rt profiler)
