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
    TKNColorAttachmentType,
    TKNDepthAttachmentType,
    TKNStencilAttachmentType,
    TKNPositionAttachmentType,
    TKNNormalAttachmentType,
    TKNAlbedoAttachmentType,
    TNKCustomAttachmentType,
} TKNAttachmentType;

typedef struct VkRenderPassCreateInfoNodePtrStruct
{
    VkRenderPassCreateInfo *pVkRenderPassCreateInfo;
    struct VkRenderPassCreateInfoNodePtrStruct *pNext;
} VkRenderPassCreateInfoPtrNode;

typedef struct VkGraphicsPipelineCreateConfigStruct
{
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo;
    uint32_t vkPipelineShaderStageCreateInfosCount;
    VkPipelineShaderStageCreateInfo *vkPipelineShaderStageCreateInfos;
    VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo;
    uint32_t vkDescriptorSetLayoutCreateInfoCount;
    VkDescriptorSetLayoutCreateInfo *vkDescriptorSetLayoutCreateInfos;
    VkRenderPassCreateInfo vkRenderPassCreateInfo;
    VkFramebufferCreateInfo vkFramebufferCreateInfo;
    TKNAttachmentType *tknAttachmentTypes;
} VkGraphicsPipelineCreateConfig;

typedef struct TKNGraphicPipelineStruct
{
    VkGraphicsPipelineCreateConfig vkGraphicsPipelineCreateConfig;
    // Runtime:
    VkRenderPass vkRenderPass;
    VkFramebuffer *vkFramebuffers;
    // For vkCmd functions
    VkPipelineLayout vkPipelineLayout;
    // For vkCmd functions
    VkPipeline vkPipeline;
    VkCommandBuffer *vkCommandBuffers;
} TKNGraphicPipeline;

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
    uint32_t submitVkCommandBufferCount;
    VkCommandBuffer *submitVkCommandBuffers;

    uint32_t acquiredImageIndex;
    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *renderFinishedSemaphores;
    VkFence *renderFinishedFences;
    uint32_t frameCount;
    uint32_t frameIndex;
    bool hasRecreateSwapchain;

    uint32_t maxVkRenderPassCount;
    uint32_t vkRenderPassCount;
    VkRenderPass *vkRenderPasses;
    VkRenderPassCreateInfo *vkRenderPassCreateInfos;
    VkRenderPassCreateInfoPtrNode *vkRenderPassCreateInfoPtrNodes;

    // TKNGraphicPipeline *tknGraphicPipelines;
    // uint32_t tknGraphicPipelineCount;
} GFXEngine;

void StartGFXEngine(GFXEngine *pGFXEngine);
void UpdateGFXEngine(GFXEngine *pGFXEngine);
void EndGFXEngine(GFXEngine *pGFXEngine);

void AddTKNGraphicPipeline(GFXEngine *pGFXEngine, VkGraphicsPipelineCreateConfig tknGraphicPipelineConfig);