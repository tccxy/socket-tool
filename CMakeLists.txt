# cmake for socket-tool
cmake_minimum_required(VERSION 3.5.1)
project(socket-tool)
# name
set(PROJECT_NAME socket-tool)

add_definitions(-DMAJOR_VER=1)
add_definitions(-DMINOR_VER=0)


option(ARM32 "option for ARM32" OFF)

# set cmake cross compile
#set(CMAKE_CROSEECOMPILING  TRUE)
if(ARM32)
    add_definitions(-DCPU_WORD_SIZE=32)
    set(CMKA_SYSTRM_NAME       Linux)
    set(CMAKE_C_COMPILER       arm-linux-gnueabihf-gcc)
    #set(CMAKE_CXX_COMPILER     arm-linux-gnueabihf-g++)
    set(LIB lib-32)
else()
    add_definitions(-DCPU_WORD_SIZE=64)
    set(CMKA_SYSTRM_NAME       Linux)
    set(CMAKE_C_COMPILER       aarch64-linux-gnu-gcc)
    #set(CMAKE_CXX_COMPILER     aarch64-linux-gnu-g++)
    set(LIB lib)
endif(ARM32)

# add define value
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -rdynamic -O2 -Wall -Wno-pointer-to-int-cast\
                            -Wno-incompatible-pointer-types -Wno-pointer-sign\
                            -Wno-missing-braces -Wno-format")

# add message
message(STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR})
message(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})
#message(SEND_ERROR "this is a SEND_ERROR")
#message(FATAL_ERROR "this is a FATAL_ERROR")

# add src
aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_SRC)

# add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# set lib dir
set(zlog_install ${PROJECT_SOURCE_DIR}/3rd_opensrc/zlog_install/${LIB})

link_directories(${zlog_install})

set(3rd_lib zlog pthread m crypt dl rt)

# set lib include
set(zlog_include ${PROJECT_SOURCE_DIR}/3rd_opensrc/zlog_install/include)

include_directories(${zlog_include})

#add_definitions(-Dxxx)

add_executable(${PROJECT_NAME} ${DIR_SRC})
target_link_libraries(${PROJECT_NAME} ${3rd_lib})

execute_process(COMMAND mkdir -p ${PROJECT_BINARY_DIR}/release)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/release)
file(COPY ${zlog_install}/libzlog.so.1.1 DESTINATION ${EXECUTABLE_OUTPUT_PATH})