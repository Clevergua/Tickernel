#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cglm.h"
#include "tickernelCore.h"

typedef struct
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkBufferMemory;
    VkDeviceSize size;
} Buffer;

typedef struct
{
    Buffer buffer;
    void *mapped;
} MappedBuffer;

typedef struct
{
    Buffer vertexBuffer;
    uint32_t vertexCount;

    Buffer indexBuffer;
    uint32_t indexCount;

    MappedBuffer instanceMappedBuffer;
    uint32_t instanceCount;
    uint32_t maxInstanceCount;
} Mesh;

typedef struct
{
    TickernelDynamicArray meshDynamicArray;
    VkDescriptorPool vkDescriptorPool;
    VkDescriptorSet vkDescriptorSet;
} Material;

typedef struct
{
    VkPipeline vkPipeline;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout vkPipelineLayout;

    uint32_t vkDescriptorPoolSizeCount;
    VkDescriptorPoolSize *vkDescriptorPoolSizes;

    TickernelDynamicArray materialDynamicArray;
} Pipeline;

typedef struct
{
    TickernelDynamicArray pipelineDynamicArray;
} Subpass;

typedef struct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;

    uint32_t subpassCount;
    Subpass *subpasses;
} RenderPass;

typedef struct
{
    VkImage vkImage;
    VkFormat vkFormat;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
} GraphicsImage;

typedef enum
{
    ATTACHMENT_TYPE_DYNAMIC,
    ATTACHMENT_TYPE_FIXED,
    ATTACHMENT_TYPE_SWAPCHAIN,
} AttachmentType;

typedef struct
{
} SwapchainAttachmentContent;

typedef struct
{
    GraphicsImage graphicsImage;
    uint32_t width;
    uint32_t height;
} FixedAttachmentContent;

typedef struct
{
    GraphicsImage graphicsImage;
    float32_t scaler;
} DynamicAttachmentContent;

typedef union
{
    SwapchainAttachmentContent swapchainAttachmentContent;
    FixedAttachmentContent fixedAttachmentContent;
    DynamicAttachmentContent dynamicAttachmentContent;
} AttachmentContent;

typedef struct
{
    AttachmentType attachmentType;
    AttachmentContent attachmentContent;
} Attachment;

typedef enum
{
    GRAPHICS_RESOURCE_TYPE_ATTACHMENT,
    GRAPHICS_RESOURCE_TYPE_IMAGE,
    GRAPHICS_RESOURCE_TYPE_UNIFORM_BUFFER,
} GraphicsResourceType;

typedef struct
{
    GraphicsResourceType graphicsResourceType;
    void *pResource;
} GraphicsResource;

typedef struct
{
    uint8_t magic[4];
    uint8_t blockDimX;
    uint8_t blockDimY;
    uint8_t blockDimZ;
    uint8_t width[3];
    uint8_t height[3];
    uint8_t depth[3];
} ASTCHeader;

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
    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    VkDevice vkDevice;
    VkQueue vkGraphicsQueue;
    VkQueue vkPresentQueue;

    VkSwapchainKHR vkSwapchain;
    VkSurfaceFormatKHR surfaceFormat;
    uint32_t swapchainImageCount;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;
    uint32_t swapchainIndex;
    Attachment swapchainAttachment;

    VkCommandPool graphicsVkCommandPool;
    VkCommandBuffer *graphicsVkCommandBuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;

    TickernelDynamicArray renderPasseDynamicArray;
    TickernelDynamicArray dynamicAttachmentDynamicArray;

} GraphicsContext;

GraphicsContext *createGraphicsContext(const char *assetsPath, int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight);
void updateGraphicsContext(GraphicsContext *pGraphicsContext, uint32_t swapchainWidth, uint32_t swapchainHeight);
void destroyGraphicsContext(GraphicsContext *pGraphicsContext);

void createASTCGraphicsImage(GraphicsContext *pGraphicsContext, const char *fileName, GraphicsImage **ppGraphicsImage);
void destroyASTCGraphicsImage(GraphicsContext *pGraphicsContext, GraphicsImage *pGraphicsImage);

void createSampler(GraphicsContext *pGraphicsContext, VkSamplerCreateInfo samplerCreateInfo, VkSampler *pVkSampler);
void destroySampler(GraphicsContext *pGraphicsContext, VkSampler vkSampler);

void createMesh(GraphicsContext *pGraphicsContext, uint32_t vertexCount, VkDeviceSize vertexBufferSize, void *vertexBufferData, uint32_t indexCount, VkDeviceSize indexBufferSize, void *indexBufferData, uint32_t instanceCount, VkDeviceSize instanceBufferSize, void *instanceBufferData, Mesh **ppMesh);
void destroyMesh(GraphicsContext *pGraphicsContext, Mesh *pMesh);
void updateMeshInstanceBuffer(GraphicsContext *pGraphicsContext, Mesh *pMesh, VkDeviceSize instanceBufferSize, void *instanceBufferData, uint32_t instanceCount);

void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **pAttachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass);
void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);

void createPipeline(GraphicsContext *pGraphicsContext, uint32_t stageCount, const char **shaderPaths, VkPipelineShaderStageCreateInfo *stages, VkPipelineVertexInputStateCreateInfo vertexInputState, VkPipelineInputAssemblyStateCreateInfo inputAssemblyState, VkPipelineViewportStateCreateInfo viewportState, VkPipelineRasterizationStateCreateInfo rasterizationState, VkPipelineMultisampleStateCreateInfo multisampleState, VkPipelineDepthStencilStateCreateInfo depthStencilState, VkPipelineColorBlendStateCreateInfo colorBlendState, VkPipelineDynamicStateCreateInfo dynamicState, VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo, RenderPass *pRenderPass, uint32_t subpassIndex, uint32_t vkDescriptorPoolSizeCount, VkDescriptorPoolSize *vkDescriptorPoolSizes, uint32_t pipelineIndex, Pipeline **ppPipeline);
void destroyPipeline(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass, uint32_t subpassIndex, Pipeline *pPipeline);

void createMaterial(GraphicsContext *pGraphicsContext, Pipeline *pPipeline, uint32_t graphicsResourceCount, GraphicsResource *graphicsResources, Material **ppMaterial);
void destroyMaterial(GraphicsContext *pGraphicsContext, Pipeline *pPipeline, Material *pMaterial);

void createDynamicAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment **ppAttachment);
void destroyDynamicAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment);

void createFixedAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment **ppAttachment);
void destroyFixedAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment);
void findSupportedFormat(GraphicsContext *pGraphicsContext, VkFormat *candidates, uint32_t candidatesCount, VkFormatFeatureFlags features, VkImageTiling tiling, VkFormat *pVkFormat);

void createUniformBuffer(GraphicsContext *pGraphicsContext, VkDeviceSize vkBufferSize, MappedBuffer **ppUniformBuffer);
void destroyUniformBuffer(GraphicsContext *pGraphicsContext, MappedBuffer *pUniformBuffer);
void updateGlobalUniformBuffer(GraphicsContext *pGraphicContext, MappedBuffer *pUniformBuffer, void *data, size_t size);