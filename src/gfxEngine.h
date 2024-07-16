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
    AttachmentType *attachmentTypes;

} TKNRenderPassConfig;

typedef struct TKNFramebufferConfigStruct
{
    uint32_t width;
    uint32_t height;
    uint32_t layers;
} TKNFramebufferConfig;

typedef struct TKNPipelineConfig
{
    TKNRenderPassConfig tknRenderPassConfig;

    // VkRect2D renderArea;
    // uint32_t vkClearValueCount;
    // VkClearValue *vkClearValues;

    uint32_t vkShaderModuleCreateInfoCount;
    size_t *codeSizes;
    uint32_t **codes;
    char **codeFunctionNames;
    VkShaderStageFlagBits *stages;

    uint32_t vkVertexInputBindingDescriptionCount;
    VkVertexInputBindingDescription *vkVertexInputBindingDescriptions;
    uint32_t vkVertexInputAttributeDescriptionCount;
    VkVertexInputAttributeDescription *vkVertexInputAttributeDescriptions;
    VkPrimitiveTopology vkPrimitiveTopology;
    VkBool32 primitiveRestartEnable;
    uint32_t viewportCount;
    VkViewport *viewports;
    uint32_t scissorCount;
    VkRect2D *scissors;

    VkBool32 vkBufferCreateInfodepthClampEnable;
    VkBool32 rasterizerDiscardEnable;
    VkPolygonMode polygonMode;
    VkCullModeFlags cullMode;
    VkFrontFace frontFace;
    VkBool32 depthBiasEnable;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;

    VkBool32 depthTestEnable;
    VkBool32 depthWriteEnable;
    VkCompareOp depthCompareOp;
    VkBool32 depthBoundsTestEnable;
    VkBool32 stencilTestEnable;
    VkStencilOpState front;
    VkStencilOpState back;
    float minDepthBounds;
    float maxDepthBounds;

    uint32_t vkPipelineColorBlendAttachmentStateCount;
    VkPipelineColorBlendAttachmentState *vkPipelineColorBlendAttachmentStates;
    float blendConstants[4];

    uint32_t setLayoutCount;
    uint32_t *bindingCounts;
    VkDescriptorSetLayoutBinding **bindingsArray;
    uint32_t pushConstantRangeCount;
    VkPushConstantRange *pushConstantRanges;

    uint32_t dynamicStateCount;
    VkDynamicState *dynamicStates;

    uint32_t subpassIndex;

} TKNPipelineConfig;

typedef struct GFXCommandStruct
{
    TKNPipelineConfig tknPipelineConfig;
    TKNFramebufferConfig tknFrameBufferConfig;

    VkRenderPass vkRenderPass;
    VkFramebuffer *vkFramebuffers;
    VkPipelineLayout vkPipelineLayout;
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