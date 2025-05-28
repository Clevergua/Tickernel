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
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    float pointSizeFactor;
    float time;
    int frameCount;
    float near;
    float far;
    float fov;
    int width;
    int height;
} GlobalUniformBuffer;

typedef struct
{
    vec4 color;
    vec3 direction;
} DirectionalLight;

typedef struct
{
    vec4 color;
    vec3 position;
    float range;
} PointLight;

typedef struct
{
    DirectionalLight directionalLight;
    int pointLightCount;
    PointLight pointLights[MAX_POINT_LIGHT_COUNT];
} LightsUniformBuffer;

typedef struct
{
    uint32_t vertexCount;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
} Mesh;

typedef struct
{
    Mesh* pMesh;

    uint32_t maxInstanceCount;
    uint32_t instanceCount;
    VkBuffer instanceBuffer;
    VkDeviceMemory instanceBufferMemory;

    VkBuffer modelUniformBuffer;
    VkDeviceMemory modelUniformBufferMemory;
    void *modelUniformBufferMapped;

    VkDescriptorPool vkDescriptorPool;
    VkDescriptorSet vkDescriptorSet;
} SubpassModel;

typedef struct
{
    VkPipeline vkPipeline;
    VkPipelineLayout vkPipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    TickernelDynamicArray modelDynamicArray;

    uint32_t vkDescriptorPoolSizeCount;
    VkDescriptorPoolSize *vkDescriptorPoolSizes;
} Pipeline;

typedef struct
{
    uint32_t pipelineCount;
    Pipeline* pipelines;
} Subpass;

typedef struct
{
    VkImage vkImage;
    VkFormat vkFormat;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
} GraphicImage;

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

void tryThrowVulkanError(VkResult vkResult);
void findMemoryType(VkPhysicalDevice vkPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex);
void findDepthFormat(VkPhysicalDevice vkPhysicalDevice, VkFormat *pDepthFormat);

void createImageView(VkDevice vkDevice, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView);
void createGraphicImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage);
void destroyGraphicImage(VkDevice vkDevice, GraphicImage graphicImage);
void createVkShaderModule(VkDevice vkDevice, const char *filePath, VkShaderModule *pVkShaderModule);
void destroyVkShaderModule(VkDevice vkDevice, VkShaderModule vkShaderModule);

void copyVkBuffer(VkCommandPool graphicVkCommandPool, VkDevice vkDevice, VkQueue vkGraphicQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize offset, VkDeviceSize size);
void createBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags msemoryPropertyFlags, VkBuffer *pBuffer, VkDeviceMemory *pDeviceMemory);
void destroyBuffer(VkDevice vkDevice, VkBuffer vkBuffer, VkDeviceMemory deviceMemory);
void updateBufferWithStagingBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer vkBuffer);
void updateBuffer(VkDevice vkDevice, VkDeviceMemory bufferMemory, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData);

GraphicImage *createASTCGraphicImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, const char *fileName, VkCommandPool commandPool, VkQueue graphicQueue);
void destroyASTCGraphicImage(VkDevice vkDevice, GraphicImage *pGraphicImage);