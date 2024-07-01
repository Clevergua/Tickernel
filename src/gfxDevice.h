#pragma once
// #include <standardLibrary.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#define TRY_THROW_VULKAN_ERROR(vkResult)             \
    if (vkResult != VK_SUCCESS)                      \
    {                                                \
        printf("Vulkan error code: %d\n", vkResult); \
    }

typedef struct GFXDeviceStruct
{
    // Config
    bool enableValidationLayers;
    char *name;
    int height;
    int width;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;
    uint32_t targetWaitFrameCount;
    uint32_t maxCommandBufferListCount;
    // Runtime
    FILE *logStream;

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

    VkImage depthImage;
    VkFormat depthFormat;
    VkImageView depthImageView;
    VkDeviceMemory depthImageMemory;

    VkCommandPool *vkCommandPools;
    uint32_t vkCommandBufferCount;
    VkCommandBuffer *vkCommandBuffers;

    uint32_t acquiredImageIndex;
    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *renderFinishedSemaphores;
    VkFence *renderFinishedFences;
    uint32_t waitFrameCount;
    uint32_t frameCount;
    uint32_t frameIndex;

    bool hasRecreateSwapchain;
} GFXDevice;

void StartGFXDevice(GFXDevice *pGFXDevice);
void UpdateGFXDevice(GFXDevice *pGFXDevice);
void EndGFXDevice(GFXDevice *pGFXDevice);