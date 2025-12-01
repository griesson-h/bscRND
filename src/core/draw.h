#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace bscRND {
    extern std::vector<VkFramebuffer> swapChainFramebuffers;
    extern VkCommandPool commandPool;
    extern VkCommandBuffer commandBuffer;
    extern VkSemaphore imageAvailableSem;
    extern VkSemaphore renderFinishedSem;
    extern VkFence inFlightFen;
    extern VkPhysicalDevice physicalDevice;
    extern VkPhysicalDeviceProperties deviceProperties;
    extern VkPhysicalDeviceFeatures deviceFeatures;

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void createFramebuffers();
    void createSyncObjects();
    void createCommandPool();
    void createCommandBuffer();
    void drawFrame();
}
