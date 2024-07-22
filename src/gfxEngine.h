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
} AttachmentType;

typedef struct TKNRenderPassConfigStruct
{
    uint32_t vkAttachmentCount;
    const VkAttachmentDescription *vkAttachmentDescriptions;
    uint32_t vkSubpassDescriptionCount;
    const VkSubpassDescription *vkSubpassDescriptions;
    uint32_t vkSubpassDependencyCount;
    const VkSubpassDependency *vkSubpassDependencies;

} TKNRenderPassConfig;

typedef struct TKNGraphicPipelineConfigStruct
{
    // RenderPass:
    // Using struct to cache VkRenderPass
    TKNRenderPassConfig tknRenderPassConfig;
    uint32_t subpassIndex;
    // Shader:
    uint32_t vkShaderModuleCreateInfoCount;
    size_t *codeSizes;
    uint32_t **codes;
    char **codeFunctionNames;
    VkShaderStageFlagBits *stages;
    // Input:
    uint32_t vkVertexInputBindingDescriptionCount;
    VkVertexInputBindingDescription *vkVertexInputBindingDescriptions;
    uint32_t vkVertexInputAttributeDescriptionCount;
    VkVertexInputAttributeDescription *vkVertexInputAttributeDescriptions;
    VkPrimitiveTopology vkPrimitiveTopology;
    VkBool32 primitiveRestartEnable;
    // Viewport:
    uint32_t viewportCount;
    VkViewport *viewports;
    uint32_t scissorCount;
    VkRect2D *scissors;
    // Rasterization:
    VkBool32 rasterizerDiscardEnable;
    VkPolygonMode polygonMode;
    VkCullModeFlags cullMode;
    VkFrontFace frontFace;
    VkBool32 depthBiasEnable;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;
    // Depth Stencil:
    VkBool32 depthTestEnable;
    VkBool32 depthWriteEnable;
    VkCompareOp depthCompareOp;
    VkBool32 depthBoundsTestEnable;
    VkBool32 stencilTestEnable;
    VkStencilOpState front;
    VkStencilOpState back;
    float minDepthBounds;
    float maxDepthBounds;
    // Color Blend:
    uint32_t vkPipelineColorBlendAttachmentStateCount;
    VkPipelineColorBlendAttachmentState *vkPipelineColorBlendAttachmentStates;
    float blendConstants[4];
    // Set Layouts:
    uint32_t setLayoutCount;
    uint32_t *bindingCounts;
    VkDescriptorSetLayoutBinding **bindingsArray;
    uint32_t pushConstantRangeCount;
    VkPushConstantRange *pushConstantRanges;
    // Dynamic:
    uint32_t dynamicStateCount;
    VkDynamicState *dynamicStates;
    // Framebuffer:
    uint32_t width;
    uint32_t height;
    uint32_t layers;
    uint32_t vkAttachmentCount;
    AttachmentType *attachmentTypes;
} TKNGraphicPipelineConfig;

typedef struct TKNGraphicPipelineStruct
{
    // Config:
    TKNGraphicPipelineConfig tknGraphicPipelineConfig;
    // Runtime:
    VkRenderPass vkRenderPass;
    VkDescriptorSetLayout *setLayouts;
    VkFramebuffer *vkFramebuffers;
    // For vkCmd functions
    VkPipelineLayout vkPipelineLayout;
    // For vkCmd functions
    VkPipeline vkPipeline;
    VkCommandBuffer *vkCommandBuffers;
    bool isValid;
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

    TKNGraphicPipeline *tknGraphicPipelines;
    uint32_t tknGraphicPipelineCount;
} GFXEngine;

void StartGFXEngine(GFXEngine *pGFXEngine);
void UpdateGFXEngine(GFXEngine *pGFXEngine);
void EndGFXEngine(GFXEngine *pGFXEngine);

void AddTKNGraphicPipeline(GFXEngine *pGFXEngine, TKNGraphicPipelineConfig tknGraphicPipelineConfig);