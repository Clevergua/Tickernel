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
    TNKCustomAttachmentType,
} TickernelAttachmentType;

typedef struct TickernelRenderPipelineStruct
{
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo;
    uint32_t vkPipelineShaderStageCreateInfosCount;
    VkPipelineShaderStageCreateInfo *vkPipelineShaderStageCreateInfos;
    VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo;
    uint32_t vkDescriptorSetLayoutCreateInfoCount;
    VkDescriptorSetLayoutCreateInfo *vkDescriptorSetLayoutCreateInfos;
    VkPipeline vkPipeline;
} TickernelRenderPipeline;

typedef struct TickernelRenderPassStruct
{
    VkRenderPassCreateInfo vkRenderPassCreateInfo;
    VkRenderPass vkRenderPass;
    VkFramebufferCreateInfo vkFramebufferCreateInfo;
    TickernelAttachmentType *tickernelAttachmentTypes;
    VkFramebuffer *vkFramebuffers;
    TickernelRenderPipeline *tickernelRenderPipelines;
} TickernelRenderPass;

// typedef struct PVkRenderPassCreateInfoNodePtrStruct
// {
//     VkRenderPassCreateInfo *pVkRenderPassCreateInfo;
//     struct PVkRenderPassCreateInfoNodePtrStruct *pNext;
// } PVkRenderPassCreateInfoNode;

// typedef struct VkGraphicsPipelineCreateConfigStruct
// {
//     VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo;
//     uint32_t vkPipelineShaderStageCreateInfosCount;
//     VkPipelineShaderStageCreateInfo *vkPipelineShaderStageCreateInfos;
//     VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo;
//     uint32_t vkDescriptorSetLayoutCreateInfoCount;
//     VkDescriptorSetLayoutCreateInfo *vkDescriptorSetLayoutCreateInfos;
//     VkRenderPassCreateInfo vkRenderPassCreateInfo;
//     VkFramebufferCreateInfo vkFramebufferCreateInfo;
//     TickernelAttachmentType *tickernelAttachmentTypes;
// } VkGraphicsPipelineCreateConfig;

// typedef struct TickernelGraphicPipelineStruct
// {
//     VkRenderPass vkRenderPass;
//     VkFramebuffer *vkFramebuffers;
//     VkPipelineLayout vkPipelineLayout;
//     VkPipeline vkPipeline;
//     VkCommandBuffer *vkCommandBuffers;
// } TickernelGraphicPipeline;

// typedef struct PGraphicPipelineNodeStruct
// {
//     struct PGraphicPipelineNodeStruct *pNext;
//     VkGraphicsPipelineCreateConfig *pVkGraphicsPipelineCreateConfig;
//     TickernelGraphicPipeline *pTickernelGraphicPipeline;
// } PGraphicPipelineNode;

typedef struct GraphicEngineStruct
{
    // Config
    bool enableValidationLayers;
    char *name;
    int height;
    int width;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;
    uint32_t maxCommandBufferListCount;
    uint32_t maxVkRenderPassCount;
    uint32_t tickernelGraphicPipelineHashSize;

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
    // uint32_t vkRenderPassCount;
    // VkRenderPass *vkRenderPasses;
    // VkRenderPassCreateInfo *vkRenderPassCreateInfos;
    // PVkRenderPassCreateInfoNode **pVkRenderPassCreateInfoNodeList;

    // PGraphicPipelineNode* *pGraphicPipelineNodes;
} GraphicEngine;

void StartGraphicEngine(GraphicEngine *pGraphicEngine);
void UpdateGraphicEngine(GraphicEngine *pGraphicEngine);
void EndGraphicEngine(GraphicEngine *pGraphicEngine);

// void AddTickernelGraphicPipeline(GraphicEngine *pGraphicEngine, VkGraphicsPipelineCreateConfig tickernelGraphicPipelineConfig);