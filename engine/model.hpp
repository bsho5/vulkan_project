#pragma once

#include "device.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {
class Model {

public:
  struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{};
    static std::vector<VkVertexInputBindingDescription> getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescription();
  };

  Model();
  ~Model();
  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;
  Model(Device &device, const std::vector<Vertex> &vertices);
  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  void createVertexBuffers(const std::vector<Vertex> &vertices);

  Device &device;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;
};
} // namespace lve