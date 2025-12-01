#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>

namespace bscRND {
    extern VkQueue graphicsQueue;
    extern VkQueue presentQueue;
    extern VkPhysicalDevice physicalDevice;
    extern VkPhysicalDeviceProperties deviceProperties;
    extern VkPhysicalDeviceFeatures deviceFeatures;
    extern const std::vector<const char*> deviceExtensions;
    extern VkDevice device;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void IsDeviceSuitable();
    void createLogicalDevice();
}
