cmake_minimum_required(VERSION 2.8)

#Miscellaneous flags for development
SET(CMAKE_VERBOSE_MAKEFILE FALSE)
SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_C_COMPILER "clang")
#SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
#SET(CMAKE_CXX_COMPILER "c++-analyzer")

project(TRADER)

FIND_PACKAGE(FLTK)
FIND_PACKAGE(OpenGL REQUIRED)

if(FLTK_FOUND)
  include_directories(${FLTK_INCLUDE_DIR})
else(FLTK_FOUND)
  message(FATAL_ERROR "Could not find FLTK")
  #include_directories("/local/include")
endif(FLTK_FOUND)

include_directories(${TRADER_SOURCE_DIR}/src)

# TWSAPI
if(MINGW)
  include_directories(/local/include/twsapi)
  set(TWSAPI_LIBRARIES "/local/lib/libtwsapi.a")
endif()

#Set compiler and linker settings
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -march=native -fno-rtti -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native -fno-rtti -fomit-frame-pointer -DNDEBUG")
add_definitions(-std=c++0x)
#set(CMAKE_EXE_LINKER_FLAGS "-s")

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIB_USE_STD_STRING -D__GXX_EXPERIMENTAL_CXX0X__ -Wfatal-errors -Wall -Wextra -Wno-ignored-qualifiers -pedantic -pthread")
endif()

set(CLIENT
  src/twsapi/PosixTestClient.cpp
  src/twsapi/EClientSocketBase.cpp
  src/twsapi/EPosixClientSocket.cpp
  src/cb/thread.cpp
  src/core/transactions.cpp
  src/core/client_thread.cpp
  src/core/router_thread.cpp
  src/core/bar_thread.cpp
  src/core/bot_thread.cpp
  src/gui/main.cpp
)

#Targets
add_executable(./bin/trader ${CLIENT})
target_link_libraries(./bin/trader ${FLTK_LIBRARIES} ${TWSAPI_LIBRARIES})

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
MESSAGE( STATUS "TRADER_SOURCE_DIR: " ${TRADER_SOURCE_DIR} )
MESSAGE( STATUS "FLTK_INCLUDE_DIR: " ${FLTK_INCLUDE_DIR} )
#MESSAGE( STATUS "FLTK_LIBRARIES: " ${FLTK_LIBRARIES} )
MESSAGE( STATUS "FLTK_BASE_LIBRARY: " ${FLTK_BASE_LIBRARY} )
MESSAGE( STATUS "FLTK_FORMS_LIBRARY: " ${FLTK_FORMS_LIBRARY} )
MESSAGE( STATUS "FLTK_GL_LIBRARY: " ${FLTK_GL_LIBRARY} )
MESSAGE( STATUS "FLTK_IMAGES_LIBRARY: " ${FLTK_IMAGES_LIBRARY} )
MESSAGE( STATUS "TWSAPI_LIBRARIES: " ${TWSAPI_LIBRARIES} )
MESSAGE( STATUS "--------------------------------------------------------------------" )
#MESSAGE( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )
MESSAGE( STATUS "--------------------------------------------------------------------" )
MESSAGE( STATUS )

