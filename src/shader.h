#pragma once
#include <array>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bscRND {
class Shader {
public:
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  struct Vertex {
    glm::vec2 pos;
    glm::vec3 color; // if you're britan you can create a macro if you want
                     // (smth like #define colour color)
  };

  std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

  VkVertexInputBindingDescription getBindingDescription();

  void setUpValues();

  void resize(uint32_t resizeVertex, uint32_t resizeIndex);

  size_t getIndicesSize();

  void setVertex(uint32_t VertexIndex, glm::float32_t Xpos, glm::float32_t Ypos,
                 glm::float32_t RRgbo, glm::float32_t GRgbo,
                 glm::float32_t BRgbo);

  void setIndices(std::vector<uint16_t> indicesArr);

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  void createVertexBuffer();
  void createIndexBuffer();
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

private:
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
};
} // namespace bscRND
