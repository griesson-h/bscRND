#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bscRND {
class Shader {
public:
  struct Vertex {
    glm::vec2 pos;
    glm::vec3 color; // if you're britan you can create a macro if you want
                     // (smth like #define colour color)
    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 2>
    getAttributeDescription();
  };

private:
  const std::vector<Vertex> vertices;
};
} // namespace bscRND
