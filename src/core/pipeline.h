#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fstream>
#include <vector>
#include <iostream>


namespace bscRND {
    extern VkPipelineLayout pipelineLayout;
    extern VkShaderModule vertShaderModule;
    extern VkShaderModule fragShaderModule;
    extern VkPipeline graphicsPipeline;

    inline std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if(!file.is_open()) {
            throw std::runtime_error("Binary opening failed :(");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        std::cout << "fileSize: " << fileSize << std::endl;
        file.close();

        return buffer;
    }
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createGraphicsPipeline();
}
