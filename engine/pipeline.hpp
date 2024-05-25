#pragma once


#include "device.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {

struct PipelineConfigInfo {
    VkPipelineViewportStateCreateInfo viewportInfo;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector<VkDynamicState> dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class Pipeline{
    
    public:
    Pipeline(Device &device,const std::string &vertFilePath,const std::string &fragFilePath, const PipelineConfigInfo& configInfo);
    ~Pipeline();
     Pipeline (const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &)=delete;
 static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);    void bind(VkCommandBuffer commandBuffer);
 
    private:
    static std::vector<char> readFile(const std::string &filePath);
    void createGraphicsPipeline(const std::string &vertFilePath,const std::string &fragFilePath, const PipelineConfigInfo& configInfoVkShaderModule );
    void createShaderModule(const std::vector<char>& code,VkShaderModule *shaderModule);
    Device &pipelineDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

        

};
}