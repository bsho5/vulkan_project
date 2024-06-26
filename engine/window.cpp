#include "window.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>



namespace lve{

    Window::Window(int width,int height,std::string windowName) : width(width), height(height), windowName(windowName){
        initWindow();
    };

    Window::~Window(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    
    void Window::initWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow( width, height, windowName.c_str(), nullptr, nullptr);
         glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface){
        if (glfwCreateWindowSurface( instance, window,nullptr, surface)) {
        throw std::runtime_error("failed to create window surface");
        }
    }

    void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
  auto lveWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  lveWindow->framebufferResized = true;
  lveWindow->width = width;
  lveWindow->height = height;
}


}