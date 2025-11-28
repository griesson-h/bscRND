cmake_minimum_required(VERSION 3.15)
project(bscRND)
add_executable(bscRND vulkan.cc swapchainpm.h)
project(bscRND VERSION 1.0
        DESCRIPTION "a vulkan-based game engine"
        LANGUAGES CXX)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
target_link_libraries(bscRND ${Vulkan_LIBRARIES} glfw)
