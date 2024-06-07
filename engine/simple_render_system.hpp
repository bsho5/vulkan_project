
#pragma once

#include "device.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"
#include "camera.hpp"


// std
#include <memory>
#include <vector>

namespace lve {
class SimpleRenderSystem {
 public:
  SimpleRenderSystem(Device &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(
      VkCommandBuffer commandBuffer,
      std::vector<GameObject> &gameObjects,
      const LveCamera &camera);

 private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  Device &lveDevice;

  std::unique_ptr<Pipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
};
}  // 