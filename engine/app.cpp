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
  glm::vec3 lightPosition{200.0, -100.0, 500.0};
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
      simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
      // pointLightSystem.render(frameInfo);

      renderer.endSwapChainRenderPass(commandBuffer);
      renderer.endFrame();
    }
  }
  vkDeviceWaitIdle(device.device());
}


void App::loadGameObjects() {
  
  std::shared_ptr<LveModel> lveModel =
      LveModel::createModelFromFile(device, "../engine/models/plane.obj");
  auto floor = GameObject::createGameObject();
  floor.model = lveModel;
  floor.transform.translation = {0.0f, 0.0f, 0.0f};
  floor.transform.scale = {50.f, 1.f, 50.f};
 // floor.transform.rotation.x = 20.f;
  //floor.transform.rotation.z = 20.f;
  floor.color = {0.222, 0.343, 0.472};
  gameObjects.push_back(std::move(floor));

  lveModel =
      LveModel::createModelFromFile(device, "../engine/models/plane.obj");
  auto floor2 = GameObject::createGameObject();
  floor2.model = lveModel;
  floor2.transform.translation = {100.0f, 0.0f, 0.f};
  floor2.transform.scale = {50.f, 1.f, 50.f};
  //floor2.transform.rotation.x = 20.f;
  //floor2.transform.rotation.z = 20.f;
  gameObjects.push_back(std::move(floor2));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor3 =
  // GameObject::createGameObject(); floor3.model = lveModel;
  // floor3.transform.translation = {200.0f, 0.0f, 0.f};
  // floor3.transform.scale = {5.f, 1.f,5.f};
  // floor3.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor3));

  //  lveModel =LveModel::createModelFromFile(device,
  //  "../engine/models/untitled2.obj");
  // auto floor4 = GameObject::createGameObject();
  // floor4.model = lveModel;
  // floor4.transform.translation = {300.0f, 0.0f, 0.f};
  // floor4.transform.scale = {5.f, 1.f,5.f};
  // floor4.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor4));

  lveModel =
      LveModel::createModelFromFile(device, "../engine/models/plane.obj");
  auto floor5 = GameObject::createGameObject();
  floor5.model = lveModel;
  floor5.transform.translation = {0.0f, 0.0f, 100.f};
  floor5.transform.scale ={50.f, 1.f, 50.f};
  floor5.color = {0.222, 0.343, 0.472};
  gameObjects.push_back(std::move(floor5));

  lveModel =
      LveModel::createModelFromFile(device, "../engine/models/plane.obj");
  auto floor6 = GameObject::createGameObject();
  floor6.model = lveModel;
  floor6.transform.translation = {100.0f, 0.f, 100.f};
  floor6.transform.scale = {50.f, 1.f, 50.f};
  gameObjects.push_back(std::move(floor6));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor7 =
  // GameObject::createGameObject(); floor7.model = lveModel;
  // floor7.transform.translation = {200.0f, 0.0f, 100.f};
  // floor7.transform.scale = {5.f, 1.f,5.f};
  // floor7.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor7));

  //  lveModel =LveModel::createModelFromFile(device,
  //  "../engine/models/untitled2.obj");
  // auto floor8 = GameObject::createGameObject();
  // floor8.model = lveModel;
  // floor8.transform.translation = {300.0f, 0.0f, 100.f};
  // floor8.transform.scale = {5.f, 1.f,5.f};
  // floor8.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor8));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor9 =
  // GameObject::createGameObject(); floor9.model = lveModel;
  // floor9.transform.translation = {0.0f, 0.5f, 200.f};
  // floor9.transform.scale = {5.f, 1.f,5.f};
  // floor9.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor9));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor10 =
  // GameObject::createGameObject(); floor10.model = lveModel;
  // floor10.transform.translation = {100.0f, 0.0f, 200.f};
  // floor10.transform.scale = {5.f, 1.f,5.f};
  // gameObjects.push_back(std::move(floor10));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor11 =
  // GameObject::createGameObject(); floor11.model = lveModel;
  // floor11.transform.translation = {200.0f, 0.0f, 200.f};
  // floor11.transform.scale = {5.f, 1.f,5.f};
  // floor11.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor11));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor12 =
  // GameObject::createGameObject(); floor12 .model = lveModel; floor12
  // .transform.translation = {300.0f, 0.0f, 200.f}; floor12 .transform.scale =
  // {5.f, 1.f,5.f}; floor12.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor12 ));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor13 =
  // GameObject::createGameObject(); floor13.model = lveModel;
  // floor13.transform.translation = {0.0f, 0.5f, 300.f};
  // floor13.transform.scale = {5.f, 1.f,5.f};
  // floor13.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor13));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor14 =
  // GameObject::createGameObject(); floor14.model = lveModel;
  // floor14.transform.translation = {100.0f, 0.0f, 300.f};
  // floor14.transform.scale = {5.f, 1.f,5.f};
  // gameObjects.push_back(std::move(floor14));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor15 =
  // GameObject::createGameObject(); floor15.model = lveModel;
  // floor15.transform.translation = {200.0f, 0.0f, 300.f};
  // floor15.transform.scale = {5.f, 1.f,5.f};
  // floor15.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor15));

  // lveModel =LveModel::createModelFromFile(device,
  // "../engine/models/untitled2.obj"); auto floor16 =
  // GameObject::createGameObject(); floor16.model = lveModel;
  // floor16.transform.translation = {300.0f, 0.0f, 300.f};
  // floor16.transform.scale = {5.f, 1.f,5.f};
  // floor16.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor16));

  lveModel =
      LveModel::createModelFromFile(device, "../engine/models/sphere.obj");
  auto sphere = GameObject::createGameObject();
  sphere.model = lveModel;

  // sphere.transform.translation = {200.0, -100.0, 500.0};
  // sphere.transform.scale = {2.0, 2.0, 2.0};

  sphere.color = glm::vec3(1.f, 1.f, 1.f);
  gameObjects.push_back(std::move(sphere));
  //   lveModel =
  //     LveModel::createModelFromFile(device, "../engine/models/untitled2.obj");
  // auto quad = GameObject::createGameObject();
  // quad.model = lveModel;
  // quad.transform.translation = {100.0f, -300.0f, 100.f};
  // quad.transform.scale = {20.f, 20.f, 20.f};
  // quad.transform.rotation = {90.f,0.f,0.f};
  // quad.color = glm::vec3(1.f, 1.f, 1.f);
  // gameObjects.push_back(std::move(quad));
}

} // namespace lve
