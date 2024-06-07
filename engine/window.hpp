#pragma once 
#include <string>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lve {
class Window{

public:

    Window(int height, int width, std::string windowName);
    Window (const Window &) = delete;
    Window &operator=(const Window &)=delete;
    ~Window();

    bool shouldClose (){return glfwWindowShouldClose(window);}
    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
    bool wasWindowResized() { return framebufferResized; }
  void resetWindowResizedFlag() { framebufferResized = false; }
   GLFWwindow *getGLFWwindow() const { return window; }
private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    void initWindow();
   int width;
  int height;
  bool framebufferResized = false;
    std::string windowName;
    GLFWwindow *window;
};
}

