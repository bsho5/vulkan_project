#include "app.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "lib/models/tiny_object.h"
#include "simple_render_system.hpp"

// std

#include <cassert>
#include <chrono>

// libs

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

struct GlobalUbo {
glm::mat4 projection{1.f};
  glm::mat4 view{1.f};
    glm::vec4 ambientLightColor{1.f, 1.f, 1.f, 0.002f}; // w is intensity
  glm::vec3 lightPosition {70.0f, -5.5f, 120.f};
  alignas(16) glm::vec4 lightColor{1.f};
  alignas(16) float t = 0.f;
};

App::App() {
  globalPool = LveDescriptorPool::Builder(device)
                   .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                SwapChain::MAX_FRAMES_IN_FLIGHT)
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

  auto globalSetLayout = LveDescriptorSetLayout::Builder(device)
                             .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                          VK_SHADER_STAGE_ALL_GRAPHICS)
                             .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(
      SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    LveDescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .build(globalDescriptorSets[i]);
  }

  SimpleRenderSystem simpleRenderSystem{
      device, renderer.getSwapChainRenderPass(),
      globalSetLayout->getDescriptorSetLayout()};
  // PointLightSystem pointLightSystem{
  //     device,
  //     renderer.getSwapChainRenderPass(),
  //     globalSetLayout->getDescriptorSetLayout()};
  LveCamera camera{};

  // camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f,
  // 0.f, 2.5f));
  auto viewerObject = GameObject::createGameObject();
  viewerObject.transform.translation.z = 100.f;
  viewerObject.transform.translation.x = 50.f;
  viewerObject.transform.translation.y = -10.f;

  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();
int x = 0;
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
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);

    if (auto commandBuffer = renderer.beginFrame()) {

      int frameIndex = renderer.getFrameIndex();
      FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera,
                          globalDescriptorSets[frameIndex]};

      GlobalUbo ubo{};
      ubo.projection = camera.getProjection();
      ubo.view = camera.getView();
      ubo.t = x *0.1f;
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

// temporary helper function, creates a 1x1x1 cube centered at offset with an
// index buffer

void App::loadGameObjects() {
  // std::shared_ptr<LveModel> lveModel =
  //         LveModel::createModelFromFile(device,
  //         "../engine/models/flat_vase.obj");
  // auto flatVase = GameObject::createGameObject();
  // flatVase.model = lveModel;
  // flatVase.transform.translation = {-.5f, .5f, 2.5f};
  // flatVase.transform.scale = {3.f, 1.5f, 3.f};
  // gameObjects.push_back(std::move(flatVase));

  // std::shared_ptr<LveModel> lveModel =LveModel::createModelFromFile(device, "../engine/models/40k.obj");
  // auto floor = GameObject::createGameObject();
  // floor.model = lveModel;
  // floor.transform.translation = {200.0f, 0.5f, 300.f};
  // floor.transform.scale = {200.f, 1.f, 200.f};

  // floor.color = {0.222,0.343,0.472};
  // gameObjects.push_back(std::move(floor));
  
  std::shared_ptr<LveModel> lveModel =LveModel::createModelFromFile(device, "../engine/models/40k.obj");
  auto floor = GameObject::createGameObject();
  floor.model = lveModel;
  floor.transform.translation = {200.0f, 0.5f, 300.f};
  floor.transform.scale = {200.f, 1.f, 200.f};

  floor.color = {0.222,0.343,0.472};
  gameObjects.push_back(std::move(floor));
  // lveModel =LveModel::createModelFromFile(device, "../engine/models/quad.obj");
//   auto light = GameObject::createGameObject();
//   //light.model = lveModel;
//   // quad.transform.translation = {.0f, .5f, 100.f};
// light.transform.scale = {10.f, 10.f, 10.f};
//   gameObjects.push_back(std::move(light));
      
lveModel =LveModel::createModelFromFile(device, "../engine/models/sphere.obj");
  auto sphere = GameObject::createGameObject();
  sphere.model = lveModel;
  // quad.transform.translation = {.0f, .5f, 100.f};
   sphere.transform.translation = {170.0f, -100.5f, 320.f};
     sphere.transform.rotation ={90.0f,0.f,0.0f};

sphere.transform.scale = {1.f, 1.f, 1.f};
  gameObjects.push_back(std::move(sphere));
      
//   // smoothVase.transform.scale = {3.f, 1.5f, 3.f};

  
  //  lveModel = LveModel::createModelFromFile(device, "../engine/models/smooth_vase.obj");
  // auto smoothVase = GameObject::createGameObject();
  // smoothVase.model = lveModel;
  // smoothVase.transform.scale = {50.f, 50.f, 50.f};
  // smoothVase.transform.rotation ={60.0f,22.5f,0.0f};
  //  smoothVase.transform.translation = {170.0f, -100.5f, 320.f};
  // gameObjects.push_back(std::move(smoothVase));
}

} // namespace lve
