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
        pugixml_git
        GIT_REPOSITORY https://github.com/zeux/pugixml.git
        GIT_TAG v1.12.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(pugixml_git)

# add library to target
target_include_directories(${TARGET} PRIVATE ${pugixml_git_SOURCE_DIR}/src)
target_link_libraries(${TARGET} PRIVATE pugixml::pugixml)


#-----------------------------------------------------------------
# add library boost
#-----------------------------------------------------------------
message(STATUS "Loading boost")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost COMPONENTS filesystem system REQUIRED)
target_include_directories(${TARGET} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${TARGET} PRIVATE ${Boost_LIBRARIES})
