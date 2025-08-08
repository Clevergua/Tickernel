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

typedef struct
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    void *mapped;
    TknHashSet descriptorPtrHashSet;
} Buffer;

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
};

typedef struct
{
    Sampler *pSampler;
} SamplerDescriptorContent;

typedef struct
{
    Attachment *pAttachment;
    VkImageLayout vkImageLayout;
} InputAttachmentDescriptorContent;

typedef union
{
    SamplerDescriptorContent samplerDescriptorContent;
    InputAttachmentDescriptorContent inputAttachmentDescriptorContent;
} DescriptorContent;

typedef struct
{
    VkDescriptorType vkDescriptorType;
    DescriptorContent descriptorContent;
    struct DescriptorSet *pDescriptorSet;
    uint32_t binding;
} Descriptor;

struct DescriptorBinding
{
    VkDescriptorType vkDescriptorType;
    DescriptorContent descriptorContent;
    uint32_t binding;
};

typedef struct DescriptorSet
{
    uint32_t descriptorCount;                    // for update descriptor sets
    Descriptor *descriptors;                     // for update descriptor sets
    VkDescriptorSetLayout vkDescriptorSetLayout; // for creating descriptor set & pipelines
    VkDescriptorPool vkDescriptorPool;           // for creating descriptor
    VkDescriptorSet vkDescriptorSet;             // subpass descriptor set
} DescriptorSet;

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
    DescriptorSet *pSubpassDescriptorSet;    // subpass descriptor set
    TknDynamicArray pipelinePtrDynamicArray; // pipelines
    uint32_t subpassIndex;                   // subpass index in the render pass
} Subpass;

struct RenderPass
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
DescriptorBinding getNullDescriptorBinding(GfxContext *pGfxContext, VkDescriptorType vkDescriptorType, uint32_t binding);

DescriptorSet *createDescriptorSetPtr(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set);
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet);
void updateDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t bindingCount, DescriptorBinding *bindings);
