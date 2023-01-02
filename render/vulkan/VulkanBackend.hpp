//
// Created by f0xeri on 31.12.2022.
//

#ifndef VULKAN_EXPERIMENTS_VULKANBACKEND_HPP
#define VULKAN_EXPERIMENTS_VULKANBACKEND_HPP

#include <vulkan/vulkan.h>
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#include <ranges>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <deque>
#include <functional>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/ext.hpp>
#include "vk_mem_alloc.h"
#include "VulkanMesh.hpp"
#include "VulkanShader.hpp"

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto & deletor : std::ranges::reverse_view(deletors)) {
            deletor(); //call the function
        }

        deletors.clear();
    }
};

struct AllocatedImage {
    VkImage image;
    VmaAllocation allocation;
};

class VulkanBackend {
private:
    uint64_t frameNumber = 0;

    VkInstance instance;
    VkSurfaceKHR surface;

    VkExtent2D windowExtent;

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSwapchainKHR swapchain;

    VkFormat swapchainImageFormat;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    VkQueue graphicsQueue;
    uint32_t graphicsQueueFamilyIndex;

    VkCommandPool commandPool;
    VkCommandBuffer mainCommandBuffer;

    VkRenderPass renderPass;
    std::vector<VkFramebuffer> framebuffers;

    VkSemaphore renderSemaphore, presentSemaphore;
    VkFence renderFence;

    VkPipeline trianglePipeline;
    VkPipelineLayout trianglePipelineLayout;

    VmaAllocator allocator;

    VulkanMesh triangleMesh;

    VkImageView depthImageView;
    AllocatedImage depthImage;
    VkFormat depthFormat;

    std::unique_ptr<ShaderLoader> shaderLoader = nullptr;

    DeletionQueue deletionQueue;
public:
    std::vector<VulkanMesh> meshes;
    Shader shader;
    void addMesh(const Mesh &mesh);
    ShaderLoader* getShaderLoader();
    void createShader(const Shader& info);
    bool windowResized = false;

    VulkanBackend(const std::shared_ptr<GLFWwindow>& window, std::string_view appName, uint32_t width, uint32_t height);
    void init(uint32_t width, uint32_t height);
    void createSwapchain(uint32_t width, uint32_t height);
    void createPhysicalDevice();
    void createLogicalDevice();
    void createQueue();
    void createCommandPoolAndBuffer();
    void createDefaultRenderPass();
    void createFramebuffers();
    void createSemaphoresAndFences();
    void drawFrame();

    void loadShaderModule(std::string_view path, VkShaderModule* outShaderModule);
    void createPipelines();
    void setWindowExtent(uint32_t width, uint32_t height) {
        windowExtent = { width, height };
    }
    void recreateSwapchain(uint32_t width, uint32_t height);
    ~VulkanBackend();

    void loadMeshes();
    void uploadMesh(VulkanMesh& mesh);

    VkImageCreateInfo createImageInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
    VkImageViewCreateInfo createImageViewInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
};

#endif //VULKAN_EXPERIMENTS_VULKANBACKEND_HPP
