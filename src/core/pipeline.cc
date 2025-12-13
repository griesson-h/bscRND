#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../shader.h"
#include "device.h"
#include "pipeline.h"
#include "renderpass.h"
#include "swapchain.h"

namespace bscRND {
Shader shader;
VkPipelineLayout pipelineLayout;
VkShaderModule vertShaderModule;
VkShaderModule fragShaderModule;
VkPipeline graphicsPipeline;

VkShaderModule createShaderModule(const std::vector<char> &code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    throw std::runtime_error("Shader module creation failed :(");
  }

  return shaderModule;
}

void createGraphicsPipeline() {
  auto vertShaderCode = readFile("../stuff/shaders/vert.spv");
  auto fragShaderCode = readFile("../stuff/shaders/frag.spv");
  auto shaderBinding = shader.getBindingDescription();
  auto shaderAttribute = shader.getAttributeDescriptions();

  vertShaderModule = createShaderModule(vertShaderCode);
  fragShaderModule = createShaderModule(fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{};
  vertShaderStageCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageCreateInfo.module = vertShaderModule;
  vertShaderStageCreateInfo.pName = "main";
  vertShaderStageCreateInfo.pSpecializationInfo =
      nullptr; // не обязательно, просто хочу помнить что это есть так-как это
               // пригодиться позже

  VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
  fragShaderStageCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageCreateInfo.module = fragShaderModule;
  fragShaderStageCreateInfo.pName = "main";
  fragShaderStageCreateInfo.pSpecializationInfo = nullptr; // не обязательно

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageCreateInfo,
                                                    fragShaderStageCreateInfo};

  struct VertexDataDescription {
    // pass
  };

  // Дальше п*здец...

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
  dynamicStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.dynamicStateCount =
      static_cast<uint32_t>(dynamicStates.size());
  dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

  VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
  vertexInputCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
  vertexInputCreateInfo.pVertexBindingDescriptions = &shaderBinding;
  vertexInputCreateInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(shaderAttribute.size());
  vertexInputCreateInfo.pVertexAttributeDescriptions = shaderAttribute.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
  inputAssemblyCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  /*
   * VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
   * VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
   * VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as
   * start vertex for the next line VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
   * triangle from every 3 vertices without reuse
   * VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every
   * triangle are used as first two vertices of the next triangle
   *
   * source:
   * https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
   */
  inputAssemblyCreateInfo.primitiveRestartEnable =
      VK_FALSE; // как я понял это херь позволяет чето оптимизировать вроде но
                // пока что мне это не нужно разберусь когда смогу рендерить
                // триугольник
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swapChainExtent.width;
  viewport.height = (float)swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapChainExtent;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
  viewportStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
  rasterizerCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerCreateInfo.depthClampEnable = VK_FALSE;
  rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  /*
   * VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
   * VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
   * VK_POLYGON_MODE_POINT: polygon vertices are drawn as points
   *
   * source:
   * https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
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

  VkPipelineMultisampleStateCreateInfo
      multisamplingCreateInfo{}; // anti-aliasing
  multisamplingCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
  multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisamplingCreateInfo.minSampleShading = 1.0f;
  multisamplingCreateInfo.pSampleMask = nullptr;
  multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
  multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;
  // temporary disabled

  VkPipelineColorBlendAttachmentState colorBlendAttachmendCreateInfo{};
  colorBlendAttachmendCreateInfo.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachmendCreateInfo.blendEnable = VK_FALSE;
  // в будуйщем надо будет разобраться что это

  VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo{};
  colorBlendingCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendingCreateInfo.attachmentCount = 1;
  colorBlendingCreateInfo.pAttachments = &colorBlendAttachmendCreateInfo;
  // опять же, стоит разобраться потом

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
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

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo,
                                nullptr, &graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("Graphics pipeline creation failed :(");
  }
  vkDestroyShaderModule(device, fragShaderModule, nullptr);
  vkDestroyShaderModule(device, vertShaderModule, nullptr);
}
} // namespace bscRND
