#pragma once

#include "device.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {
class LveModel {

public:
  struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{};
    static std::vector<VkVertexInputBindingDescription> getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescription();
  };

  LveModel();
  ~LveModel();
  LveModel(const LveModel &) = delete;
  LveModel &operator=(const LveModel &) = delete;
struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};
  };

  LveModel(Device &device, const LveModel::Builder &builder);  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  void createVertexBuffers(const std::vector<Vertex> &vertices);
  void createIndexBuffers(const std::vector<uint32_t> &indices);

  Device &device;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  uint32_t indexCount;

};
} // namespace lve