cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(3DFPSgame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_executable(3DFPSgame 
    "main.cpp" 
    "Camera.cpp" 
    "Model.cpp" 
    "Physics.cpp" 
    "PlayerController.cpp" 
    "Gun.cpp" "Shader.cpp")

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Jolt CONFIG REQUIRED)


target_link_libraries(3DFPSgame PRIVATE glfw)
target_link_libraries(3DFPSgame PRIVATE glm::glm)
target_link_libraries(3DFPSgame PRIVATE assimp::assimp)
target_link_libraries(3DFPSgame PRIVATE glad::glad)
target_link_libraries(3DFPSgame PRIVATE Jolt::Jolt)

target_include_directories(3DFPSgame PRIVATE ${Stb_INCLUDE_DIR})

target_compile_definitions(3DFPSgame
    PRIVATE
        $<$<CONFIG:Debug>:JPH_PROFILE_ENABLED=0>
        $<$<CONFIG:Release>:JPH_DISABLE_PROFILING>
)

target_compile_definitions(3DFPSgame
    PRIVATE
    $<$<CONFIG:Debug>:JPH_DEBUG_RENDERER>
)



set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "3DFPSgame")
set_property(TARGET 3DFPSgame PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set_property(TARGET 3DFPSgame PROPERTY INTERPROCEDURAL_OPTIMIZATION_DISTRIBUTION TRUE)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Distribution" CACHE STRING "" FORCE)

if(MSVC)
    target_compile_options(3DFPSgame PRIVATE /GR- /EHs-)
else()
    target_compile_options(3DFPSgame PRIVATE -fno-rtti -fno-exceptions)
endif()

message(STATUS "Jolt include dir: ${Jolt_INCLUDE_DIRS}")
message(STATUS "Jolt libraries: ${Jolt_LIBRARIES}")