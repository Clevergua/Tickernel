#pragma once
#include "cglm.h"
#include "tknCore.h"
#include "spirv_reflect.h"

#define ASSERT_VK_SUCCESS(result) tknAssert((result) == VK_SUCCESS, "Vulkan error: %d", (result))

typedef struct
{
    VkInstance vkInstance;
    VkSurfaceKHR vkSurface;

    VkPhysicalDevice vkPhysicalDevice;
    VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;

    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;

    VkDevice vkDevice;
    VkQueue vkGraphicsQueue;
    VkQueue vkPresentQueue;

    VkExtent2D swapchainExtent;
    VkSwapchainKHR vkSwapchain;
    uint32_t swapchainImageCount;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;
    uint32_t swapchainIndex;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;

    VkCommandPool graphicsVkCommandPool;
    VkCommandBuffer *graphicsVkCommandBuffers;

    TknDynamicArray renderPassPtrDynamicArray;
} GraphicsContext;

typedef struct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
} RenderPass;

void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **pAttachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass);
void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);
