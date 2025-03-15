#pragma once
#include "deferredRenderPass.h"
#include "postProcessRenderPass.h"
#include "uiRenderPass.h"
typedef struct
{
    const char *assetsPath;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;
    VkInstance vkInstance;
    VkSurfaceKHR vkSurface;

    uint32_t swapchainHeight;
    uint32_t swapchainWidth;
    VkPhysicalDevice vkPhysicalDevice;
    uint32_t queueFamilyPropertyCount;
    uint32_t graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    VkDevice vkDevice;
    VkQueue vkGraphicQueue;
    VkQueue vkPresentQueue;
    VkSwapchainKHR vkSwapchain;
    VkSurfaceFormatKHR surfaceFormat;
    uint32_t swapchainImageCount;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;
    VkCommandPool graphicVkCommandPool;
    VkCommandBuffer *graphicVkCommandBuffers;
    uint32_t swapchainIndex;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;
    VkBuffer globalUniformBuffer;
    VkDeviceMemory globalUniformBufferMemory;
    void *globalUniformBufferMapped;
    VkBuffer lightsUniformBuffer;
    VkDeviceMemory lightsUniformBufferMemory;
    void *lightsUniformBufferMapped;
    bool canUpdateGlobalUniformBuffer;
    bool canUpdateLightsUniformBuffer;
    GlobalUniformBuffer inputGlobalUniformBuffer;
    LightsUniformBuffer inputLightsUniformBuffer;
    GraphicImage colorGraphicImage;
    GraphicImage depthGraphicImage;
    GraphicImage albedoGraphicImage;
    GraphicImage normalGraphicImage;
    DeferredRenderPass deferredRenderPass;
    PostProcessRenderPass postProcessRenderPass;
    UIRenderPass uiRenderPass;
    uint32_t fullScreenTriangleModelIndex;
} GraphicContext;

GraphicContext *startGraphic(const char *assetsPath, int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight);
void updateGraphic(GraphicContext *pGraphicContext, uint32_t swapchainWidth, uint32_t swapchainHeight);
void endGraphic(GraphicContext *pGraphicContext);
