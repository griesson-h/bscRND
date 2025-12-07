#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"
#include "draw.h"
#include "imageviews.h"
#include "init.h"
#include "instance.h"
#include "pipeline.h"
#include "renderpass.h"
#include "surface.h"
#include "swapchain.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#include <cstdlib>
#include <iostream>

using namespace bscRND;

namespace bscRND {
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
GLFWwindow *window;
uint32_t currentFrame = 0;

void Engine::run() {
  std::cout << "so, welcome to hell i guess" << std::endl;
  initWindow();
  initVulkan();
  mainLoop();
  cleanup();
}
void Engine::initWindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "bscRND", nullptr, nullptr);
  if (window == NULL)
    throw std::runtime_error(
        "Window creation failed (are you running this in terminal?) :(");
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallBack);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  GLFWimage icons[1];
   icons[0].pixels =
       stbi_load("../stuff/icon.png", &icons[0].width, &icons[0].height, 0, 4);
  glfwSetWindowIcon(window, 1, icons);
  stbi_image_free(icons[0].pixels);
}

void Engine::initVulkan() {
  createInstance();
  std::cout << "The main instance has been created successfuly" << std::endl;
  createSurface();
  std::cout << "Window surface has been created successfuly" << std::endl;
  pickPhysicalDevice();
  createLogicalDevice();
  std::cout << "Logical device has been created successfuly" << std::endl;
  createSwapChain();
  std::cout << "Swapchain has been created successfuly" << std::endl;
  createImageViews();
  std::cout << "Image views have been created successfuly" << std::endl;
  createRenderPass();
  std::cout << "Render pass has been created successfuly" << std::endl;
  createGraphicsPipeline();
  std::cout << "Graphics pipeline has been created successfuly" << std::endl;
  createFramebuffers();
  std::cout << "Framebuffers have been created successfuly" << std::endl;
  createCommandPool();
  std::cout << "Command pool has been created successfuly" << std::endl;
  createCommandBuffers();
  std::cout << "Command buffer has been allocated successfuly" << std::endl;
  createSyncObjects();
  std::cout << "Sync objects have been created successfuly" << std::endl;
}

void Engine::mainLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(device);
}

void Engine::cleanup() {
  std::cout << "Deallocating memory and quiting.." << std::endl;

  cleanupSwapChain();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, imageAvailableSems[i], nullptr);
    vkDestroySemaphore(device, renderFinishedSems[i], nullptr);
    vkDestroyFence(device, inFlightFens[i], nullptr);
  }
  vkDestroyCommandPool(device, commandPool, nullptr);
  vkDestroyPipeline(device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
  vkDestroyRenderPass(device, renderPass, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
  glfwDestroyWindow(window);

  glfwTerminate();
}
} // namespace bscRND

int main() {
  Engine bscRND;
  try {
    bscRND.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
