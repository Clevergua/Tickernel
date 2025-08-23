#pragma once
#include <stdio.h>
#include "cglm.h"
#include "tknCore.h"
#include "spirv_reflect.h"

struct Sampler
{
    VkSampler vkSampler;
    TknHashSet bindingPtrHashSet;
};

struct Image
{
    VkImage vkImage;
    VkDeviceMemory vkDeviceMemory;
    VkImageView vkImageView;
    TknHashSet bindingPtrHashSet;
};

struct UniformBuffer
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    void *mapped;
    TknHashSet bindingPtrHashSet;
    VkDeviceSize size;
};
struct StorageBuffer
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    TknHashSet bindingPtrHashSet;
    VkDeviceSize size;
};

struct UniformTexelBuffer
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    TknHashSet bindingPtrHashSet;
    VkDeviceSize size;
};
struct StorageTexelBuffer
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    TknHashSet bindingPtrHashSet;
    VkDeviceSize size;
};

struct UniformDynamicBuffer
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    void *mapped;
    TknHashSet bindingPtrHashSet;
    VkDeviceSize size;
};
struct StorageDynamicBuffer
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    void *mapped;
    TknHashSet bindingPtrHashSet;
    VkDeviceSize size;
};

typedef struct
{
    VkImage vkImage;
    VkDeviceMemory vkDeviceMemory;
    VkImageView vkImageView;
    uint32_t width;
    uint32_t height;
    TknHashSet bindingPtrHashSet;
} FixedAttachment;

typedef struct
{
    VkImage vkImage;
    VkDeviceMemory vkDeviceMemory;
    VkImageView vkImageView;
    float32_t scaler;
    VkImageUsageFlags vkImageUsageFlags;
    VkImageAspectFlags vkImageAspectFlags;
    TknHashSet bindingPtrHashSet;
} DynamicAttachment;

typedef struct
{
    VkExtent2D swapchainExtent;
    VkSwapchainKHR vkSwapchain;
    uint32_t swapchainImageCount;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;
} SwapchainAttachment;

typedef union
{
    FixedAttachment fixedAttachment;
    DynamicAttachment dynamicAttachment;
    SwapchainAttachment swapchainAttachment;
} AttachmentUnion;

typedef enum
{
    ATTACHMENT_TYPE_DYNAMIC,
    ATTACHMENT_TYPE_FIXED,
    ATTACHMENT_TYPE_SWAPCHAIN,
} AttachmentType;

struct Attachment
{
    AttachmentType attachmentType;
    AttachmentUnion attachmentUnion;
    VkFormat vkFormat;
    TknHashSet renderPassPtrHashSet;
};

typedef struct
{
    Sampler *pSampler;
} SamplerBinding;

typedef struct
{
    UniformBuffer *pUniformBuffer;
} UniformBufferBinding;

typedef struct
{
    Attachment *pAttachment;
    VkImageLayout vkImageLayout;
} InputAttachmentBinding;

typedef union
{
    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    SamplerBinding samplerBinding;
    // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
    // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    UniformBufferBinding uniformBufferBinding;
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
    // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
    InputAttachmentBinding inputAttachmentBinding;
} BindingUnion;

typedef struct
{
    VkDescriptorType vkDescriptorType;
    BindingUnion bindingUnion;
    Material *pMaterial;
    uint32_t binding;
} Binding;

typedef struct
{
    TknDynamicArray materialPtrDynamicArray;
    VkDescriptorSetLayout vkDescriptorSetLayout;
    TknDynamicArray vkDescriptorPoolSizeDynamicArray;
    uint32_t descriptorCount;
    VkDescriptorType *vkDescriptorTypes;
} DescriptorSet;

typedef enum
{
    VERTEX_BINDING_DESCRIPTION,
    INSTANCE_BINDING_DESCRIPTION,
    MAX_VERTEX_BINDING_DESCRIPTION
} VertexBindingDescription;



struct MeshLayout
{
    uint32_t vertexAttributeLayoutCount;
    AttributeLayout *vertexAttributeLayouts;
    VkIndexType vkIndexType;

    TknHashSet meshPtrHashSet;
    TknHashSet pipelinePtrHashSet;
};

struct Instance
{
    VkBuffer instanceVkBuffer;
    VkDeviceMemory instanceVkDeviceMemory;
    void *instanceMappedBuffer;
    uint32_t instanceCount;
    uint32_t maxInstanceCount;
};

struct Mesh
{
    VkBuffer vertexVkBuffer;
    VkDeviceMemory vertexVkDeviceMemory;
    uint32_t vertexCount;

    VkBuffer indexVkBuffer;
    VkDeviceMemory indexVkDeviceMemory;
    uint32_t indexCount;

    MeshLayout *pMeshLayout;
};

struct Material
{
    VkDescriptorSet vkDescriptorSet;
    uint32_t bindingCount;
    Binding *bindings;
    VkDescriptorPool vkDescriptorPool;
    DescriptorSet *pDescriptorSet;
    TknDynamicArray instancePtrDynamicArray;
    TknDynamicArray meshPtrDynamicArray;
};

typedef enum
{
    TKN_GLOBAL_DESCRIPTOR_SET,
    TKN_SUBPASS_DESCRIPTOR_SET,
    TKN_PIPELINE_DESCRIPTOR_SET,
    TKN_MAX_DESCRIPTOR_SET,
} TickernelDescriptorSet;

struct Pipeline
{
    VkPipeline vkPipeline;
    DescriptorSet *pPipelineDescriptorSet;
    VkPipelineLayout vkPipelineLayout;

    MeshLayout *pMeshLayout;
};

typedef struct
{
    DescriptorSet *pSubpassDescriptorSet;
    TknDynamicArray pipelinePtrDynamicArray;
} Subpass;

struct RenderPass
{
    VkRenderPass vkRenderPass;
    uint32_t attachmentCount;
    Attachment **attachmentPtrs;
    VkClearValue *vkClearValues;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    VkRect2D renderArea;
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

    VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
    Attachment *pSwapchainAttachment;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence renderFinishedFence;

    VkCommandPool gfxVkCommandPool;
    VkCommandBuffer *gfxVkCommandBuffers;

    TknDynamicArray dynamicAttachmentPtrDynamicArray;
    TknDynamicArray renderPassPtrDynamicArray;

    DescriptorSet *pGlobalDescriptorSet;
};

void assertVkResult(VkResult vkResult);
SpvReflectShaderModule createSpvReflectShaderModule(const char *filePath);
void destroySpvReflectShaderModule(SpvReflectShaderModule *pSpvReflectShaderModule);
size_t getSizeOfVkFormat(VkFormat format);