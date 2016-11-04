cmake_minimum_required(VERSION 2.8.7)
set(PROJECT_NAME_STR arXivSearchEngine)
project (${PROJECT_NAME_STR})

#---------
#Boost
#---------

#Load boost
FIND_PACKAGE(Boost 1.50.0 COMPONENTS regex system filesystem REQUIRED) 
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found.")
endif()

#Boost flags
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

#---------
#Directories
#---------

find_package(Threads REQUIRED)

#C++11 flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Set dirs
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/${PROJECT_NAME_STR}/*.h)
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/external)
#Sources
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/${PROJECT_NAME_STR}/*.cpp")
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

#GTest
enable_testing()
add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
include_directories(${GTEST_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COMMON_INCLUDES})

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)

add_executable(${PROJECT_NAME_STR} ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME_STR} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_TEST_NAME} ${Boost_LIBRARIES})


if(NOT WIN32 OR MINGW)
    target_link_libraries(${PROJECT_TEST_NAME}
        ${GTEST_LIBS_DIR}/libgtest.a
        ${GTEST_LIBS_DIR}/libgtest_main.a
        )
else()
    target_link_libraries(${PROJECT_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
    target_link_libraries(${PROJECT_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
endif()

target_link_libraries(${PROJECT_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})

add_test(test1 ${PROJECT_TEST_NAME})
