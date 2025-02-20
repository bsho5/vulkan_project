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
    Pipeline(LveDevice &device,const std::string &vertFilePath,const std::string &fragFilePath, const PipelineConfigInfo& configInfo);
    Pipeline(LveDevice &device,const std::string &computeFilePath,const PipelineConfigInfo& configInfo);
    ~Pipeline();
     Pipeline (const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &)=delete;
 static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo); 
    void bind(VkCommandBuffer commandBuffer);
    void bindCompute(VkCommandBuffer commandBuffer);

 
    private:
    static std::vector<char> readFile(const std::string &filePath);
    void createGraphicsPipeline(const std::string &vertFilePath,const std::string &fragFilePath, const PipelineConfigInfo& configInfoVkShaderModule );
    void createComputePipeline(const std::string &compFilePath,const PipelineConfigInfo &configInfo);
    void createShaderModule(const std::vector<char>& code,VkShaderModule *shaderModule);
    LveDevice &pipelineDevice;
    VkPipeline graphicsPipeline;
    VkPipeline computePipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkShaderModule computeShaderModule;

        

};
}