cmake_minimum_required(VERSION 2.8.8)
set(PROJECT_NAME_STR arXivSearchEngine)
project (${PROJECT_NAME_STR})
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

option(COVERALLS "Turn on coveralls support" OFF)
option(COVERALLS_UPLOAD "Upload the generated coveralls json" ON)

#---------
#Boost
#---------
#file(GLOB SOURCES "arXivSearchEngine/*.cpp")
#list(REMOVE_ITEM SOURCES "arXivSearchEngine/wt.cpp")

set(SOURCES
    ${SRC_DIR}/Compressor.cpp
    ${SRC_DIR}/DiskInvertedIndex.cpp
    ${SRC_DIR}/DocInfo.cpp
    ${SRC_DIR}/Engine.cpp
    ${SRC_DIR}/InvertedIndex.cpp
    ${SRC_DIR}/KDeserializer.cpp
    ${SRC_DIR}/KEngine.cpp
    ${SRC_DIR}/KgramIndex.cpp
    ${SRC_DIR}/KSerializer.cpp
    ${SRC_DIR}/PorterStemmer.cpp
    ${SRC_DIR}/QEngine.cpp
    ${SRC_DIR}/Serializer.cpp
    ${SRC_DIR}/Tokenizer.cpp
    ${SRC_DIR}/WildEngine.cpp
    ${SRC_DIR}/ClassifierEngine.cpp
    ${SRC_DIR}/ClassifierClass.cpp
    ${SRC_DIR}/DiskReader.cpp
    ${SRC_DIR}/DiskWriter.cpp
)

#Load boost
if(WIN32)
	add_definitions( -DBOOST_ALL_NO_LIB ) #Disable boost autolinking
endif()
FIND_PACKAGE(Boost 1.55.0 COMPONENTS regex system filesystem thread signals REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found.")
endif()

#Boost flags
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

#C++11 flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0") # debug, no optimisation
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage") # enabling coverage

find_package(Threads REQUIRED)

#---------
#Coveralls
#---------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
	${PROJECT_SOURCE_DIR}/external/coveralls-cmake)

if (COVERALLS)
	include(Coveralls)
	coveralls_turn_on_coverage()
endif()

if (COVERALLS)
	coveralls_setup(
        "${SOURCES}" 
        "${COVERALLS_UPLOAD}"                 
        "${PROJECT_SOURCE_DIR}/external/coveralls-cmake")
endif()


#---------
#Directories
#---------
#Set dirs
set(COMMON_INCLUDES ${INCLUDE_DIR}/*.h)
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/external)
#Sources
#file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/${PROJECT_NAME_STR}/*.cpp")
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

#GTest
enable_testing()
add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
include_directories(${GTEST_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COMMON_INCLUDES} ${INCLUDE_DIR})

#Build
add_executable(${PROJECT_NAME_STR} ${SRC_DIR}/ConsoleEngine.cpp ${SOURCES})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME_STR} ${Boost_LIBRARIES})

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${SOURCES})
add_dependencies(${PROJECT_TEST_NAME} googletest)
TARGET_LINK_LIBRARIES(${PROJECT_TEST_NAME} ${Boost_LIBRARIES})


#Wt
find_library(wt wt)
find_library(wthttp wthttp)
if(wt)
  message("-- Wt found")
  add_executable(${PROJECT_NAME_STR}Web ${SRC_DIR}/Wt.cpp ${SOURCES})
  target_link_libraries(arXivSearchEngineWeb
                        ${Boost_LIBRARIES}
                        wt wthttp)
endif()

#------
#GTest
#------
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
