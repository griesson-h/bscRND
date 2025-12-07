#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace bscRND {
    extern std::vector<VkFramebuffer> swapChainFramebuffers;
    extern const int MAX_FRAMES_IN_FLIGHT;
    extern VkCommandPool commandPool;
    extern std::vector<VkCommandBuffer> commandBuffers;
    extern std::vector<VkSemaphore> imageAvailableSems;
    extern std::vector<VkSemaphore> renderFinishedSems;
    extern std::vector<VkFence> inFlightFens;
    extern bool framebufferResized;
    extern VkPhysicalDevice physicalDevice;
    extern VkPhysicalDeviceProperties deviceProperties;
    extern VkPhysicalDeviceFeatures deviceFeatures;

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void createFramebuffers();
    void createSyncObjects();
    void createCommandPool();
    void createCommandBuffers();
    void framebufferResizeCallBack(GLFWwindow* window, int width, int height);
    void drawFrame();
}
