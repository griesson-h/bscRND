#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "init.h"
#include "device.h"
#include "imageviews.h"
#include "instance.h"
#include "pipeline.h"
#include "renderpass.h"
#include "surface.h"
#include "swapchain.h"
#include "draw.h"

#include <iostream>
#include <cstdlib>
#include "../stb/stb_image.h"

using namespace bscRND;

namespace bscRND {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow* window;

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
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            window = glfwCreateWindow(WIDTH, HEIGHT, "bscRND", nullptr, nullptr);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            GLFWimage icons[1];
            icons[0].pixels = stbi_load("../stuff/icon.png", &icons[0].width, &icons[0].height, 0, 4);
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
            createCommandBuffer();
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
            vkDestroySemaphore(device, imageAvailableSem, nullptr);
            vkDestroySemaphore(device, renderFinishedSem, nullptr);
            vkDestroyCommandPool(device, commandPool, nullptr);
            for (auto framebuffer : swapChainFramebuffers) {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            }
            vkDestroyPipeline(device, graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            vkDestroyRenderPass(device, renderPass, nullptr);
            for (auto imageView : swapChainImageViews) {
                vkDestroyImageView(device, imageView, nullptr);
            }
            vkDestroySwapchainKHR(device, swapChain, nullptr);
            vkDestroyDevice(device, nullptr);
            vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyInstance(instance, nullptr);
            glfwDestroyWindow(window);

            glfwTerminate();
        }
}

int main() {
    Engine bscRND;
    try {
        bscRND.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

