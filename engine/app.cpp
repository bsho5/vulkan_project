#include "app.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "glm/fwd.hpp"
#include "keyboard_movement_controller.hpp"
#include "lib/models/tiny_object.h"
#include "lve_cubemap.hpp"

#include "lve_texture.hpp"
#include "simple_render_system.hpp"

// std

#include <cassert>
#include <chrono>

#include <vulkan/vulkan_core.h>


// libs

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

struct GlobalUbo {
  glm::mat4 projection{1.f};
  glm::mat4 view{1.f};
  glm::vec4 ambientLightColor{1.f, 1.f, 1.f, 0.05f}; // w is intensity
  glm::vec3 lightPosition{800.0, -500.0, 1500.0};
  glm::mat4 invViewMatrix{1.0f};
  glm::vec3 cameraPosition{0.0f};
  alignas(16) glm::vec4 lightColor{1.f};
  alignas(16) float t = 0.f;
};

App::App() {
  globalPool = LveDescriptorPool::Builder(device)
                   .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                SwapChain::MAX_FRAMES_IN_FLIGHT)
                   .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                   .build();
  loadGameObjects();
}
App::~App() {}
void App::run() {
  std::vector<std::unique_ptr<LveBuffer>> uboBuffers(
      SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<LveBuffer>(
        device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uboBuffers[i]->map();
  }

  cubemap = std::make_unique<CubeMap>(device);
  
  VkDescriptorImageInfo imageInfos[2] ={};

  VkDescriptorImageInfo cubemapImageInfo = {};
  cubemapImageInfo.sampler = cubemap->getSampler();
  cubemapImageInfo.imageView = cubemap->getImageView();
  cubemapImageInfo.imageLayout = cubemap->getImageLayout();

 
  // texture = std::make_unique<Texture>(device,"../engine/textures/moon3.png");
  
  
  // VkDescriptorImageInfo imageInfo = {};
  // imageInfo.sampler = texture->getSampler();
  // imageInfo.imageView = texture->getImageView();
  // imageInfo.imageLayout = texture->getImageLayout();
 

  auto globalSetLayout = LveDescriptorSetLayout::Builder(device)
                             .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         VK_SHADER_STAGE_ALL_GRAPHICS)
                             .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)                        

                             .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(
      SwapChain::MAX_FRAMES_IN_FLIGHT);
     
  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    LveDescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
         .writeImage(1, &cubemapImageInfo)
        // .writeImage(2, &imageInfo2)
        .build(globalDescriptorSets[i]);
  }
 SimpleRenderSystem simpleRenderSystem{
      device, renderer.getSwapChainRenderPass(),
      globalSetLayout->getDescriptorSetLayout()};

  LveCamera camera{};

  int x = 0;
  auto viewerObject = GameObject::createGameObject();

  // viewerObject.transform.translation.y = -200.f;
  // viewerObject.transform.translation.x = -50.f;

  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!window.shouldClose()) {
    x++;

    glfwPollEvents();
    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime,
                                   viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = renderer.getAspectRatio();
    // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10000.f);

    if (auto commandBuffer = renderer.beginFrame()) {

      int frameIndex = renderer.getFrameIndex();
      FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera,
                          globalDescriptorSets[frameIndex]};

      GlobalUbo ubo{};
      ubo.projection = camera.getProjection();
      ubo.view = camera.getView();
      ubo.cameraPosition = viewerObject.transform.translation;
      ubo.invViewMatrix = camera.getInverseView();
      ubo.t = x * 0.1f;
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      // render
      renderer.beginSwapChainRenderPass(commandBuffer);
    
     simpleRenderSystem.renderSkyBox(frameInfo);
     simpleRenderSystem.renderOcean(frameInfo);
     simpleRenderSystem.renderMoon(frameInfo);
      

      renderer.endSwapChainRenderPass(commandBuffer);
      renderer.endFrame();
    }
  }
  vkDeviceWaitIdle(device.device());
}


void App::loadGameObjects() {}

} // namespace lve
