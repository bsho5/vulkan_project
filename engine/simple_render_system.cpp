#include "simple_render_system.hpp"

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
  createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
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

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  lvePipeline = std::make_unique<Pipeline>(
      lveDevice, "../engine/shaders/shader.vert.spv",
      "../engine/shaders/shader.frag.spv", pipelineConfig);
  floorPipeline = std::make_unique<Pipeline>(
      lveDevice, "../engine/shaders/floor_shader.vert.spv",
      "../engine/shaders/floor_shader.frag.spv", pipelineConfig);
}

void SimpleRenderSystem::renderGameObjects(
    FrameInfo &frameInfo, std::vector<GameObject> &gameObjects) {
  // lvePipeline->bind(frameInfo.commandBuffer);
  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0, nullptr);

  // for (auto &obj : gameObjects) {
  //   // obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f,
  //   // glm::two_pi<float>()); obj.transform.rotation.x =
  //   // glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());

  //   SimplePushConstantData push{};
  //   push.modelMatrix = obj.transform.mat4();

  //   push.normalMatrix = obj.transform.normalMatrix();
  //   vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
  //                      VK_SHADER_STAGE_VERTEX_BIT |
  //                          VK_SHADER_STAGE_FRAGMENT_BIT,
  //                      0, sizeof(SimplePushConstantData), &push);
  //   obj.model->bind(frameInfo.commandBuffer);
  //   obj.model->draw(frameInfo.commandBuffer);
  // }
    lvePipeline->bind(frameInfo.commandBuffer);
   SimplePushConstantData push1{};
    // obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f,
    // glm::two_pi<float>()); obj.transform.rotation.x =
    // glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());
    push1.modelMatrix = gameObjects[1].transform.mat4();

    push1.normalMatrix = gameObjects[1].transform.normalMatrix();
    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push1);
    gameObjects[1].model->bind(frameInfo.commandBuffer);
    gameObjects[1].model->draw(frameInfo.commandBuffer);
  floorPipeline->bind(frameInfo.commandBuffer);
   SimplePushConstantData push{};
    // obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f,
    // glm::two_pi<float>()); obj.transform.rotation.x =
    // glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());
    push.modelMatrix = gameObjects[0].transform.mat4();

    push.normalMatrix = gameObjects[0].transform.normalMatrix();
    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
    gameObjects[0].model->bind(frameInfo.commandBuffer);
    gameObjects[0].model->draw(frameInfo.commandBuffer);
}

} // namespace lve