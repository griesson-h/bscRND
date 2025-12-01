#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "surface.h"
#include "instance.h"
#include "init.h"

namespace bscRND {
    VkSurfaceKHR surface;

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("Window surface creation failed :(");
        }
    }
}
