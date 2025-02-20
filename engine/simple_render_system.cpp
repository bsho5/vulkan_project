#include "simple_render_system.hpp"
#include "glm/fwd.hpp"
#include <vulkan/vulkan_core.h>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <cassert>
#include <stdexcept>

namespace lve {

struct SimplePushConstantData {
  glm::mat4 modelMatrix{1.f};
  glm::mat4 normalMatrix{1.f};
};

SimpleRenderSystem::SimpleRenderSystem(LveDevice &device,
                                       VkRenderPass renderPass,
                                       VkDescriptorSetLayout globalSetLayout)

    : lveDevice{device} {
  createPipelineLayout(globalSetLayout);
  createComputePipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
   vkDestroyPipelineLayout(lveDevice.device(), computePipelineLayout, nullptr);
  vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
  
}

void SimpleRenderSystem::createPipelineLayout(
    VkDescriptorSetLayout globalSetLayout) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);
  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void SimpleRenderSystem::createComputePipelineLayout(
    VkDescriptorSetLayout globalSetLayout) {
 
  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};
  

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  

  if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr,
                             &computePipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create compute pipeline layout!");
  }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");
  assert(computePipelineLayout != nullptr &&
         "Cannot create pipeline before compute pipeline layout");
  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  skyBoxPipeline = std::make_unique<Pipeline>(
      lveDevice, "../engine/shaders/spv/skybox_shader.vert.spv",
      "../engine/shaders/spv/skybox_shader.frag.spv", pipelineConfig);
  moonPipeline = std::make_unique<Pipeline>(
      lveDevice, "../engine/shaders/spv/moon_shader.vert.spv",
      "../engine/shaders/spv/moon_shader.frag.spv", pipelineConfig);
  oceanPipeline = std::make_unique<Pipeline>(
      lveDevice, "../engine/shaders/spv/ocean_shader.vert.spv",
      "../engine/shaders/spv/ocean_shader.frag.spv", pipelineConfig);


   PipelineConfigInfo comptePipelineConfig{};
  comptePipelineConfig.pipelineLayout = computePipelineLayout;
  computeOceanPipeline = std::make_unique<Pipeline>(
      lveDevice, "../engine/shaders/spv/compute_ocean_shader.comp.spv",
      comptePipelineConfig );
}

void SimpleRenderSystem::renderGameObjects(
    FrameInfo &frameInfo, std::vector<GameObject> &gameObjects) {

  moonPipeline->bind(frameInfo.commandBuffer);
  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0, nullptr);
  SimplePushConstantData push{};
  for (auto &obj : gameObjects) {
    push.modelMatrix = obj.transform.mat4();

    push.normalMatrix = obj.transform.normalMatrix();
    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
    obj.model->bind(frameInfo.commandBuffer);

    obj.model->draw(frameInfo.commandBuffer);
  }
}

void SimpleRenderSystem::renderSkyBox(FrameInfo &frameInfo) {
  skyBoxPipeline->bind(frameInfo.commandBuffer);
  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0, nullptr);
  SimplePushConstantData push{};
    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
  
  vkCmdDraw(frameInfo.commandBuffer, 36, 1, 0, 0);
}

void SimpleRenderSystem::renderOcean(FrameInfo &frameInfo) {
  oceanPipeline->bind(frameInfo.commandBuffer);
  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0, nullptr);
  SimplePushConstantData push{};
    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
  
  vkCmdDraw(frameInfo.commandBuffer, 6291456, 1, 0, 0);
}
void SimpleRenderSystem::renderMoon(FrameInfo &frameInfo) {
    moonPipeline->bind(frameInfo.commandBuffer);
  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0, nullptr);
  SimplePushConstantData push{};
    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);

  vkCmdDraw(frameInfo.commandBuffer, 36, 1, 0, 0);
}

void SimpleRenderSystem::computeOcean(FrameInfo &frameInfo) {
    computeOceanPipeline->bindCompute(frameInfo.commandBuffer);
  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0, nullptr);
                            SimplePushConstantData push{};

  uint32_t numElements = 1572864;
  uint32_t groupCount = (numElements  / 64) ;
  vkCmdDispatch(frameInfo.commandBuffer, groupCount, 1, 1);
}
} // namespace lve