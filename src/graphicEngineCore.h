#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#include <tickernelCore.h>
#include <cglm.h>
#define INVALID_VKFRAMEBUFFER 0
#define MAX_VK_DESCRIPTOR_TPYE 11

typedef struct GlobalUniformBufferStruct
{
    mat4 view;
    mat4 proj;
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
} SubpassModel;

typedef struct ModelGroupStruct
{
    uint32_t modelCount;
    SubpassModel *subpassModels;
    Uint32Node *pRemovedIndexLinkedList;

    VkDescriptorPool vkDescriptorPool;
} ModelGroup;

typedef struct SubpassStruct
{
    VkPipeline vkPipeline;
    VkPipelineLayout vkPipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;

    uint32_t maxModelGroupCount;
    uint32_t modelGroupCount;
    ModelGroup *modelGroups;
    uint32_t modelCountPerGroup;

    uint32_t *vkDescriptorTypeToCount;
} Subpass;

typedef struct RenderPassStruct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;

    uint32_t subpassCount;
    Subpass *subpasses;
} RenderPass;

typedef struct GraphicImageStruct
{
    VkImage vkImage;
    VkFormat vkFormat;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
} GraphicImage;

typedef struct GraphicEngineStruct
{
    // Config
    bool enableValidationLayers;
    char *name;
    int height;
    int width;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;

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

    VkCommandPool graphicVkCommandPool;
    VkCommandBuffer *graphicVkCommandBuffers;
    uint32_t acquiredImageIndex;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;
    uint32_t frameCount;
    uint32_t frameIndex;
    bool hasRecreatedSwapchain;

    VkBuffer globalUniformBuffer;
    VkDeviceMemory globalUniformBufferMemory;
    void *globalUniformBufferMapped;

    GraphicImage depthGraphicImage;
    GraphicImage albedoGraphicImage;
    RenderPass deferredRenderPass;
    uint32_t fullScreenVerticesGroupIndex;
    uint32_t fullScreenVerticesModelIndex;
} GraphicEngine;

void TryThrowVulkanError(VkResult vkResult);
void FindMemoryType(GraphicEngine *pGraphicEngine, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex);
void FindDepthFormat(GraphicEngine *pGraphicEngine, VkFormat *pDepthFormat);

void CreateImageView(GraphicEngine *pGraphicEngine, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView);
void CreateGraphicImage(GraphicEngine *pGraphicEngine, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage);
void DestroyGraphicImage(GraphicEngine *pGraphicEngine, GraphicImage graphicImage);
void CreateVkShaderModule(GraphicEngine *pGraphicEngine, const char *filePath, VkShaderModule *pVkShaderModule);
void DestroyVkShaderModule(GraphicEngine *pGraphicEngine, VkShaderModule vkShaderModule);

void CreateBuffer(GraphicEngine *pGraphicEngine, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags msemoryPropertyFlags, VkBuffer *pBuffer, VkDeviceMemory *pDeviceMemory);
void DestroyBuffer(VkDevice vkDevice, VkBuffer vkBuffer, VkDeviceMemory deviceMemory);

void CreateVertexBuffer(GraphicEngine *pGraphicEngine, VkDeviceSize vertexBufferSize, void *vertices, VkBuffer *pVertexBuffer, VkDeviceMemory *pVertexBufferMemory);
void DestroyVertexBuffer(GraphicEngine *pGraphicEngine, VkBuffer vertexBuffer, VkDeviceMemory vertexBufferMemory);

void CreateModelGroup(GraphicEngine *pGraphicEngine, Subpass *pGeometrySubpass, ModelGroup *pModelGroup);
void DestroyModelGroup(GraphicEngine *pGraphicEngine, ModelGroup modelGroup);

void AddModelToSubpass(GraphicEngine *pGraphicEngine, Subpass *pSubpass, uint32_t *pGroupIndex, uint32_t *pModelIndex);
void RemoveModelFromSubpass(GraphicEngine *pGraphicEngine, uint32_t groupIndex, uint32_t modelIndex, Subpass *pSubpass);
