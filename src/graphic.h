#pragma once
#include "graphicCore.h"
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

    TickernelDynamicArray RenderPasseDynamicArray;
    TickernelDynamicArray AttachmentDynamicArray;
    TickernelDynamicArray UniformBufferDynamicArray;
} GraphicContext;

GraphicContext *startGraphic(const char *assetsPath, int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight);
void updateGraphic(GraphicContext *pGraphicContext, uint32_t swapchainWidth, uint32_t swapchainHeight);
void endGraphic(GraphicContext *pGraphicContext);

void createPipeline(GraphicContext *pGraphicContext, VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo, VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo, char **shaderPaths, uint32_t vkDescriptorPoolSizeCount, VkDescriptorPoolSize *vkDescriptorPoolSizes, Pipeline *pPipeline);
void destroyPipeline(Pipeline pipeline, VkDevice vkDevice);

void createMaterial(VkDevice vkDevice, Pipeline pipeline, size_t meshSize, VkWriteDescriptorSet *vkWriteDescriptorSets, uint32_t vkWriteDescriptorSetCount, Material *pMaterial);
void destroyMaterial(Material material, VkDevice vkDevice);

void createMesh(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, uint32_t vertexCount, VkDeviceSize vertexBufferSize, void *vertexBufferData, uint32_t indexCount, VkDeviceSize indexBufferSize, void *indexBufferData, uint32_t instanceCount, VkDeviceSize instanceBufferSize, void *instanceBufferData, Mesh *pMesh);
void destroyMesh(Mesh *pMesh, VkDevice vkDevice);
void updateMeshInstanceBuffer(Mesh *pMesh, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, VkDeviceSize instanceBufferSize, void *instanceBufferData, uint32_t instanceCount);