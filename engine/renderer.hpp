
#pragma once

#include "device.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace   lve{
class  Renderer {
 public:
   Renderer( Window &window,  LveDevice &device);
  ~ Renderer();

   Renderer(const  Renderer &) = delete;
   Renderer &operator=(const  Renderer &) = delete;

  VkRenderPass getSwapChainRenderPass() const { return  swapChain->getRenderPass(); }
  bool isFrameInProgress() const { return isFrameStarted; }

    float getAspectRatio() const { return swapChain->extentAspectRatio(); }


  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted && "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

 private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  Window & window;
  LveDevice & device;
  std::unique_ptr<SwapChain>  swapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  int currentFrameIndex;
  bool isFrameStarted;

};
}