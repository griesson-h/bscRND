<<<<<<< HEAD
cmake_minimum_required(VERSION 3.15)
project(bscRND)
add_executable(bscRND vulkan.cc)
=======
cmake_minimum_required(VERSION 3.15...4.1)
project(bscRND VERSION 1.0
        DESCRIPTION "a vulkan-based game engine"
        LANGUAGES CXX)
add_executable(bscRND vulkan.cc)
add_library(libbscRND vulkan.cc)
>>>>>>> 7a6c156 (basic vulkan initializer)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
target_link_libraries(bscRND ${Vulkan_LIBRARIES} glfw)
