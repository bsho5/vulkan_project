#pragma once
#include "buffer.hpp"
#include "device.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {
class LveModel {

public:
  struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};
    static std::vector<VkVertexInputBindingDescription> getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescription();
    bool operator==(const Vertex &other) const {
      return position == other.position && color == other.color &&
             normal == other.normal && uv == other.uv;
    }
  };

  LveModel();
  ~LveModel();
  LveModel(const LveModel &) = delete;
  LveModel &operator=(const LveModel &) = delete;
  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};
    void loadModel(const std::string &filepath);
  };

  LveModel(LveDevice &device, const LveModel::Builder &builder);
  static std::unique_ptr<LveModel>
  createModelFromFile(LveDevice &device, const std::string &filepath);
  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  void createVertexBuffers(const std::vector<Vertex> &vertices);
  void createIndexBuffers(const std::vector<uint32_t> &indices);

  LveDevice &device;
  std::unique_ptr<LveBuffer> vertexBuffer;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  std::unique_ptr<LveBuffer> indexBuffer;

  uint32_t indexCount;
};
} // namespace lve