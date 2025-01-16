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

// GlobalBuffer 世界场景中的数据
// ModelBuffer 每种建筑的数据
// InstanceBuffer 每个建筑实例的数据
typedef struct SubpassModelStruct
{
    uint32_t vertexCount;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

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

typedef struct SubpassStruct
{
    VkPipeline vkPipeline;
    VkPipelineLayout vkPipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;

    TickernelCollection modelCollection;
    uint32_t vkDescriptorPoolSizeCount;
    VkDescriptorPoolSize *vkDescriptorPoolSizes;
} Subpass;

typedef struct GraphicImageStruct
{
    VkImage vkImage;
    VkFormat vkFormat;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
} GraphicImage;

void TryThrowVulkanError(VkResult vkResult);
void FindMemoryType(VkPhysicalDevice vkPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex);
void FindDepthFormat(VkPhysicalDevice vkPhysicalDevice, VkFormat *pDepthFormat);

void CreateImageView(VkDevice vkDevice, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView);
void CreateGraphicImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage);
void DestroyGraphicImage(VkDevice vkDevice, GraphicImage graphicImage);
void CreateVkShaderModule(VkDevice vkDevice, const char *filePath, VkShaderModule *pVkShaderModule);
void DestroyVkShaderModule(VkDevice vkDevice, VkShaderModule vkShaderModule);

void CopyVkBuffer(VkCommandPool graphicVkCommandPool, VkDevice vkDevice, VkQueue vkGraphicQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize offset, VkDeviceSize size);
void CreateBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags msemoryPropertyFlags, VkBuffer *pBuffer, VkDeviceMemory *pDeviceMemory);
void DestroyBuffer(VkDevice vkDevice, VkBuffer vkBuffer, VkDeviceMemory deviceMemory);
void UpdateBufferWithStagingBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer vkBuffer);
void UpdateBuffer(VkDevice vkDevice, VkDeviceMemory bufferMemory, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData);
