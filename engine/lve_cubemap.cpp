#include "lve_cubemap.hpp"
#include "device.hpp"
#include <cstdio>
#include <stdexcept>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"

namespace lve {
CubeMap::CubeMap(LveDevice &device)
    : lveDevice{device} {

  const char *cubemapFaces[6] = {
      "../engine/textures/cube-map/px.png", "../engine/textures/cube-map/nx.png",
      "../engine/textures/cube-map/py.png", "../engine/textures/cube-map/ny.png",
      "../engine/textures/cube-map/pz.png", "../engine/textures/cube-map/nz.png",
  };

  // mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width,
  // height)))) + 1;
  mipLevels = static_cast<uint32_t>(1);


  stbi_uc *pixels[6] = {};

  for (int i = 0; i < 6; i++) {
    pixels[i] =
        stbi_load(cubemapFaces[i], &width, &height, &m_BytesPerPixel, 0);
    if (!pixels[i]) {

      throw std::runtime_error("Failed to load cubemap face image! ");
    }
  }
    VkDeviceSize imageSize = width * height * m_BytesPerPixel;
    if (imageSize % 4 != 0) {
    imageSize += 4 - (imageSize % 4);
}
//   LveBuffer stagingBuffer{lveDevice, static_cast<uint32_t>(width * height * 4),
//                           1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

//   // Create and allocate staging buffer ...

//   stagingBuffer.map();
  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.extent = {static_cast<uint32_t>(width),
                            static_cast<uint32_t>(height), 1};
  imageCreateInfo.mipLevels =
      1; // You can change this if you plan to generate mipmaps
  imageCreateInfo.arrayLayers = 6; // 6 layers for a cubemap
  imageCreateInfo.format =
      VK_FORMAT_R8G8B8A8_UNORM; // Use the format appropriate for your images
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageCreateInfo.usage =
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.flags =
      VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT; // Important for cubemaps

  lveDevice.createImageWithInfo(imageCreateInfo,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                cubemapImage, cubemapImageMemory);

  transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  // unsigned char* whiteData = new unsigned char(width*height*4);
  // memset(whiteData, 0xff000000, width*height*4);

  // Copy each face's data into the staging buffer
//   for (int i = 0; i < 6; i++) {
//     VkBufferImageCopy regions = {};
//     VkCommandBuffer commandBuffer = lveDevice.beginSingleTimeCommands();
//     stagingBuffer.writeToBuffer(pixels[i]);
//     regions.bufferOffset = imageSize * i;
//     regions.bufferRowLength = 0;
//     regions.bufferImageHeight = 0;
//     regions.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//     regions.imageSubresource.mipLevel = 0;
//     regions.imageSubresource.baseArrayLayer = i;
//     regions.imageSubresource.layerCount = 1;
//     regions.imageOffset = {0, 0, 0};
//     regions.imageExtent = {static_cast<uint>(width), static_cast<uint>(height),
//                            1};
//     vkCmdCopyBufferToImage(commandBuffer, stagingBuffer.getBuffer(),
//                            cubemapImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                            1, &regions);
//     lveDevice.endSingleTimeCommands(commandBuffer);
//     // memcpy(static_cast<uint8_t*>(data) + i * imageSize, pixels[i],
//     // static_cast<size_t>(imageSize));
//   }
//   transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

//   stagingBuffer.unmap();


VkBuffer stagingBuffer{};
VkDeviceMemory stagingBufferMemory{};

  device.createBuffer(static_cast<size_t>(imageSize * 6.5), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                      stagingBufferMemory);
// Create and allocate staging buffer ...

void* data;
vkMapMemory(device.device(), stagingBufferMemory, 0, static_cast<size_t>(imageSize*6.5), 0, &data);

// Copy each face's data into the staging buffer

uint8_t sum =0;
for (int i = 0; i < 6; i++) {
    memcpy(static_cast<uint8_t*>(data) + (i * imageSize), pixels[i], static_cast<size_t>(imageSize));

}

data = nullptr;
VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

VkBufferImageCopy regions[6] = {};
for (int i = 0; i < 6; i++) {
    regions[i].bufferOffset = imageSize * i;
    regions[i].bufferRowLength = 0;
    regions[i].bufferImageHeight = 0;
    regions[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    regions[i].imageSubresource.mipLevel = 0;
    regions[i].imageSubresource.baseArrayLayer = i;
    regions[i].imageSubresource.layerCount = 1;
    regions[i].imageOffset = {0, 0, 0};
    regions[i].imageExtent = {static_cast<uint>(width), static_cast<uint>(height), 1};
}

vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, cubemapImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6, regions);



device.endSingleTimeCommands(commandBuffer);
vkUnmapMemory(device.device(), stagingBufferMemory);
vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
 transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  // generateMipmaps();
  imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter =
      VK_FILTER_LINEAR; // Magnification filter (linear interpolation)
  samplerInfo.minFilter =
      VK_FILTER_LINEAR; // Minification filter (linear interpolation)
  samplerInfo.mipmapMode =
      VK_SAMPLER_MIPMAP_MODE_NEAREST; // Set to nearest since mipmaps are not
                                      // used
  samplerInfo.addressModeU =
      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; // Wrap mode for U (S) coordinate
  samplerInfo.addressModeV =
      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; // Wrap mode for V (T) coordinate
  samplerInfo.addressModeW =
      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; // Wrap mode for W (R) coordinate
  samplerInfo.mipLodBias = 0.0f; // No level of detail bias, but can be omitted
  samplerInfo.anisotropyEnable = VK_TRUE; // Enable anisotropic filtering
  samplerInfo.maxAnisotropy = 4.0f;       // Maximum anisotropy, typically 16.0
  samplerInfo.compareEnable = VK_FALSE;   // No comparison function needed
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS; // No comparison operator needed
  samplerInfo.minLod = 0.0f;                   // No mipmaps, so minLod is 0
  samplerInfo.maxLod = 0.0f;                   // No mipmaps, maxLod is 0
  samplerInfo.borderColor =
      VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; // Border color if using border
                                          // addressing mode
  // samplerInfo.unnormalizedCoordinates = VK_FALSE; // Use normalized texture
  // coordinates

  vkCreateSampler(lveDevice.device(), &samplerInfo, nullptr, &sampler);

  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
  viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  viewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                         VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 6;
  viewInfo.image = cubemapImage;

  vkCreateImageView(lveDevice.device(), &viewInfo, nullptr, &imageView);

  for (int i = 0; i < 6; i++) {
    stbi_image_free(pixels[i]);
  }
}

CubeMap::~CubeMap() {
  vkDestroyImage(lveDevice.device(), cubemapImage, nullptr);
  vkFreeMemory(lveDevice.device(), cubemapImageMemory, nullptr);
  vkDestroyImageView(lveDevice.device(), imageView, nullptr);
  vkDestroySampler(lveDevice.device(), sampler, nullptr);
}

void CubeMap::transitionImageLayout(VkImageLayout oldLayout,
                                    VkImageLayout newLayout) {
  VkCommandBuffer commandBuffer = lveDevice.beginSingleTimeCommands();

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = cubemapImage;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = mipLevels;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 6;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::runtime_error("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  lveDevice.endSingleTimeCommands(commandBuffer);
}

void CubeMap::generateMipmaps() {
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties(lveDevice.getPhysicalDevice(),
                                      imageFormat, &formatProperties);

  if (!(formatProperties.optimalTilingFeatures &
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    throw std::runtime_error(
        "CubeMap image format does not support linear blitting!");
  }

  VkCommandBuffer commandBuffer = lveDevice.beginSingleTimeCommands();

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = cubemapImage;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.subresourceRange.levelCount = 1;

  int32_t mipWidth = width;
  int32_t mipHeight = height;

  for (uint32_t i = 1; i < mipLevels; i++) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &barrier);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1,
                          mipHeight > 1 ? mipHeight / 2 : 1, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(commandBuffer, cubemapImage,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, cubemapImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                   VK_FILTER_LINEAR);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);

    if (mipWidth > 1)
      mipWidth /= 2;
    if (mipHeight > 1)
      mipHeight /= 2;
  }

  barrier.subresourceRange.baseMipLevel = mipLevels - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &barrier);

  lveDevice.endSingleTimeCommands(commandBuffer);
}

} // namespace lve