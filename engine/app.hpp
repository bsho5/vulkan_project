#pragma once
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

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

  std::vector<GameObject> gameObjects;
};
} // namespace lve