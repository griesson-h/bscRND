#pragma once

#include <vector>

namespace bscRND {
    #ifdef NDEBUG
    const bool enableValidationLayers = false;
    #else
    const bool enableValidationLayers = true;
    #endif

    extern const std::vector<const char*> validationLayers;

    bool checkValidationLayerSupport();
}
