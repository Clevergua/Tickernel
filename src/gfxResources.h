#pragma once
#include "cglm.h"
#include "tknCore.h"
#include "spirv_reflect.h"

#define ASSERT_VK_SUCCESS(result) tknAssert((result) == VK_SUCCESS, "Vulkan error: %d", (result))

typedef struct
{
    VkSampler vkSampler;
} Sampler;

typedef struct
{
    VkImage vkImage;
    VkDeviceMemory vkDeviceMemory;
    VkImageView vkImageView;
} Image;

typedef struct
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    void *mapped;
} Buffer;

typedef struct
{
    Image image;
    VkFormat vkFormat;
    uint32_t width;
    uint32_t height;
} FixedAttachmentContent;

typedef struct
{
    Image image;
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

typedef struct
{
    Sampler *pSampler;
} SamplerDescriptorBinding;

typedef struct
{
    Image *pImage;
    Sampler *pSampler;
} CombinedImageSamplerDescriptorBinding;

typedef struct
{
    Image *pImage;
} SampledImageDescriptorBinding;

typedef struct
{
    Image *pImage;
} StorageImageDescriptorBinding;

typedef struct
{
    Buffer *pBuffer;
} UniformTexelBufferDescriptorBinding;

typedef struct
{
    Buffer *pBuffer;
} StorageTexelBufferDescriptorBinding;

typedef struct
{
    Buffer *pBuffer;
} UniformBufferDescriptorBinding;

typedef struct
{
    Buffer *pBuffer;
} StorageBufferDescriptorBinding;

typedef struct
{
    Buffer *pBuffer;
} UniformBufferDynamicDescriptorBinding;

typedef struct
{
    Buffer *pBuffer;
} StorageBufferDynamicDescriptorBinding;

typedef struct
{
    Attachment *pAttachment;
} InputAttachmentDescriptorBinding;

typedef union
{
    SamplerDescriptorBinding samplerDescriptorBinding;
    CombinedImageSamplerDescriptorBinding combinedImageSamplerDescriptorBinding;
    SampledImageDescriptorBinding sampledImageDescriptorBinding;
    StorageImageDescriptorBinding storageImageDescriptorBinding;
    UniformTexelBufferDescriptorBinding uniformTexelBufferDescriptorBinding;
    StorageTexelBufferDescriptorBinding storageTexelBufferDescriptorBinding;
    UniformBufferDescriptorBinding uniformBufferDescriptorBinding;
    StorageBufferDescriptorBinding storageBufferDescriptorBinding;
    UniformBufferDynamicDescriptorBinding uniformBufferDynamicDescriptorBinding;
    StorageBufferDynamicDescriptorBinding storageBufferDynamicDescriptorBinding;
    InputAttachmentDescriptorBinding inputAttachmentDescriptorBinding;
} DescriptorBindingContent;

typedef struct
{
    VkWriteDescriptorSet vkWriteDescriptorSet;
    DescriptorBindingContent descriptorBindingContent;
} DescriptorBinding;

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
    uint32_t gfxQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;

    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;

    VkDevice vkDevice;
    VkQueue vkGfxQueue;
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

    VkCommandPool gfxVkCommandPool;
    VkCommandBuffer *gfxVkCommandBuffers;

    TknDynamicArray dynamicAttachmentPtrDynamicArray;
    TknDynamicArray renderPassPtrDynamicArray;
} GfxContext;

void createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment **ppAttachment);
void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);

void createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment **ppAttachment);
void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);

void getSwapchainAttachmentPtr(GfxContext *pGfxContext, uint32_t *pSwapchainAttachmentCount, Attachment **pAttachments);

void createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, Image **ppImage);
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage);

