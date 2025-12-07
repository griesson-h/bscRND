cmake_minimum_required(VERSION 3.16)

project(MyProject)

include(FetchContent)
find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)

# --- GLFW ---
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

AUX_SOURCE_DIRECTORY(src/core CORE)

add_executable(${PROJECT_NAME} ${CORE})

target_include_directories(MyProject PRIVATE ${Vulkan_INCLUDE_DIRS} include)
target_link_libraries(MyProject PRIVATE ${Vulkan_LIBRARIES} ${X11_LIBRARIES} glfw)
