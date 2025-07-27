#pragma once
#include "cglm.h"
#include "tknCore.h"
#include "spirv_reflect.h"

#define ASSERT_VK_SUCCESS(result) tknAssert((result) == VK_SUCCESS, "Vulkan error: %d", (result))

typedef struct
{
    VkSampler vkSampler;
    VkSamplerCreateInfo vkSamplerCreateInfo;
    TknHashSet descriptorBindingPtrHashSet;
} Sampler;

typedef struct
{
    VkImage vkImage;
    VkDeviceMemory vkDeviceMemory;
    VkImageView vkImageView;
    VkExtent3D vkExtent3D;
    VkFormat vkFormat;
    VkImageTiling vkImageTiling;
    VkImageUsageFlags vkImageUsageFlags;
    VkMemoryPropertyFlags vkMemoryPropertyFlags;
    VkImageAspectFlags vkImageAspectFlags;
    TknHashSet descriptorBindingPtrHashSet;
} Image;

typedef struct
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    void *mapped;
    TknHashSet descriptorBindingPtrHashSet;
} Buffer;

typedef struct
{
    Image image;
    uint32_t width;
    uint32_t height;
} FixedAttachmentContent;

typedef struct
{
    Image image;
    float32_t scaler;
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
    Sampler *activeSamplerPtr;
    Sampler *pendingSamplerPtr;
} SamplerDescriptorBinding;

// typedef struct
// {
//     Image *currentImagePtr;
//     Image *nextImagePtr;
//     Sampler *currentSamplerPtr;
//     Sampler *nextSamplerPtr;
// } CombinedImageSamplerDescriptorBinding;

// typedef struct
// {
//     Image *pImage;
// } SampledImageDescriptorBinding;

// typedef struct
// {
//     Image *pImage;
// } StorageImageDescriptorBinding;

// typedef struct
// {
//     Buffer *pBuffer;
// } UniformTexelBufferDescriptorBinding;

// typedef struct
// {
//     Buffer *pBuffer;
// } StorageTexelBufferDescriptorBinding;

// typedef struct
// {
//     Buffer *pBuffer;
// } UniformBufferDescriptorBinding;

// typedef struct
// {
//     Buffer *pBuffer;
// } StorageBufferDescriptorBinding;

// typedef struct
// {
//     Buffer *pBuffer;
// } UniformBufferDynamicDescriptorBinding;

// typedef struct
// {
//     Buffer *pBuffer;
// } StorageBufferDynamicDescriptorBinding;

typedef struct
{
    Attachment *pAttachment;
    VkImageLayout vkImageLayout;
} InputAttachmentDescriptorBinding;

typedef union
{
    SamplerDescriptorBinding samplerDescriptorBinding;
    // CombinedImageSamplerDescriptorBinding combinedImageSamplerDescriptorBinding;
    // SampledImageDescriptorBinding sampledImageDescriptorBinding;
    // StorageImageDescriptorBinding storageImageDescriptorBinding;
    // UniformTexelBufferDescriptorBinding uniformTexelBufferDescriptorBinding;
    // StorageTexelBufferDescriptorBinding storageTexelBufferDescriptorBinding;
    // UniformBufferDescriptorBinding uniformBufferDescriptorBinding;
    // StorageBufferDescriptorBinding storageBufferDescriptorBinding;
    // UniformBufferDynamicDescriptorBinding uniformBufferDynamicDescriptorBinding;
    // StorageBufferDynamicDescriptorBinding storageBufferDynamicDescriptorBinding;
    InputAttachmentDescriptorBinding inputAttachmentDescriptorBinding;
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
    VkPipelineLayout vkPipelineLayout;                // for recording command buffers
    VkDescriptorSetLayout vkDescriptorSetLayout;      // for creating descriptor set
    TknDynamicArray vkDescriptorPoolSizeDynamicArray; // for creating descriptor pool
    VkPipeline vkPipeline;                            // vkPipeline
    TknDynamicArray materialPtrDynamicArray;          // materials
} Pipeline;

typedef struct
{
    uint32_t descriptorBindingCount;                             // for update descriptor sets
    DescriptorBinding *descriptorBindings;                       // for update descriptor sets
    VkDescriptorSetLayoutBinding *vkDescriptorSetLayoutBindings; // for creating descriptor set layout
    VkDescriptorSetLayout vkDescriptorSetLayout;                 // for creating descriptor set & pipelines
    VkDescriptorPool vkDescriptorPool;                           // for creating descriptor set
    VkDescriptorSet vkDescriptorSet;                             // subpass descriptor set
} DescriptorSet;

typedef struct
{
    DescriptorSet subpassDescriptorSet;      // subpass descriptor set
    TknDynamicArray pipelinePtrDynamicArray; // pipelines
    uint32_t subpassIndex;                   // subpass index in the render pass
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
    bool isValid;
} RenderPass;

typedef struct
{
    uint32_t frameCount;
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

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;

    VkCommandPool gfxVkCommandPool;
    VkCommandBuffer *gfxVkCommandBuffers;

    TknDynamicArray dynamicAttachmentPtrDynamicArray;
    TknDynamicArray renderPassPtrDynamicArray;

    DescriptorSet globalDescriptorSet;
} GfxContext;
