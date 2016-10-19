cmake_minimum_required(VERSION 2.8.7)
project (SimpleEngine)

#Bring the headers into the project
include_directories(SimpleEngine/*.h)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "SimpleEngine/*.cpp")

#C++11 flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Boost flags
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

FIND_PACKAGE(Boost 1.50.0 COMPONENTS regex system filesystem chrono REQUIRED) 
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	add_executable(SimpleEngine ${SOURCES})
	TARGET_LINK_LIBRARIES(SimpleEngine ${Boost_LIBRARIES})
endif()
