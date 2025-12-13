#include "shader.h"
#include "core/device.h"
#include "core/draw.h"
#include "core/pipeline.h"
#include "iostream"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bscRND {

VkVertexInputBindingDescription Shader::getBindingDescription() {
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Shader::Vertex);
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

std::array<VkVertexInputAttributeDescription, 2>
Shader::getAttributeDescriptions() {
  std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Shader::Vertex, pos);

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Shader::Vertex, color);

  return attributeDescriptions;
}

void Shader::resize(uint32_t resizeVertex, uint32_t resizeIndex) {
  vertices.resize(resizeVertex);
  indices.resize(resizeIndex);
}

void Shader::setVertex(uint32_t VertexIndex, glm::float32_t Xpos,
                       glm::float32_t Ypos, glm::float32_t RRgbo,
                       glm::float32_t GRgbo, glm::float32_t BRgbo) {
  Vertex Vertex = {{Xpos, Ypos}, {RRgbo, GRgbo, BRgbo}};
  vertices.at(VertexIndex) = Vertex;
}

void Shader::setIndices(std::vector<uint16_t> indicesArr) {
  indices.resize(indicesArr.size());

  indices.swap(indicesArr);
}

size_t Shader::getIndicesSize() { return indices.size(); }

uint32_t Shader::findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if (typeFilter & (1 << i) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) ==
            properties) { // wtf is this lol
      return i;
    }
  }
  throw std::runtime_error("ur gpu aint got no memory lol :(");

  // i actually have no idea what we're doing here
}

void Shader::setUpValues() {
  resize(6, 6);

  shader.setIndices({0, 1, 2, 3, 4, 5});

  shader.setVertex(0, -0.75f, 0.0f, 1.0f, 1.0f, 1.0f);
  shader.setVertex(1, -0.25f, -0.25f, 1.0f, 1.0f, 1.0f);
  shader.setVertex(2, -0.25f, 0.25f, 1.0f, 1.0f, 1.0f);

  shader.setVertex(3, 0.75f, 0.0f, 1.0f, 1.0f, 1.0f);
  shader.setVertex(4, 0.25f, -0.25f, 1.0f, 1.0f, 1.0f);
  shader.setVertex(5, 0.25f, 0.25f, 1.0f, 1.0f, 1.0f);

  for (auto i = 0; i < vertices.size(); i++) {
    for (auto k = 0; k <= 1; k++) {
      std::cout << vertices[i].pos[k] << std::endl;
    }
  }
  for (auto i = 0; i < indices.size(); i++) {
    std::cout << indices[i] << std::endl;
  }
}

void Shader::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties, VkBuffer &buffer,
                          VkDeviceMemory &bufferMemory) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("Buffer creation failed :(");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, properties);
  if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("Memory allocation failed :(");
  }
  vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Shader::createVertexBuffer() {
  shader.resize(4, 6);
  // shader.setUpValues();
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  shader.setVertex(0, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f);
  shader.setVertex(1, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f);
  shader.setVertex(2, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f);
  shader.setVertex(3, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f);
  shader.setIndices({0, 1, 2, 2, 3, 0});

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0,
              bufferSize /*если зарофлить то можно поставить VK_WHOLE_SIZE*/, 0,
              &data);
  memcpy(data, vertices.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

  copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Shader::createIndexBuffer() {
  VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

  copyBuffer(stagingBuffer, indexBuffer, bufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Shader::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                        VkDeviceSize size) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0;
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue);

  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
} // namespace bscRND
