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
} GraphicImage;

typedef enum
{
    ATTACHMENT_TYPE_DYNAMIC,
    ATTACHMENT_TYPE_FIXED,
    ATTACHMENT_TYPE_SWAPCHAIN,
} AttachmentType;

typedef struct
{
} SwapchainAttachment;

typedef struct
{
    GraphicImage graphicImage;
    uint32_t width;
    uint32_t height;
} FixedAttachment;

typedef struct
{
    GraphicImage graphicImage;
    float32_t scaler;
} DynamicAttachment;

typedef union
{
    SwapchainAttachment swapchainAttachment;
    FixedAttachment fixedAttachment;
    DynamicAttachment dynamicAttachment;
} AttachmentContent;

typedef struct
{
    AttachmentType attachmentType;
    AttachmentContent attachmentContent;
} Attachment;

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
    uint32_t graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    VkDevice vkDevice;
    VkQueue vkGraphicQueue;
    VkQueue vkPresentQueue;

    VkSwapchainKHR vkSwapchain;
    VkSurfaceFormatKHR surfaceFormat;
    uint32_t swapchainImageCount;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;
    uint32_t swapchainIndex;

    VkCommandPool graphicVkCommandPool;
    VkCommandBuffer *graphicVkCommandBuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;

    TickernelDynamicArray renderPasseDynamicArray;
    TickernelDynamicArray attachmentDynamicArray;
    TickernelDynamicArray uniformBufferDynamicArray;
} GraphicContext;

GraphicContext *startGraphic(const char *assetsPath, int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight);
void updateGraphic(GraphicContext *pGraphicContext, uint32_t swapchainWidth, uint32_t swapchainHeight);
void endGraphic(GraphicContext *pGraphicContext);

void createASTCGraphicImage(GraphicContext *pGraphicContext, const char *fileName, VkCommandPool commandPool, VkQueue graphicQueue, GraphicImage *pGraphicImage);
void destroyASTCGraphicImage(GraphicContext *pGraphicContext, GraphicImage graphicImage);

void createSampler(GraphicContext *pGraphicContext, VkSamplerCreateInfo samplerCreateInfo, VkSampler *pVkSampler);
void destroySampler(GraphicContext *pGraphicContext, VkSampler vkSampler);

void createPipeline(GraphicContext *pGraphicContext, VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo, VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo, char **shaderPaths, uint32_t vkDescriptorPoolSizeCount, VkDescriptorPoolSize *vkDescriptorPoolSizes, Pipeline *pPipeline);
void destroyPipeline(GraphicContext *pGraphicContext, Pipeline pipeline);

void createMaterial(GraphicContext *pGraphicContext, Pipeline pipeline, size_t meshSize, VkWriteDescriptorSet *vkWriteDescriptorSets, uint32_t vkWriteDescriptorSetCount, Material *pMaterial);
void destroyMaterial(GraphicContext *pGraphicContext, Material material);

void createMesh(GraphicContext *pGraphicContext, uint32_t vertexCount, VkDeviceSize vertexBufferSize, void *vertexBufferData, uint32_t indexCount, VkDeviceSize indexBufferSize, void *indexBufferData, uint32_t instanceCount, VkDeviceSize instanceBufferSize, void *instanceBufferData, Mesh *pMesh);
void destroyMesh(GraphicContext *pGraphicContext, Mesh *pMesh);
void updateMeshInstanceBuffer(GraphicContext *pGraphicContext, Mesh *pMesh, VkDeviceSize instanceBufferSize, void *instanceBufferData, uint32_t instanceCount);

void createRenderPass(GraphicContext *pGraphicContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment *attachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, RenderPass *pRenderPass);
void destroyRenderPass(GraphicContext *pGraphicContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment *attachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, RenderPass *pRenderPass);
void addRenderPass(GraphicContext *pGraphicContext, RenderPass renderPass, RenderPass *pRenderPass);
void removeRenderPass(GraphicContext *pGraphicContext, RenderPass *pRenderPass);