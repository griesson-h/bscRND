#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "swapchainpm.h"
#include "vulkan.h"

#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else 
  const bool enableValidationLayers = true;
#endif
VkSurfaceKHR surface;

class HelloTriangleApplication {
public:
    void run() {
        std::cout << "so, welcome to hell i guess" << std::endl;
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance instance;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    VkPipelineLayout pipelineLayout;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    VkRenderPass renderPass;

    VkPipeline graphicsPipeline;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    struct QueueFamilyIndices {
      std::optional<uint32_t> graphicsFamily;
      std::optional<uint32_t> presentFamily;

      bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
      }
    };

    struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    };
    // tudu: Сделать кастомную систему слойев валидности (щя лень)

    bool isDeviceSuitable(VkPhysicalDevice device) {
      vkGetPhysicalDeviceProperties(device, &deviceProperties);
      vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

      QueueFamilyIndices indices = findQueueFamilies(device);
      bool extensionsSupported = checkDeviceExtensionSupport(device);

      bool swapChainAdequate = false;
      if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
      }

      return indices.isComplete() && extensionsSupported && swapChainAdequate; // tudu: Сделать авто-подборку видюхи
    }

    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
      uint32_t extensionCount;
      vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

      std::vector<VkExtensionProperties> availableExtensions(extensionCount);
      vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

      std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

      for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
      }

      return requiredExtensions.empty();
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
      QueueFamilyIndices indices;

      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
      
      std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

      int i = 0;
      for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
          indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
          indices.presentFamily = i;
        }
        if (indices.isComplete()) {
          break;
        }

        i++;
      }

         // В будуйщем для оптимизации можно добавить отбор видюх чтобы видеокарта могла рендерить и рисовать это на экран на одной очереди

      return indices;
    }

    void pickPhysicalDevice() {
      uint32_t deviceCount = 0;
      vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

      if (deviceCount == 0) {
       throw std::runtime_error("There has been no GPU detected :(");
     }
      
      std::vector<VkPhysicalDevice> devices(deviceCount);
      vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

      for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
          physicalDevice = device;
          break;
        }

        if (physicalDevice == VK_NULL_HANDLE) {
          throw std::runtime_error("Unable to find any GPU which fits requirments :(");
        }

      }
      std::cout << "GPU detected: " << deviceProperties.deviceName << std::endl;

    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
      SwapChainSupportDetails details;
      
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

      if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
      }

      uint32_t presentModeCount;
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

      if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
      }

      return details;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
      for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == 
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
          return availableFormat;
        }
      }

      return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
#ifdef VK_PRESENT_MODE_MAILBOX_KHR_IN_USE
      for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        std::cout << "Present mode has been selected to: " << "VK_PRESENT_MODE_MAILBOX_KHR" << std::endl;
          return availablePresentMode;
        }
      }
#endif
#ifdef VK_PRESENT_MODE_FIFO_RELAXED_KHR_IN_USE
      for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
          std::cout << "Present mode has been selected to: " << "VK_PRESENT_MODE_FIFO_RELAXED_KHR" << std::endl;
          return availablePresentMode;
        }
      }     
#endif
#ifdef VK_PRESENT_MODE_FIFO_KHR_IN_USE
      for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
          std::cout << "Present mode has been selected to: " << "VK_PRESENT_MODE_FIFO_KHR" << std::endl;
          return availablePresentMode;
        }
      }
#endif
      std::cout << "Present mode has been selected to: " << "VK_PRESENT_MODE_IMMEDIATE_KHR" << std::endl;
      return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
      if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
      } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
          static_cast<uint32_t>(width),
          static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
      }
    }

    void initWindow() {
      glfwInit();
      
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

      window = glfwCreateWindow(WIDTH, HEIGHT, "bscRND", nullptr, nullptr);
    }

    void initVulkan() {
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
    }

    void createImageViews() {
      swapChainImageViews.resize(swapChainImages.size());
      


      for (size_t i = 0; i < swapChainImages.size(); ++i) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
          throw std::runtime_error("Image views creation failed :(");
        }
      }
    }

    void createSurface() {
      if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Window surface creation failed :(");
      }
    }

    void createSwapChain() {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

      VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
      VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
      VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

      uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
      if (swapChainSupport.capabilities.maxImageCount > 0 /*zero means there's no max*/
          && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
      }

      VkSwapchainCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = surface;

      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = surfaceFormat.format;
      createInfo.imageColorSpace = surfaceFormat.colorSpace;
      createInfo.imageExtent = extent;
      createInfo.imageArrayLayers = 1;
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

      QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
      uint32_t QueueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

      if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = QueueFamilyIndices;
      } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      }

      createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
      createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      createInfo.presentMode = presentMode;
      createInfo.clipped = VK_TRUE;
      createInfo.oldSwapchain = VK_NULL_HANDLE;
      
      if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("Swapchain creation failed :(");
      }

      vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
      swapChainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

      swapChainImageFormat = surfaceFormat.format;
      swapChainExtent = extent;
    }

    void createLogicalDevice() {
      QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

      std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
      std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
      float queuePriority = 1.0f;
      for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
      }

      VkDeviceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      createInfo.pQueueCreateInfos = queueCreateInfos.data();
      createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

      createInfo.pEnabledFeatures = &deviceFeatures;

      createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
      createInfo.ppEnabledExtensionNames = deviceExtensions.data();

      if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
      } else {
        createInfo.enabledLayerCount = 0;
      }

      if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Logical device creation failed :(");
      }

      vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
      vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    }

    void mainLoop() {
      while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
      }
    }


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

    void createInstance() {
      if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available :(");
      }


      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Hello Triangle";
      appInfo.applicationVersion = VK_MAKE_VERSION(1,0,1);
      appInfo.pEngineName = "No Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
      appInfo.apiVersion = VK_API_VERSION_1_0;
      
      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;

      if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
      } else {
        createInfo.enabledLayerCount = 0;
      }

      uint32_t glfwExtensionCount = 0;
      const char** glfwExtensions;
      
      glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

      createInfo.enabledExtensionCount = glfwExtensionCount;
      createInfo.ppEnabledExtensionNames = glfwExtensions;
      createInfo.enabledLayerCount = 0;

      VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
      if (result != VK_SUCCESS) {
        throw std::runtime_error("Instance creation failed :(");
      }
    }

    void createRenderPass() {
      VkAttachmentDescription colorAttachment{};
      colorAttachment.format = swapChainImageFormat;
      colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
      colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      /*
      VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
      VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
      VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them

      source: https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes
      */
      colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      /*
      VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
      VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation

      source: https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes
      */
      colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      /*
      Some of the most common layouts are:

      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation

      source: https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes
      (если быть чесным я ниче не понял практически но ладно со временем пройму)
      */
      
      VkAttachmentReference colorAttachmentRef{};
      colorAttachmentRef.attachment = 0;
      colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      VkSubpassDescription subpass{};
      subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpass.colorAttachmentCount = 1;
      subpass.pColorAttachments = &colorAttachmentRef;
      /*
      The following other types of attachments can be referenced by a subpass:

      pInputAttachments: Attachments that are read from a shader
      pResolveAttachments: Attachments used for multisampling color attachments
      pDepthStencilAttachment: Attachment for depth and stencil data
      pPreserveAttachments: Attachments that are not used by this subpass, but for which the data must be preserved

      source: https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes
      */

      VkRenderPassCreateInfo renderPassCreateInfo{};
      renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
      renderPassCreateInfo.attachmentCount = 1;
      renderPassCreateInfo.pAttachments = &colorAttachment;
      renderPassCreateInfo.subpassCount = 1;
      renderPassCreateInfo.pSubpasses = &subpass;

      if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Render pass creation failed :(");
      }
    }

    void createGraphicsPipeline() {
      auto vertShaderCode = readFile("../stuff/shaders/vert.spv");
      auto fragShaderCode = readFile("../stuff/shaders/frag.spv");

      vertShaderModule = createShaderModule(vertShaderCode);
      fragShaderModule = createShaderModule(fragShaderCode);

      VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{};
      vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
      vertShaderStageCreateInfo.module = vertShaderModule;
      vertShaderStageCreateInfo.pName = "main";
      vertShaderStageCreateInfo.pSpecializationInfo = nullptr; // не обязательно, просто хочу помнить что это есть так-как это пригодиться позже 
      
      VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
      fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      fragShaderStageCreateInfo.pName = "main";
      fragShaderStageCreateInfo.pSpecializationInfo = nullptr; // не обязательно
      
      VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageCreateInfo,
        fragShaderStageCreateInfo
      };

      struct VertexDataDescription {
        // pass
      };
      
      // Дальше п*здец...

      std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
      };

      VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
      dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
      dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
      
      VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
      vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
      vertexInputCreateInfo.pVertexBindingDescriptions = nullptr; // мы еще к этому вернемся...
      vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
      vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr; // мы еще к этому вернемся...

      VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
      inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      /*
      VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
      VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
      VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as start vertex for the next line
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices without reuse
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every triangle are used as first two vertices of the next triangle

      source: https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
      */
      inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE; // как я понял это херь позволяет чето оптимизировать вроде но пока что мне это не нужно разберусь когда смогу рендерить триугольник 
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float) swapChainExtent.width;
      viewport.height = (float) swapChainExtent.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = swapChainExtent;

      VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
      viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportStateCreateInfo.viewportCount = 1;
      viewportStateCreateInfo.scissorCount = 1;

      VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
      rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizerCreateInfo.depthClampEnable = VK_FALSE;
      rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
      rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
      /*
      VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
      VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
      VK_POLYGON_MODE_POINT: polygon vertices are drawn as points

      source: https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
      */
      rasterizerCreateInfo.lineWidth = 1.0f;
      rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
      rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
      // шото для теней
      rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
      rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
      rasterizerCreateInfo.depthBiasClamp = 0.0f;
      rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;
      //

      VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{}; // anti-aliasing
      multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
      multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      multisamplingCreateInfo.minSampleShading = 1.0f;
      multisamplingCreateInfo.pSampleMask = nullptr;
      multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
      multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;
      // temporary disabled

      VkPipelineColorBlendAttachmentState colorBlendAttachmendCreateInfo{};
      colorBlendAttachmendCreateInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      colorBlendAttachmendCreateInfo.blendEnable = VK_FALSE;
      // в будуйщем надо будет разобраться что это

      VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo{};
      colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
      colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY;
      colorBlendingCreateInfo.attachmentCount = 1;
      colorBlendingCreateInfo.pAttachments = &colorBlendAttachmendCreateInfo;
      // опять же, стоит разобраться потом

      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

      if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Pipeline layout creation failed :(");
      }

     // Я ЭТО СДЕЛАЛ ЕМАЕ КАК ЖЕ Я ЗАКОЛЕБАЛСЯ

      VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
      pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineCreateInfo.stageCount = 2;
      pipelineCreateInfo.pStages = shaderStages;
      pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
      pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
      pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
      pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
      pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
      pipelineCreateInfo.pDepthStencilState = nullptr; // Optional
      pipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
      pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
      pipelineCreateInfo.layout = pipelineLayout;
      pipelineCreateInfo.renderPass = renderPass;
      pipelineCreateInfo.subpass = 0;
      pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      pipelineCreateInfo.basePipelineIndex = -1;

      if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Graphics pipeline creation failed :(");
      }
      vkDestroyShaderModule(device, fragShaderModule, nullptr);
      vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }
    
    VkShaderModule createShaderModule(const std::vector<char>& code) {
     VkShaderModuleCreateInfo createInfo{};
     createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
     createInfo.codeSize = code.size();
     createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
     VkShaderModule shaderModule;
     if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
       throw std::runtime_error("Shader module creation failed :(");
     }

     return shaderModule;
    }

    void cleanup() {
      std::cout << "Deallocating memory and quiting.." << std::endl;
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
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
