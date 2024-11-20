#pragma once
#include <deferredRenderPass.h>
typedef struct GraphicEngineStruct
{
    // Config
    bool enableValidationLayers;
    char *name;
    int windowHeight;
    int windowWidth;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;
    char *assetsPath;

    // Runtime
    uint32_t swapchainHeight;
    uint32_t swapchainWidth;
    TickernelWindow *pTickernelWindow;
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
    uint32_t swapchainImageCount;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;

    VkCommandPool graphicVkCommandPool;
    VkCommandBuffer *graphicVkCommandBuffers;
    uint32_t acquiredImageIndex;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;
    uint32_t frameCount;
    uint32_t frameIndex;

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

    GraphicImage depthGraphicImage;
    GraphicImage albedoGraphicImage;
    GraphicImage normalGraphicImage;
    DeferredRenderPass deferredRenderPass;
    uint32_t fullScreenTriangleModelIndex;
} GraphicEngine;
void StartGraphicEngine(GraphicEngine *pGraphicEngine);
void UpdateGraphicEngine(GraphicEngine *pGraphicEngine, bool *pCanUpdate);
void EndGraphicEngine(GraphicEngine *pGraphicEngine);