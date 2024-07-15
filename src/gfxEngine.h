#pragma once
// #include <standardLibrary.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#include <tickernelCore.h>

typedef enum
{
    ColorAttachmentType,
    DepthAttachmentType,
    StencilAttachmentType,
    PositionAttachmentType,
    NormalAttachmentType,
    AlbedoAttachmentType,
    CustomAttachmentType,
} AttachmentType;

typedef struct GFXCommandCreateInfoStruct
{
    uint32_t vkAttachmentCount;
    const VkAttachmentDescription *vkAttachmentDescriptions;
    uint32_t vkSubpassDescriptionCount;
    const VkSubpassDescription *vkSubpassDescriptions;
    uint32_t vkSubpassDependencyCount;
    const VkSubpassDependency *vkSubpassDependencies;
    AttachmentType *attachmentTypes;
    uint32_t width;
    uint32_t height;
    uint32_t layers;
    VkRect2D renderArea;
    uint32_t vkClearValueCount;
    VkClearValue *vkClearValues;

    uint32_t vkShaderModuleCreateInfoCount;
    size_t *codeSizeArray;
    uint32_t **codeArray;
    char **codeFunctionNameArray;
    VkShaderStageFlagBits *stageArray;
    uint32_t vkVertexInputBindingDescriptionCount;
    VkVertexInputBindingDescription *vkVertexInputBindingDescriptions;
    uint32_t vkVertexInputAttributeDescriptionCount;
    VkVertexInputAttributeDescription *vkVertexInputAttributeDescriptions;
    VkPrimitiveTopology vkPrimitiveTopology;
    VkBool32 primitiveRestartEnable;
    VkViewport viewport;
} GFXCommandCreateInfo;

typedef struct GFXCommandStruct
{
    GFXCommandCreateInfo gfxCommandCreateInfo;
    VkRenderPass vkRenderPass;
    VkFramebuffer *vkFramebuffers;
    VkPipeline vkPipeline;
    VkCommandBuffer vkCommandBuffer;
    bool isValid;
} GFXCommand;

typedef struct GFXEngineStruct
{
    // Config
    bool enableValidationLayers;
    char *name;
    int height;
    int width;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;
    uint32_t maxCommandBufferListCount;

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
    uint32_t frameCount;
    uint32_t frameIndex;
    bool hasRecreateSwapchain;

    GFXCommand *gfxCommands;
    uint32_t gfxCommandCount;
} GFXEngine;

void StartGFXEngine(GFXEngine *pGFXEngine);
void UpdateGFXEngine(GFXEngine *pGFXEngine);
void EndGFXEngine(GFXEngine *pGFXEngine);