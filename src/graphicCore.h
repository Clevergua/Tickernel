#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cglm.h"
#include "tickernelCore.h"

#define INVALID_VKFRAMEBUFFER 0
#define MAX_VK_DESCRIPTOR_TPYE 11
#define MAX_POINT_LIGHT_COUNT 256

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
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo;
    VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo;
    char **shaderPaths;
    uint32_t vkDescriptorPoolSizeCount;
    VkDescriptorPoolSize *vkDescriptorPoolSizes;
} PipelineConfig;

typedef struct
{
    VkRenderPassCreateInfo vkRenderPassCreateInfo;

} RenderPassConfig;

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
    Buffer vertexBuffer;
    uint32_t vertexCount;

    Buffer indexBuffer;
    uint32_t indexCount;

    MappedBuffer instanceMappedBuffer;
    uint32_t instanceCount;
    uint32_t maxInstanceCount;
} Mesh;

void tryThrowVulkanError(VkResult vkResult);
void findMemoryType(VkPhysicalDevice vkPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex);
void findDepthFormat(VkPhysicalDevice vkPhysicalDevice, VkFormat *pDepthFormat);

void createImageView(VkDevice vkDevice, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView);
void createGraphicImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage);
void destroyGraphicImage(VkDevice vkDevice, GraphicImage graphicImage);
void createVkShaderModule(VkDevice vkDevice, const char *filePath, VkShaderModule *pVkShaderModule);
void destroyVkShaderModule(VkDevice vkDevice, VkShaderModule vkShaderModule);

void createBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, Buffer *pBuffer);
void destroyBuffer(VkDevice vkDevice, Buffer buffer);
void updateBufferWithStagingBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer vkBuffer);
void updateBuffer(VkDevice vkDevice, VkDeviceMemory vkBufferMemory, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData);

void createMappedBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, MappedBuffer *pMappedBuffer);
void destroyMappedBuffer(VkDevice vkDevice, MappedBuffer mappedBuffer);
void updateMappedBuffer(MappedBuffer *pMappedBuffer, void *data, VkDeviceSize size);

GraphicImage *createASTCGraphicImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, const char *fileName, VkCommandPool commandPool, VkQueue graphicQueue);
void destroyASTCGraphicImage(VkDevice vkDevice, GraphicImage *pGraphicImage);