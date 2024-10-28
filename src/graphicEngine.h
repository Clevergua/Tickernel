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
    VkViewport viewport;
    VkRect2D scissor;
    uint32_t height;
    uint32_t width;
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
    // VkExtent2D swapchainExtent;
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

    GraphicImage depthGraphicImage;
    GraphicImage albedoGraphicImage;
    GraphicImage normalGraphicImage;
    RenderPass deferredRenderPass;
    uint32_t fullScreenTriangleModelIndex;

    vec3 cameraPosition;
    vec3 targetPosition;
} GraphicEngine;
void StartGraphicEngine(GraphicEngine *pGraphicEngine);
void UpdateGraphicEngine(GraphicEngine *pGraphicEngine, bool *pCanUpdate);
void EndGraphicEngine(GraphicEngine *pGraphicEngine);