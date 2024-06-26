
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

private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

  void createPipeline(VkRenderPass renderPass);

  LveDevice &lveDevice;

  std::unique_ptr<Pipeline> lvePipeline;
  std::unique_ptr<Pipeline> floorPipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve