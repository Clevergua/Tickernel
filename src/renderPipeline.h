#pragma once
#include "cglm.h"
#include "tknCore.h"
#include "spirv_reflect.h"

#define ASSERT_VK_SUCCESS(result) tknAssert((result) == VK_SUCCESS, "Vulkan error: %d", (result))

typedef struct
{
    VkImage vkImage;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
    VkFormat vkFormat;
    uint32_t width;
    uint32_t height;
} FixedAttachmentContent;

typedef struct
{
    VkImage vkImage;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
    VkFormat vkFormat;
    float32_t scaler;
    VkImageUsageFlags vkImageUsageFlags;
    VkMemoryPropertyFlags vkMemoryPropertyFlags;
    VkImageAspectFlags vkImageAspectFlags;
} DynamicAttachmentContent;

typedef struct
{
} SwapchainAttachmentContent;

typedef union
{
    FixedAttachmentContent fixedAttachmentContent;
    DynamicAttachmentContent dynamicAttachmentContent;
    SwapchainAttachmentContent swapchainAttachmentContent;
} AttachmentContent;

typedef enum
{
    ATTACHMENT_TYPE_DYNAMIC,
    ATTACHMENT_TYPE_FIXED,
    ATTACHMENT_TYPE_SWAPCHAIN,
} AttachmentType;

typedef struct
{
    AttachmentType attachmentType;
    AttachmentContent attachmentContent;
} Attachment;

typedef enum
{
    TICKERNEL_MATERIAL_DESCRIPTOR_SET,
    TICKERNEL_SUBPASS_DESCRIPTOR_SET,
    TICKERNEL_GLOBAL_DESCRIPTOR_SET,
    TICKERNEL_MAX_DESCRIPTOR_SET,
} TickernelDescriptorSet;

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
    Attachment *swapchainAttachmentPtr;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;
    uint32_t swapchainIndex;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;

    VkCommandPool graphicsVkCommandPool;
    VkCommandBuffer *graphicsVkCommandBuffers;

    TknDynamicArray dynamicAttachmentPtrDynamicArray;
    TknDynamicArray renderPassPtrDynamicArray;

} GraphicsContext;

typedef struct PipelineStruct
{
    VkPipelineLayout vkPipelineLayout;                // for recording command buffers
    VkDescriptorSetLayout vkDescriptorSetLayout;      // for creating descriptor set
    TknDynamicArray vkDescriptorPoolSizeDynamicArray; // for creating descriptor pool
    VkPipeline vkPipeline;                            // vkPipeline
    TknDynamicArray materialPtrDynamicArray;          // materials
} Pipeline;

typedef struct
{
    TknDynamicArray pipelinePtrDynamicArray;          // pipelines
    TknDynamicArray vkWriteDescriptorSetDynamicArray; // for rewriting descriptor sets
    TknDynamicArray inputAttachmentIndexDynamicArray; // for rewriting descriptor sets
    VkDescriptorSetLayout vkDescriptorSetLayout;      // for creating descriptor set & pipelines
    VkDescriptorPool vkDescriptorPool;                // for creating descriptor set
    VkDescriptorSet vkDescriptorSet;                  // subpass descriptor set
} Subpass;

typedef struct
{
    VkRenderPass vkRenderPass;
    uint32_t attachmentCount;
    Attachment **attachmentPtrs;
    bool useSwapchain;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    uint32_t subpassCount;
    Subpass *subpasses;
} RenderPass;

void createFramebuffers(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);
void destroyFramebuffers(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);

void createDynamicAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment **ppAttachment);
void destroyDynamicAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment);
void resizeDynamicAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment);

void createFixedAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment **ppAttachment);
void destroyFixedAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment);

void getSwapchainAttachment(GraphicsContext *pGraphicsContext, uint32_t *pSwapchainAttachmentCount, Attachment **pAttachments);

void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass);
void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);
