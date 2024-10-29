#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm.h>
#include <tickernelWindow.h>
#define INVALID_VKFRAMEBUFFER 0
#define MAX_VK_DESCRIPTOR_TPYE 11

typedef struct GlobalUniformBufferStruct
{
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    float pointSizeFactor;
} GlobalUniformBuffer;

typedef struct SubpassModelStruct
{
    uint32_t vertexCount;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer modelUniformBuffer;
    VkDeviceMemory modelUniformBufferMemory;
    void *modelUniformBufferMapped;

    VkDescriptorSet vkDescriptorSet;
    bool isValid;
} SubpassModel;

typedef struct SubpassStruct
{
    VkPipeline vkPipeline;
    VkPipelineLayout vkPipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;

    uint32_t modelCountPerDescriptorPool;
    uint32_t vkDescriptorPoolCount;
    VkDescriptorPool *vkDescriptorPools;
    uint32_t modelCount;
    SubpassModel *models;
    Uint32Node *pRemovedIndexLinkedList;
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

void CreateBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags msemoryPropertyFlags, VkBuffer *pBuffer, VkDeviceMemory *pDeviceMemory);
void DestroyBuffer(VkDevice vkDevice, VkBuffer vkBuffer, VkDeviceMemory deviceMemory);

void CreateVertexBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkDeviceSize vertexBufferSize, void *vertices, VkBuffer *pVertexBuffer, VkDeviceMemory *pVertexBufferMemory);
void DestroyVertexBuffer(VkDevice vkDevice, VkBuffer vertexBuffer, VkDeviceMemory vertexBufferMemory);

void AddModelToSubpass(VkDevice vkDevice, Subpass *pSubpass, uint32_t *pModelIndex);
void RemoveModelFromSubpass(uint32_t modelIndex, Subpass *pSubpass);
