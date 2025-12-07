#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"
#include "draw.h"
#include "imageviews.h"
#include "init.h"
#include "pipeline.h"
#include "renderpass.h"
#include "swapchain.h"
#include <stdexcept>

namespace bscRND {
std::vector<VkFramebuffer> swapChainFramebuffers;

const int MAX_FRAMES_IN_FLIGHT = 2;
bool framebufferResized = false;

VkCommandPool commandPool;
std::vector<VkCommandBuffer> commandBuffers;

std::vector<VkSemaphore> imageAvailableSems;
std::vector<VkSemaphore> renderFinishedSems;
std::vector<VkFence> inFlightFens;

void createCommandPool() {
  QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  /*
   * VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers are
   * rerecorded with new commands very often (may change memory allocation
   * behavior) VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command
   * buffers to be rerecorded individually, without this flag they all have to
   * be reset together
   *
   * source:
   * https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Command_buffers
   */
  commandPoolCreateInfo.queueFamilyIndex =
      queueFamilyIndices.graphicsFamily.value();
  if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr,
                          &commandPool)) {
    throw std::runtime_error("Command pool creation failed :(");
  }
}

void createCommandBuffers() {
  commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo commandBufferAlocateInfo{};
  commandBufferAlocateInfo.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAlocateInfo.commandPool = commandPool;
  commandBufferAlocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  /*
   * VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution,
   * but cannot be called from other command buffers.
   * VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be
   * called from primary command buffers.
   *
   * source:
   * https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Command_buffers
   */
  commandBufferAlocateInfo.commandBufferCount = 1;
  commandBufferAlocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  if (vkAllocateCommandBuffers(device, &commandBufferAlocateInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Command buffer allocation failed :(");
  }
}

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Command buffer recording begin failed :(");
  }

  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = renderPass;
  renderPassBeginInfo.framebuffer = swapChainFramebuffers[imageIndex];
  renderPassBeginInfo.renderArea.offset = {0, 0};
  renderPassBeginInfo.renderArea.extent = swapChainExtent;
  VkClearValue clearColor = {{{0.03f, 0.01f, 0.0f, 1.0f}}};
  renderPassBeginInfo.clearValueCount = 1;
  renderPassBeginInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  /*
   * VK_SUBPASS_CONTENTS_INLINE: The render pass commands will be embedded in
   * the primary command buffer itself and no secondary command buffers will be
   * executed. VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render pass
   * commands will be executed from secondary command buffers.
   *
   * source:
   * https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Command_buffers
   */

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphicsPipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapChainExtent.width);
  viewport.height = static_cast<float>(swapChainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapChainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  // HOLY TRIANGLE
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  // HOLY TRIANGLE

  vkCmdEndRenderPass(commandBuffer);
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Command buffer recording failed :(");
  }
}

void createFramebuffers() {
  swapChainFramebuffers.resize(swapChainImageViews.size());

  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    VkImageView attachments[] = {swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = swapChainExtent.width;
    framebufferCreateInfo.height = swapChainExtent.height;
    framebufferCreateInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr,
                            &swapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("Framebuffer creation failure :(");
    }
  }
}

void createSyncObjects() {
  imageAvailableSems.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSems.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFens.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &imageAvailableSems[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &renderFinishedSems[i]) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFens[i]) !=
            VK_SUCCESS) {
      throw std::runtime_error("Semaphores creation failed :(");
    }
  }
}

void framebufferResizeCallBack(GLFWwindow *window, int width, int height) {
  auto bscRND = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
  framebufferResized = true;
}

void drawFrame() {
  vkWaitForFences(device, 1, &inFlightFens[currentFrame], VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
                                          imageAvailableSems[currentFrame],
                                          VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    framebufferResized = true;
    recreateSwapChain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  vkResetFences(device, 1, &inFlightFens[currentFrame]);

  vkResetCommandBuffer(commandBuffers[currentFrame], 0);
  recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSems[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

  VkSemaphore signalSemaphores[] = {renderFinishedSems[currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo,
                    inFlightFens[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("Command buffer submition failed :(");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  result = vkQueuePresentKHR(presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
    framebufferResized = true;
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Unable to acquire swap chain image :(");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
} // namespace bscRND
