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

// Designed with the principle of minimal learning cost and simplicity, we utilize Vulkan’s native VkCreateInfo as pipeline construction parameters and for managing runtime objects. The fields of VkCreateInfo are divided into configurable sections and runtime object sections. The former is configured through external input, while the latter is generated and managed during runtime calls to ensure its lifecycle.
typedef enum
{
    TickernelColorAttachmentType,
    TickernelDepthAttachmentType,
    TickernelStencilAttachmentType,
    TickernelPositionAttachmentType,
    TickernelNormalAttachmentType,
    TickernelAlbedoAttachmentType,
} TickernelAttachmentType;

typedef struct TickernelRenderPipelineStruct
{
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo;
    uint32_t stageCount;
    VkPipelineShaderStageCreateInfo *vkPipelineShaderStageCreateInfos;
    VkShaderModuleCreateInfo *vkShaderModuleCreateInfos;
    VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo;
    uint32_t vkDescriptorSetLayoutCreateInfoCount;
    VkDescriptorSetLayoutCreateInfo *vkDescriptorSetLayoutCreateInfos;
    VkPipeline vkPipeline;
    struct TickernelRenderPassStruct *pTickernelRenderPass;

} TickernelRenderPipeline;

typedef struct TickernelRenderPassStruct
{
    VkRenderPassCreateInfo vkRenderPassCreateInfo;
    VkRenderPass vkRenderPass;
    VkFramebufferCreateInfo vkFramebufferCreateInfo;
    uint32_t tickernelAttachmentTypeCount;
    TickernelAttachmentType *tickernelAttachmentTypes;
    VkFramebuffer *vkFramebuffers;
    uint32_t tickernelRenderPipelineCount;
    struct TickernelRenderPipelineStruct **pTickernelRenderPipelines;
} TickernelRenderPass;

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
    VkImage depthImage;
    VkFormat depthFormat;
    VkImageView depthImageView;
    VkDeviceMemory depthImageMemory;
    VkCommandPool graphicVkCommandPool;
    VkCommandBuffer *graphicVkCommandBuffers;
    uint32_t acquiredImageIndex;
    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *renderFinishedSemaphores;
    VkFence *renderFinishedFences;
    uint32_t frameCount;
    uint32_t frameIndex;
    bool hasRecreateSwapchain;
} GraphicEngine;

void StartGraphicEngine(GraphicEngine *pGraphicEngine);
void UpdateGraphicEngine(GraphicEngine *pGraphicEngine);
void EndGraphicEngine(GraphicEngine *pGraphicEngine);