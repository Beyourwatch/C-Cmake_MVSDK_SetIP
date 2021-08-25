# C-Cmake_MVSDK_SetIP
use Cmake to compile mvsdk, SET IP with linux console

CMakeLit:
#MV already did .cmake
include($ENV{MVIMPACT_ACQUIRE_DIR}/mvIMPACT_AcquireConfig.cmake)

target_link_libraries(${TARGET} PRIVATE ${mvIMPACT_Acquire_LIBRARIES})
target_include_directories(${TARGET} PRIVATE ${mvIMPACT_Acquire_INCLUDE_DIRS})

******************
Operation:
cmake .
make 
./SetCameraIP

