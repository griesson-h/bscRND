#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
<<<<<<< HEAD
#include <vector>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
=======

#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <optional>
>>>>>>> 7a6c156 (basic vulkan initializer)

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

<<<<<<< HEAD
=======
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else 
  const bool enableValidationLayers = true;
#endif


>>>>>>> 7a6c156 (basic vulkan initializer)
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
<<<<<<< HEAD
=======
    VkDevice device;
    VkQueue graphicsQueue;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    struct QueueFamilyIndices {
      std::optional<uint32_t> graphicsFamily;

      bool isComplete() {
        return graphicsFamily.has_value();
      }
    };
    // tudu: Сделать кастомную систему слойев валидности (щя лень)

    bool isDeviceSuitable(VkPhysicalDevice device) {
      vkGetPhysicalDeviceProperties(device, &deviceProperties);
      vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

      QueueFamilyIndices indices = findQueueFamilies(device);

      return indices.isComplete(); // tudu: Сделать авто-подборку видюхи
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
        if (indices.isComplete()) {
          break;
        }

        i++;
      }

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
      std::cout << deviceProperties.deviceName << " - has been detected as a graphic proccessing unit" << std::endl;

    }
>>>>>>> 7a6c156 (basic vulkan initializer)

    void initWindow() {
      glfwInit();
      
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

<<<<<<< HEAD
      window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }
    void initVulkan() {
      createInstance();
=======
      window = glfwCreateWindow(WIDTH, HEIGHT, "bscRND", nullptr, nullptr);
    }

    void initVulkan() {
      createInstance();
      pickPhysicalDevice();
      createLogicalDevice();
    }

    void createLogicalDevice() {
      QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
      queueCreateInfo.queueCount = 1;
      float queuePriority = 1.0f;
      queueCreateInfo.pQueuePriorities = &queuePriority;

      VkDeviceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      createInfo.pQueueCreateInfos = &queueCreateInfo;
      createInfo.queueCreateInfoCount = 1;

      createInfo.pEnabledFeatures = &deviceFeatures;

      createInfo.enabledExtensionCount = 0;

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

      std::cout << "Logical device has been created successfuly" << std::endl;
>>>>>>> 7a6c156 (basic vulkan initializer)
    }

    void mainLoop() {
      while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
      }
    }

    void cleanup() {
<<<<<<< HEAD
=======
      vkDestroyDevice(device, nullptr);
>>>>>>> 7a6c156 (basic vulkan initializer)
      vkDestroyInstance(instance, nullptr);
      glfwDestroyWindow(window);

      glfwTerminate();
    }

<<<<<<< HEAD
    void createInstance() {
=======
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
        throw std::runtime_error("validation layers requested, but not available :(");
      }


>>>>>>> 7a6c156 (basic vulkan initializer)
      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Hello Triangle";
      appInfo.applicationVersion = VK_MAKE_VERSION(1,0,1);
      appInfo.pEngineName = "No Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
      appInfo.apiVersion = VK_API_VERSION_1_0;
<<<<<<< HEAD

=======
      
>>>>>>> 7a6c156 (basic vulkan initializer)
      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;

<<<<<<< HEAD
=======
      if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
      } else {
        createInfo.enabledLayerCount = 0;
      }

>>>>>>> 7a6c156 (basic vulkan initializer)
      uint32_t glfwExtensionCount = 0;
      const char** glfwExtensions;
      
      glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

      createInfo.enabledExtensionCount = glfwExtensionCount;
      createInfo.ppEnabledExtensionNames = glfwExtensions;
      createInfo.enabledLayerCount = 0;

      VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
      if (result != VK_SUCCESS) {
<<<<<<< HEAD
        throw std::runtime_error("you somehow managed to fuck up the creation of the instance youre such a moron");
      }
=======
        throw std::runtime_error("Instance creation failed :(");
      }
      std::cout << "The main instance has been created successfuly" << std::endl;
>>>>>>> 7a6c156 (basic vulkan initializer)
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

