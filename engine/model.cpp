#include "model.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {
Model::Model(Device &device, const std::vector<Vertex> &vertices)
    : device{device} {
  createVertexBuffers(vertices);
};
Model::~Model() {
  vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
  vkFreeMemory(device.device(), vertexBufferMemory, nullptr);
}
void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
  device.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      vertexBuffer, vertexBufferMemory);
void *data;
vkMapMemory(device.device(), vertexBufferMemory, 0, bufferSize, 0,&data);
memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
vkUnmapMemory(device.device(),vertexBufferMemory);



}
void Model::draw(VkCommandBuffer commandBuffer){
    vkCmdDraw(commandBuffer, vertexCount,  1, 0 ,  0);
}
void Model::bind(VkCommandBuffer commandBuffer){
    VkBuffer buffers[]= {vertexBuffer};
    VkDeviceSize offsets[] ={0}; 
    vkCmdBindVertexBuffers(commandBuffer, 0,  1, buffers ,  offsets);
}
std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescription(){
    std::vector<VkVertexInputBindingDescription> bindingDescription(1);
    bindingDescription[0].binding =0;
        bindingDescription[0].stride = sizeof(Vertex);
            bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
}
std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescription(){
    std::vector<VkVertexInputAttributeDescription> attributeDescription(2);
    attributeDescription[0].binding =0;
        attributeDescription[0].location = 0;
            attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
                    attributeDescription[0].offset = offsetof(Vertex, postion);

     attributeDescription[1].binding =0;
        attributeDescription[1].location = 1;
            attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                    attributeDescription[1].offset = offsetof(Vertex, color);

            return attributeDescription;
}
} // namespace lve