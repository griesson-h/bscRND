#include "shader.h"
#include <array>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bscRND {

VkVertexInputBindingDescription Shader::Vertex::getBindingDescription() {
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  /*
  VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
  VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each
  instance

  source:
  https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description
  */

  return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
  std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

  return attributeDescriptions;
}

} // namespace bscRND
