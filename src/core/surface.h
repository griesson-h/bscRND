#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bscRND {
    extern VkSurfaceKHR surface;
    void createSurface();
}
