#pragma once
#include "device.hpp"
#include "game_object.hpp"

#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "window.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {
class App {
public:
  static constexpr int HEIGHT = 600;
  static constexpr int WIDTH = 800;

  // couldn't debug it

  App();
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  void run();

private:
  void loadGameObjects();
  void createPipeline();
  void createPipelineLayout();
  void createCommandBuffers();
    void freeCommandBuffers();

  void drawFrame();
   void recreateSwapChain();
  void recordCommandBuffer(int imageIndex);
  void sierpinski(std::vector<Model::Vertex> &vertices, int depth,
                  glm::vec2 left, glm::vec2 right, glm::vec2 top);
  void renderGameObjects(VkCommandBuffer commandBuffer);

  Window window{WIDTH, HEIGHT, "vulkan window"};
  Device device{window};
  std::unique_ptr<SwapChain> swapChain;
  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::vector<GameObject> gameObjects;
};
} // namespace lve