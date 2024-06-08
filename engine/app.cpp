#include "app.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "lib/models/tiny_object.h"
#include "camera.hpp"



// std


#include <cassert>
#include <chrono>



//libs

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

 
namespace lve {



App::App() {  loadGameObjects();}
App::~App(){}
void App::run() {
  SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};
    LveCamera camera{};

    //camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));
auto viewerObject = GameObject::createGameObject();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!window.shouldClose()) {
   
   
    glfwPollEvents();
        auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = renderer.getAspectRatio();
    // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);


     if (auto commandBuffer = renderer.beginFrame()) {
       
      renderer.beginSwapChainRenderPass(commandBuffer);
simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      renderer.endSwapChainRenderPass(commandBuffer);
      renderer.endFrame();
      
    }
   
  }
  vkDeviceWaitIdle(device.device());
}


// temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer


void App::loadGameObjects() {
  std::shared_ptr<LveModel> lveModel =
      LveModel::createModelFromFile(device, "../engine/models/smooth_vase.obj");
  auto gameObj = GameObject::createGameObject();
  gameObj.model = lveModel;
  gameObj.transform.translation = {.0f, .0f, 2.5f};
  gameObj.transform.scale = glm::vec3(3.f);
  gameObjects.push_back(std::move(gameObj));


}







} // namespace lve
