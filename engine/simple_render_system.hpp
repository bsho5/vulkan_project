
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
  SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(FrameInfo &frameInfo,
                         std::vector<GameObject> &gameObjects);

private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  LveDevice &lveDevice;

  std::unique_ptr<Pipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve