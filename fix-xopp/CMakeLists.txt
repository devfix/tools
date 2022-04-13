cmake_minimum_required(VERSION 3.20)


#-----------------------------------------------------------------
# use ccache (optional)
#-----------------------------------------------------------------
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
else ()
    message(STATUS "ccache not found")
endif ()


#-----------------------------------------------------------------
# general setups
#-----------------------------------------------------------------
project(fix_xopp)
include(FetchContent)
include(ExternalProject)
set(FETCHCONTENT_QUIET false)
set(CMAKE_CXX_STANDARD 23)
set(TARGET fix-xopp)


#-----------------------------------------------------------------
# create target
#-----------------------------------------------------------------
add_executable(${TARGET} main.cpp)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set_target_properties(${TARGET} PROPERTIES LINK_SEARCH_START_STATIC 1)


#-----------------------------------------------------------------
# add library pugixml
#-----------------------------------------------------------------
message(STATUS "Loading pugixml")
FetchContent_Declare(
        pugixml_targz
        URL https://github.com/zeux/pugixml/archive/refs/tags/v1.12.1.tar.gz
        URL_HASH SHA256=1e28ab24b6e04e013d96f45d25e9f2d04c921dc68c613fd010ecaaad3892c14d
)
FetchContent_MakeAvailable(pugixml_targz)

# add library to target
target_include_directories(${TARGET} PRIVATE ${pugixml_targz_SOURCE_DIR}/src)
target_link_libraries(${TARGET} PRIVATE pugixml::pugixml)


#-----------------------------------------------------------------
# add library boost
#-----------------------------------------------------------------
message(STATUS "Loading boost")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost COMPONENTS filesystem system context coroutine unit_test_framework REQUIRED)
target_include_directories(${TARGET} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${TARGET} PRIVATE ${Boost_LIBRARIES} Boost::filesystem)