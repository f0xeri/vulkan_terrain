//
// Created by f0xeri on 30.12.2022.
//
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "Application.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Frustum.hpp"
#include "HeightMap.hpp"

Application::Application(int width, int height, const char* title) {
    this->width = width;
    this->height = height;
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwVulkanSupported();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    mainWindow = std::shared_ptr<GLFWwindow>(glfwCreateWindow(width, height, title, nullptr, nullptr), [](GLFWwindow* window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    });
    if (!mainWindow) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(mainWindow.get());
    vulkanBackend = std::make_unique<VulkanBackend>(mainWindow, "Vulkan Experiments", width, height);
    glfwSetWindowUserPointer(mainWindow.get(), this);
    glfwSetFramebufferSizeCallback(mainWindow.get(), [](GLFWwindow* window, int width, int height) {
        auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->vulkanBackend->setWindowExtent(width, height);
        app->vulkanBackend->recreateSwapchain(width, height);
        app->width = width;
        app->height = height;
    });
    initScene();
    vulkanBackend->init(width, height);
    initPipelines();
}

Application::~Application() {
    glfwTerminate();
    vulkanBackend.reset();
    mainWindow.reset();
}

void Application::initScene() {
    Shader shader = {};
    shader.name = "default";
    shader.stagesInfo.push_back(vulkanBackend->getShaderLoader()->loadFromBinaryFile("assets/shaders/triangle.vert.spv", ShaderStage::VERTEX));
    shader.stagesInfo.push_back(vulkanBackend->getShaderLoader()->loadFromBinaryFile("assets/shaders/triangle.tesc.spv", ShaderStage::TESSELLATION_CONTROL));
    shader.stagesInfo.push_back(vulkanBackend->getShaderLoader()->loadFromBinaryFile("assets/shaders/triangle.tese.spv", ShaderStage::TESSELLATION_EVALUATION));
    shader.stagesInfo.push_back(vulkanBackend->getShaderLoader()->loadFromBinaryFile("assets/shaders/triangle.frag.spv", ShaderStage::FRAGMENT));
    shader.descriptorBinding = DescriptorBinding();
    shader.descriptorBinding.addUniform(0,
                                        "cameraBuffer",
                                        UniformType::UNIFORM_BUFFER,
                                        sizeof(CameraData),
                                        {ShaderStage::VERTEX, ShaderStage::TESSELLATION_CONTROL, ShaderStage::TESSELLATION_EVALUATION});
    shader.constants.push_back({"modelBuffer", sizeof(glm::mat4), 0, {ShaderStage::VERTEX}});
    vulkanBackend->createShader(shader);

    Texture texture("heightmap");
    texture.loadTextureFromFile("assets/clarityi.png");
    HeightMap heightMap(&texture, 64);
    Mesh terrainMesh = Mesh::generateTerrainPatch(64, heightMap);
    vulkanBackend->addMesh("terrain", terrainMesh);
}

void Application::run() {
    glm::vec3 camPos = { 0.f,-20.0f,-100.f };
    Frustum frustum;
    while (!glfwWindowShouldClose(mainWindow.get())) {
        glfwPollEvents();
        vulkanBackend->beginFrame();

        glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
        glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)width / height, 0.1f, 2000.0f);
        projection[1][1] *= -1;

        vulkanBackend->bindPipeline("default");
        vulkanBackend->bindDescriptorSets();

        CameraData cameraData;
        cameraData.view = view;
        cameraData.projection = projection;
        //cameraData.lightPos.y = -0.5f - cameraData.displacementFactor;
        cameraData.viewportDim = glm::vec2(width, height);

        for (auto &mesh : vulkanBackend->meshes) {
            glm::mat4 model = glm::rotate(mesh.second.model, glm::radians(vulkanBackend->frameNumber * 0.4f), glm::vec3(0, 1, 0));
            cameraData.modelView = view * model;
            frustum.update(projection * cameraData.modelView);
            memcpy(cameraData.frustumPlanes, frustum.planes.data(), sizeof(glm::vec4) * 6);
            vulkanBackend->setUniformBuffer("cameraBuffer", &cameraData, sizeof(CameraData));
            vulkanBackend->pushConstants(&model, sizeof(glm::mat4), ShaderStage::VERTEX);
            vulkanBackend->drawMeshIndexed(mesh.second);
        }
        vulkanBackend->endFrame();
    }
}

void Application::initPipelines() {
    Texture defaultTexture("defaultTexture");
    defaultTexture.loadTextureFromFile("assets/clarityi.png");
    vulkanBackend->addTexture(defaultTexture, 0);

    TextureArray terrainTexture("terrainTexture");
    terrainTexture.loadTexturesFromFiles({"assets/terrainTextures/1.png",
                                          "assets/terrainTextures/2.png",
                                          "assets/terrainTextures/3.png",
                                          "assets/terrainTextures/4.png",
                                          "assets/terrainTextures/5.png",
                                          "assets/terrainTextures/6.png"});
    vulkanBackend->addTexture(terrainTexture, 1);

    vulkanBackend->createDescriptors(vulkanBackend->shaders["default"]);
    vulkanBackend->createGraphicsPipeline("default", vulkanBackend->shaders["default"]);
}
