#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#include <tickernelCore.h>
#include <cglm.h>
#define INVALID_VKFRAMEBUFFER 0

typedef struct GlobalUniformBufferObjectStruct
{
    // mat4 model;
    mat4 view;
    mat4 proj;
} GlobalUniformBufferObject;

typedef struct RenderPipelineStruct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    uint32_t vkPipelineCount;
    VkPipeline *vkPipelines;
    VkPipelineLayout *vkPipeline2Layout;
    VkDescriptorSetLayout *vkPipeline2DescriptorSetLayout;

    VkDescriptorPool vkDescriptorPool;
    VkDescriptorSet **vkPipeline2DescriptorSets;
} RenderPipeline;

typedef struct GraphicImageStruct
{
    VkImage vkImage;
    VkFormat vkFormat;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
} GraphicImage;

typedef struct GraphicEngineStruct
{
    // Config
    bool enableValidationLayers;
    char *name;
    int height;
    int width;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;

    // Runtime
    GLFWwindow *pGLFWWindow;
    VkInstance vkInstance;
    VkSurfaceKHR vkSurface;
    VkPhysicalDevice vkPhysicalDevice;
    uint32_t queueFamilyPropertyCount;
    uint32_t graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    VkDevice vkDevice;
    VkQueue vkGraphicQueue;
    VkQueue vkPresentQueue;
    VkSwapchainKHR vkSwapchain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D swapchainExtent;
    uint32_t swapchainImageCount;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;

    VkCommandPool graphicVkCommandPool;
    VkCommandBuffer *graphicVkCommandBuffers;
    uint32_t acquiredImageIndex;
    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *renderFinishedSemaphores;
    VkFence *renderFinishedFences;
    uint32_t frameCount;
    uint32_t frameIndex;
    bool hasRecreatedSwapchain;

    VkBuffer *globalUniformBuffers;
    VkDeviceMemory *globalUniformBufferMemories;
    void **globalUniformBuffersMapped;

    GraphicImage depthGraphicImage;
    GraphicImage albedoGraphicImage;
    RenderPipeline deferredRenderPipeline;
    RenderPipeline uiRenderPipeline;

} GraphicEngine;

void TryThrowVulkanError(VkResult vkResult);
void FindMemoryType(GraphicEngine *pGraphicEngine, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex);
void FindDepthFormat(GraphicEngine *pGraphicEngine, VkFormat *pDepthFormat);

void CreateImageView(GraphicEngine *pGraphicEngine, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView);
void CreateGraphicImage(GraphicEngine *pGraphicEngine, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage);
void DestroyGraphicImage(GraphicEngine *pGraphicEngine, GraphicImage graphicImage);
void CreateVkShaderModule(GraphicEngine *pGraphicEngine, const char *filePath, VkShaderModule *pVkShaderModule);
void DestroyVkShaderModule(GraphicEngine *pGraphicEngine, VkShaderModule vkShaderModule);
