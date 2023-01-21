//
// Created by f0xeri on 30.12.2022.
//

#ifndef VULKAN_EXPERIMENTS_APPLICATION_HPP
#define VULKAN_EXPERIMENTS_APPLICATION_HPP

#include <memory>
#include <GLFW/glfw3.h>
#include "render/vulkan/VulkanBackend.hpp"

struct CameraData {
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 modelView;
    glm::vec4 lightPos = glm::vec4(-48.0f, -40.0f, 46.0f, 0.0f);
    glm::vec4 frustumPlanes[6];
    float displacementFactor = 32.0f;
    float tessellationFactor = 0.75f;
    glm::vec2 viewportDim;
    float tessellatedEdgeSize = 20.0f;
};

class Application {
private:
    std::shared_ptr<GLFWwindow> mainWindow;
    std::unique_ptr<VulkanBackend> vulkanBackend;
public:
    int width;
    int height;
    Application(int width, int height, const char* title);
    ~Application();
    void initScene();
    void initPipelines();
    void run();
};


#endif //VULKAN_EXPERIMENTS_APPLICATION_HPP
