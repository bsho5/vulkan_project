#pragma once
#include "descriptors.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>
#include "lve_texture.hpp"


namespace lve {
class App {
public:
  static constexpr int HEIGHT = 600;
  static constexpr int WIDTH = 800;

  // couldn't debug it

  App();
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  void run();

private:
  void loadGameObjects();


  Window window{WIDTH, HEIGHT, "vulkan window"};
  LveDevice device{window};
  Renderer renderer{window, device};

  std::unique_ptr<LveDescriptorPool> globalPool{};
  std::unique_ptr<Texture> texture{};
  std::unique_ptr<Texture> texture2{};
  std::vector<GameObject> gameObjects;
};
} // namespace lve