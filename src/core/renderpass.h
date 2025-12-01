#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bscRND {
    extern VkRenderPass renderPass;

    void createRenderPass();
}
