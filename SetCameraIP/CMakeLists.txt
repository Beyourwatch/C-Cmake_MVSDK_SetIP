cmake_minimum_required(VERSION 3.10) 

# 设定工程名和版本号 
project(SetCameraIP VERSION 1.0) 


# specify the C++ standard 
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
set(TARGET SetCameraIP)

include($ENV{MVIMPACT_ACQUIRE_DIR}/mvIMPACT_AcquireConfig.cmake)

# add the executable 
add_executable(SetCameraIP SetIP.cpp) 

target_link_libraries(${TARGET} PRIVATE ${mvIMPACT_Acquire_LIBRARIES})
target_include_directories(${TARGET} PRIVATE ${mvIMPACT_Acquire_INCLUDE_DIRS})
