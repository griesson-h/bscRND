#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <cstdint>

namespace bscRND {
extern const uint32_t WIDTH;
extern const uint32_t HEIGHT;
extern uint32_t currentFrame;

extern GLFWwindow *window;

class Engine {
public:
  float dt;

  void run();

private:
  void initWindow();

  void initVulkan();

  void mainLoop();

  void cleanup();
};
} // namespace bscRND
