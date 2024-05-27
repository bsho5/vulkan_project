#include "app.hpp"

// std

#include <array>
#include <stdexcept>
#include <cassert>

//libs

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

 
namespace lve {
void App::run() {
  while (!window.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }
  vkDeviceWaitIdle(device.device());
}

// sierpinski triangle demo

// void App::sierpinski(
//     std::vector<Model::Vertex> &vertices,
//     int depth,
//     glm::vec2 left,
//     glm::vec2 right,
//     glm::vec2 top) {
//   if (depth <= 0) {
//     vertices.push_back({top});
//     vertices.push_back({right});
//     vertices.push_back({left});
//   } else {
//     auto leftTop = 0.5f * (left + top);
//     auto rightTop = 0.5f * (right + top);
//     auto leftRight = 0.5f * (left + right);
//     sierpinski(vertices, depth - 1, left, leftRight, leftTop);
//     sierpinski(vertices, depth - 1, leftRight, right, rightTop);
//     sierpinski(vertices, depth - 1, leftTop, rightTop, top);
//   }
// }
// void App::loadModels() {
//   std::vector<Model::Vertex> vertices{};
//   sierpinski(vertices, 5, {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.0f, -0.5f});
//   model = std::make_unique<Model>(device, vertices);
// }


// 40 triangles demo

// void App::loadGameObjects() {
//   std::vector<Model::Vertex> vertices{
//       {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//       {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//       {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
//   auto lveModel = std::make_shared<Model>(device, vertices);
    
//   // https://www.color-hex.com/color-palette/5361
//   std::vector<glm::vec3> colors{
//       {1.f, .7f, .73f},
//       {1.f, .87f, .73f},
//       {1.f, 1.f, .73f},
//       {.73f, 1.f, .8f},
//       {.73, .88f, 1.f}  //
//   };
//   for (auto& color : colors) {
//     color = glm::pow(color, glm::vec3{2.2f});
//   }
//   for (int i = 0; i < 40; i++) {
//     auto triangle = GameObject::createGameObject();
//     triangle.model = lveModel;
//     triangle.transform2d.scale = glm::vec2(.5f) + i * 0.025f;
//     triangle.transform2d.rotation = i * glm::pi<float>() * .025f;
//     triangle.color = colors[i % colors.size()];
//     gameObjects.push_back(std::move(triangle));
//   }
// }




void App::loadGameObjects(){
  std::vector<Model::Vertex>vertices{
{{0.0f,-0.5f},{1.0f,0.0f,0.0f}},
{{0.5f,0.5f},{0.0f,1.0f,0.0f}},
{{-0.5f,0.5f},{0.0f,0.0f,1.0f}}
  };
 auto  model =std::make_shared<Model>(device, vertices);
  auto triangle = GameObject::createGameObject();
  triangle.model = model;
  triangle.color = {.1f, .8f, .1f};
  triangle.transform2d.translation.x = .2f;
  triangle.transform2d.scale = {2.f, .5f};
  triangle.transform2d.rotation = .25f * glm::two_pi<float>();

  gameObjects.push_back(std::move(triangle));
}

struct SimplePushConstantData {
  glm::mat2 transform{1.f};
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};


App::App(){
    loadGameObjects();
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
}

App::~App(){
vkDestroyPipelineLayout( device.device(),  pipelineLayout, nullptr);
}

void App::recreateSwapChain() {
  auto extent = window.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = window.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(device.device());

  if (swapChain == nullptr) {
    swapChain = std::make_unique<SwapChain>(device, extent);
  } else {
    swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));
    if (swapChain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }

  createPipeline();
}


void App::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void App::createPipeline(){
   assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
    
PipelineConfigInfo configInfo{};
     Pipeline::defaultPipelineConfigInfo(configInfo);
  configInfo.renderPass = swapChain->getRenderPass();
    configInfo.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(
        device,"../engine/shaders/shader.vert.spv",
        "../engine/shaders/shader.frag.spv",
        configInfo
        
    );
}

void App::createCommandBuffers(){
  commandBuffers.resize(swapChain->imageCount());
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = device.getCommandPool();
  allocInfo.commandBufferCount =  static_cast<uint32_t>(commandBuffers.size());
  if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data())!= VK_SUCCESS) {
  throw std::runtime_error("failed to allocate command buffers!");
  }
 

  
  
};
void App::recordCommandBuffer(int imageIndex) {


  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    
  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
   VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = swapChain->getRenderPass();
  renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
  viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  renderGameObjects(commandBuffers[imageIndex]);

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

// 40 triangles demo

// void App::renderGameObjects(VkCommandBuffer commandBuffer) {
//   // update
//   int i = 0;
//   for (auto& obj : gameObjects) {
//     i += 1;
//     obj.transform2d.rotation =
//         glm::mod<float>(obj.transform2d.rotation + 0.001f * i, 2.f * glm::pi<float>());
//   }
//    pipeline->bind(commandBuffer);
//   for (auto& obj : gameObjects) {
//     SimplePushConstantData push{};
//     push.offset = obj.transform2d.translation;
//     push.color = obj.color;
//     push.transform = obj.transform2d.mat2();
 
//     vkCmdPushConstants(
//         commandBuffer,
//         pipelineLayout,
//         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
//         0,
//         sizeof(SimplePushConstantData),
//         &push);
//     obj.model->bind(commandBuffer);
//     obj.model->draw(commandBuffer);
//   }
// }

void App::renderGameObjects(VkCommandBuffer commandBuffer) {
  pipeline->bind(commandBuffer);

  for (auto& obj : gameObjects) {
    obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

  
    SimplePushConstantData push{};
     push.offset = obj.transform2d.translation;
    push.color = obj.color;
    push.transform = obj.transform2d.mat2();
    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &push);
       obj.model->bind(commandBuffer);
    obj.model->draw(commandBuffer);

  }

  
  }
void App::freeCommandBuffers() {
  vkFreeCommandBuffers(
      device.device(),
      device.getCommandPool(),
      static_cast<uint32_t>(commandBuffers.size()),
      commandBuffers.data());
  commandBuffers.clear();
}
void App:: drawFrame(){
  uint32_t imageIndex;
 auto result = swapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }  if (result!= VK_SUCCESS && result !=VK_SUBOPTIMAL_KHR) {
  throw std::runtime_error("failed to acquire swap chain image!");
  }

   recordCommandBuffer(imageIndex);
  result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window.wasWindowResized()) {
    window.resetWindowResizedFlag();
    recreateSwapChain();
 
  }

};

} // namespace lve
