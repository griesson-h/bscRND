#pragma once

#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bscRND {
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

extern const std::vector<const char *> validationLayers;

bool checkValidationLayerSupport();
std::vector<const char *> getRequiredExtensions();
extern VkDebugUtilsMessengerEXT debugMessenger;
void setupDebugMesseger();
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator);
void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);
} // namespace bscRND
