#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "validation.h"

#include <vector>
#include <iostream>
#include <cstring>

using namespace bscRND;

namespace bscRND {
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                std::cout << layerName << " " << layerProperties.layerName << std::endl;
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                std::cout << "checkValidationLayerSupport returned: false" << std::endl;
                return false;
            }

        }

        std::cout << "checkValidationLayerSupport returned: true" << std::endl;
        return true;
    }
}
