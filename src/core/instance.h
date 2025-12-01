#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

namespace bscRND {
    extern VkInstance instance;
    void createInstance();
}
