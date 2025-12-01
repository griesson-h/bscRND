cmake_minimum_required(VERSION 3.15)
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)
project(bscRND VERSION 1.0
        DESCRIPTION "a vulkan-based game engine"
        LANGUAGES CXX)

include(FetchContent)

# --- GLFW ---
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

add_executable(bscRND src/legacy/vulkan.cc src/legacy/swapchainpm.h src/legacy/vulkan.h src/stb/stb_image.cpp)
target_include_directories(bscRND PRIVATE $(CMAKE_CURRENT_SOURCE_DIR)include)
find_package(Vulkan REQUIRED)
target_link_libraries(bscRND ${Vulkan_LIBRARIES} glfw)
