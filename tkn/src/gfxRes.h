#pragma once
#include <stdio.h>
#include "cglm.h"
#include "tknCore.h"
#include "spirv_reflect.h"

struct Sampler
{
    VkSampler vkSampler;
    VkSamplerCreateInfo vkSamplerCreateInfo;
    TknHashSet descriptorPtrHashSet;
};

struct Image
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
    TknHashSet descriptorPtrHashSet;
};

struct Buffer
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    TknHashSet descriptorPtrHashSet;
    VkDeviceSize size;
};

struct MappedBuffer
{
    Buffer buffer;
    void *mapped;
};

typedef struct
{
    Image *pImage;
    uint32_t width;
    uint32_t height;
} FixedAttachmentContent;

typedef struct
{
    Image *pImage;
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

struct Attachment
{
    AttachmentType attachmentType;
    AttachmentContent attachmentContent;
    TknHashSet renderPassPtrHashSet;
};

typedef struct
{
    Sampler *pSampler;
} SamplerDescriptorContent;

typedef struct
{
    Buffer *pBuffer;
} UniformBufferDescriptorContent;

typedef struct
{
    Attachment *pAttachment;
    VkImageLayout vkImageLayout;
} InputAttachmentDescriptorContent;

typedef union
{
    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    SamplerDescriptorContent samplerDescriptorContent;
    // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
    // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    UniformBufferDescriptorContent uniformBufferDescriptorContent;
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
    // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
    InputAttachmentDescriptorContent inputAttachmentDescriptorContent;
} DescriptorContent;

typedef struct
{
    uint32_t descriptorCount;                    // for update descriptor sets
    Descriptor *descriptors;                     // for update descriptor sets
    VkDescriptorSetLayout vkDescriptorSetLayout; // for creating descriptor set & pipelines
    VkDescriptorPool vkDescriptorPool;           // for creating descriptor
    VkDescriptorSet vkDescriptorSet;             // subpass descriptor set
} DescriptorSet;

struct Descriptor
{
    VkDescriptorType vkDescriptorType;
    DescriptorContent descriptorContent;
    DescriptorSet *pDescriptorSet;
    uint32_t binding;
};

typedef enum
{
    TICKERNEL_MATERIAL_DESCRIPTOR_SET,
    TICKERNEL_SUBPASS_DESCRIPTOR_SET,
    TICKERNEL_GLOBAL_DESCRIPTOR_SET,
    TICKERNEL_MAX_DESCRIPTOR_SET,
} TickernelDescriptorSet;

struct Pipeline
{
    VkPipelineLayout vkPipelineLayout;                // for recording command buffers
    VkDescriptorSetLayout vkDescriptorSetLayout;      // for creating descriptor set
    TknDynamicArray vkDescriptorPoolSizeDynamicArray; // for creating descriptor pool
    VkPipeline vkPipeline;                            // vkPipeline
    TknDynamicArray materialPtrDynamicArray;          // materials
};

typedef struct
{
    DescriptorSet *pSubpassDescriptorSet;    // subpass descriptor set
    TknDynamicArray pipelinePtrDynamicArray; // pipelines
    uint32_t subpassIndex;                   // subpass index in the render pass
} Subpass;

struct RenderPass
{
    VkRenderPass vkRenderPass;
    uint32_t attachmentCount;
    Attachment **attachmentPtrs;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    uint32_t subpassCount;
    Subpass *subpasses;
};

struct GfxContext
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
};

void assertVkResult(VkResult vkResult);
DescriptorContent getNullDescriptorContent(VkDescriptorType vkDescriptorType);
DescriptorSet *createDescriptorSetPtr(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set);
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet);
void updateDescriptors(GfxContext *pGfxContext, uint32_t descriptorCount, Descriptor *descriptors);
