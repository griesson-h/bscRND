cmake_minimum_required(VERSION 3.15)
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)
project(bscRND VERSION 1.0
        DESCRIPTION "a vulkan-based game engine"
        LANGUAGES CXX)
add_executable(bscRND src/vulkan.cc src/swapchainpm.h)
target_include_directories(bscRND PRIVATE $(CMAKE_CURRENT_SOURCE_DIR)include)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
target_link_libraries(bscRND ${Vulkan_LIBRARIES} glfw)
