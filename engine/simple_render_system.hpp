
#pragma once

#include "device.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
class SimpleRenderSystem {
public:
  SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass,
                     VkDescriptorSetLayout globalSetLayout);

  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(FrameInfo &frameInfo,
                         std::vector<GameObject> &gameObjects);
  void renderSkyBox(FrameInfo &frameInfo);
  void renderOcean(FrameInfo &frameInfo);
  void renderMoon(FrameInfo &frameInfo);

private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

  void createPipeline(VkRenderPass renderPass);

  LveDevice &lveDevice;

  std::unique_ptr<Pipeline> skyBoxPipeline;
  std::unique_ptr<Pipeline> oceanPipeline;
  std::unique_ptr<Pipeline> moonPipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve