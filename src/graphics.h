#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cglm.h"
#include "tickernelCore.h"
#include "spirv_reflect.h"

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
    const char *name;
    uint32_t size;
    uint32_t offset;
} FieldLayout;

typedef struct
{
    uint32_t vertexLayoutCount;
    FieldLayout *vertexLayouts;
    uint32_t vertexSize;
    uint32_t instanceLayoutCount;
    FieldLayout *instanceLayouts;
    uint32_t instanceSize;
} MeshLayout;

typedef struct
{
    VkImage vkImage;
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
    VkFormat vkFormat;
    uint32_t width;
    uint32_t height;
} FixedAttachmentContent;

typedef struct
{
    VkImageLayout vkImageLayout;
    uint32_t set;
    uint32_t binding;
    struct PipelineStruct *pPipeline;
} DynamicAttachmentPipelineRef;

typedef struct
{
    GraphicsImage graphicsImage;
    VkFormat vkFormat;
    float32_t scaler;
    VkImageUsageFlags vkImageUsageFlags;
    VkMemoryPropertyFlags vkMemoryPropertyFlags;
    VkImageAspectFlags vkImageAspectFlags;
    TickernelDynamicArray pipelineRefDynamicArray;
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

typedef struct
{
    uint32_t vkDescriptorSetsCount;
    VkDescriptorSet *vkDescriptorSets;
    VkDescriptorPool vkDescriptorPool;
    TickernelDynamicArray pMeshDynamicArray;
} Material;

typedef struct PipelineStruct
{
    TickernelDynamicArray vkDescriptorSetLayoutDynamicArray;
    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkPipeline;
    TickernelDynamicArray vkDescriptorPoolSizeDynamicArray;
    TickernelDynamicArray pMaterialDynamicArray;
    TickernelDynamicArray pDynamicAttachmentRefDynamicArray;
} Pipeline;

typedef struct
{
    TickernelDynamicArray pPipelineDynamicArray;
    uint32_t inputAttachmentCount;
    VkAttachmentReference *inputAttachmentReferences;
} Subpass;

typedef struct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;

    uint32_t pAttachmentCount;
    Attachment **pAttachments;

    uint32_t subpassCount;
    Subpass *subpasses;
} RenderPass;

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
    VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
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

    TickernelDynamicArray pRenderPassDynamicArray;
    TickernelDynamicArray pAttachmentDynamicArray;
} GraphicsContext;

void createGraphicsContext(int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight, GraphicsContext *pGraphicsContext);
void updateGraphicsContext(GraphicsContext *pGraphicsContext, uint32_t swapchainWidth, uint32_t swapchainHeight);
void destroyGraphicsContext(GraphicsContext *pGraphicsContext);

void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **pAttachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass);
void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);

void findSupportedFormat(GraphicsContext *pGraphicsContext, VkFormat *candidates, uint32_t candidatesCount, VkFormatFeatureFlags features, VkImageTiling tiling, VkFormat *pVkFormat);

void createDynamicAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment **ppAttachment);
void destroyDynamicAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment);
void createFixedAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment **ppAttachment);
void destroyFixedAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment);

void createUniformBuffer(GraphicsContext *pGraphicsContext, VkDeviceSize vkBufferSize, MappedBuffer **ppUniformBuffer);
void destroyUniformBuffer(GraphicsContext *pGraphicsContext, MappedBuffer *pUniformBuffer);
void updateUniformBuffer(GraphicsContext *pGraphicContext, MappedBuffer *pUniformBuffer, size_t offset, void *data, size_t size);

void createASTCGraphicsImage(GraphicsContext *pGraphicsContext, const char *fileName, GraphicsImage **ppGraphicsImage);
void destroyASTCGraphicsImage(GraphicsContext *pGraphicsContext, GraphicsImage *pGraphicsImage);

void createSampler(GraphicsContext *pGraphicsContext, VkSamplerCreateInfo samplerCreateInfo, VkSampler *pVkSampler);
void destroySampler(GraphicsContext *pGraphicsContext, VkSampler vkSampler);

void createMesh(GraphicsContext *pGraphicsContext, uint32_t vertexCount, VkDeviceSize vertexBufferSize, void *vertexBufferData, uint32_t indexCount, VkDeviceSize indexBufferSize, void *indexBufferData, uint32_t instanceCount, VkDeviceSize instanceBufferSize, void *instanceBufferData, Mesh **ppMesh);
void destroyMesh(GraphicsContext *pGraphicsContext, Mesh *pMesh);
void updateMeshInstanceBuffer(GraphicsContext *pGraphicsContext, Mesh *pMesh, VkDeviceSize instanceBufferSize, void *instanceBufferData, uint32_t instanceCount);