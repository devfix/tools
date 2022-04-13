cmake_minimum_required(VERSION 3.16)
Include(FetchContent)
project(lnxfn)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-psabi -Wno-implicit-fallthrough -pedantic -Wno-long-long")

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/lnxfn/*.hpp)
add_library(lnxfn STATIC ${HEADERS} ${SOURCES})
set_target_properties(lnxfn PROPERTIES CXX_VISIBILITY_PRESET hidden)
set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(lnxfn PUBLIC ${INCLUDE_DIRS})
set(lnxfn_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)


#-----------------------------------------------------------------
# add library torquis
#-----------------------------------------------------------------
FetchContent_Declare(
        torquis_targz
        URL https://github.com/devfix/torquis/archive/refs/tags/v0.3.1.tar.gz
        URL_HASH SHA256=a67fdfe0a8cbaaf3a67686e6c9dbbc81329570c7c4143ed9a58b3fe1c425049e
)
FetchContent_MakeAvailable(torquis_targz)
target_include_directories(lnxfn PRIVATE ${torquis_INCLUDE_DIRS})
