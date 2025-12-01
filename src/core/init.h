#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>
namespace bscRND {
    extern const uint32_t WIDTH;
    extern const uint32_t HEIGHT;

    extern GLFWwindow* window;

    class Engine {
    public:
        void run();
    private:
        void initWindow();

        void initVulkan();

        void mainLoop();

        void cleanup();
    };
}
