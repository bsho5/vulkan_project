#pragma once

#include "device.hpp"
#include <vulkan/vulkan_core.h>

namespace lve {
    class CubeMap {
    public:
        CubeMap(LveDevice &device);
        ~CubeMap();

        CubeMap(const CubeMap &) = delete;
        CubeMap &operator=(const CubeMap &) = delete;
        CubeMap(CubeMap &&) = delete;
        CubeMap &operator=(CubeMap &&) = delete;

        VkSampler getSampler() { return sampler; }
        VkImageView getImageView() { return imageView; }
        VkImageLayout getImageLayout() { return imageLayout; }
    private:
        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
        void generateMipmaps();
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        int width ;
        int height;
        int mipLevels;
        int channels;
        int m_BytesPerPixel;

        LveDevice& lveDevice;
VkImage cubemapImage;
VkDeviceMemory cubemapImageMemory;

        VkImageView imageView;
        VkSampler sampler;
        VkFormat imageFormat;
        VkImageLayout imageLayout;
    };
}