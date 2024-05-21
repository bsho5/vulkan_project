#pragma once
#include "device.hpp"
#include "pipeline.hpp"
#include "window.hpp"
#include "swap_chain.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {
class App{
    public:
    static constexpr int HEIGHT = 600;
    static constexpr int WIDTH = 800;

// couldn't debug it

    App();
    ~App();

    App(const App &)= delete;
    App &operator =(const App &) = delete;

    void run();

    private:
        void createPipeline();
        void createPipelineLayout();
        void createCommandBuffers();
        void drawFrame();

        Window window{WIDTH,HEIGHT,"vulkan window"};
        Device device{window};
        SwapChain swapChain{device,window.getExtent()};
        std::unique_ptr<Pipeline> pipeline ;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
};
}