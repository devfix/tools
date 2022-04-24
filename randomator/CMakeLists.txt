cmake_minimum_required(VERSION 3.14)


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
# project setup
#-----------------------------------------------------------------
project(randomator)

set(CMAKE_CXX_STANDARD 20)


#-----------------------------------------------------------------
# add library boost
#-----------------------------------------------------------------
set(Boost_USE_STATIC_LIBS OFF)  # sad
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)  # sad
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})


#-----------------------------------------------------------------
# create target
#-----------------------------------------------------------------

add_executable(randomator main.cpp)
target_link_libraries(randomator PRIVATE Boost::program_options ${Boost_LIBRARIES})
install(TARGETS randomator DESTINATION /usr/bin)
