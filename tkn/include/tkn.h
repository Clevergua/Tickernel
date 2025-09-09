#pragma once
#include <stdbool.h>
#include "vulkan/vulkan.h"

#define TKN_ARRAY_COUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))

typedef struct GfxContext GfxContext;
typedef struct RenderPass RenderPass;
typedef struct VertexInputLayout VertexInputLayout;
typedef struct Pipeline Pipeline;
typedef struct Material Material;
typedef struct Instance Instance;
typedef struct Mesh Mesh;
typedef struct DrawCall DrawCall;

typedef struct Attachment Attachment;
typedef struct Image Image;
typedef struct Sampler Sampler;
typedef struct UniformBuffer UniformBuffer;

typedef struct
{
    Sampler *pSampler;
} SamplerBinding;

typedef struct
{
    UniformBuffer *pUniformBuffer;
} UniformBufferBinding;

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
} InputBindingUnion;

typedef struct
{
    VkDescriptorType vkDescriptorType;
    InputBindingUnion inputBindingUnion;
    uint32_t binding;
} InputBinding;

VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

GfxContext *createGfxContextPtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, uint32_t spvPathCount, const char **spvPaths);
void waitGfxContextPtr(GfxContext *pGfxContext);
void updateGfxContextPtr(GfxContext *pGfxContext, VkExtent2D swapchainExtent);
void destroyGfxContextPtr(GfxContext *pGfxContext);

Attachment *createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkImageAspectFlags vkImageAspectFlags, float scaler);
void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
Attachment *createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height);
void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
Attachment *getSwapchainAttachmentPtr(GfxContext *pGfxContext);

VertexInputLayout *createVertexInputLayoutPtr(GfxContext *pGfxContext, uint32_t attributeCount, const char **names, uint32_t *sizes);
void destroyVertexInputLayoutPtr(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout);

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, VkClearValue *vkClearValues, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t *spvPathCounts, const char ***spvPathsArray, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex);
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);

Pipeline *createPipelinePtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex, uint32_t spvPathCount, const char **spvPaths, VertexInputLayout *pMeshVertexInputLayout, VertexInputLayout *pInstanceVertexInputLayout, VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo, VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo, VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo, VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo, VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo, VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo);
void destroyPipelinePtr(GfxContext *pGfxContext, Pipeline *pPipeline);

DrawCall *addDrawCallPtr(GfxContext *pGfxContext, Pipeline *pPipeline, Material *pMaterial, Mesh *pMesh, Instance *pInstance);
void removeDrawCallPtr(GfxContext *pGfxContext, DrawCall *pDrawCall);
void clearDrawCalls(GfxContext *pGfxContext, Pipeline *pPipeline);

Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags);
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage);

UniformBuffer *createUniformBufferPtr(GfxContext *pGfxContext, const void *data, VkDeviceSize size);
void destroyUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer);
void updateUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer, const void *data, VkDeviceSize size);

Mesh *createMeshPtr(GfxContext *pGfxContext, VertexInputLayout *pMeshVertexInputLayout, void *vertices, uint32_t vertexCount, VkIndexType vkIndexType, void *indices, uint32_t indexCount);
void destroyMeshPtr(GfxContext *pGfxContext, Mesh *pMesh);

Instance *createInstancePtr(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout, uint32_t instanceCount, void *instances);
void destroyInstancePtr(GfxContext *pGfxContext, Instance *pInstance);

Material *getGlobalMaterialPtr(GfxContext *pGfxContext);
Material *getSubpassMaterialPtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex);
Material *createPipelineMaterialPtr(GfxContext *pGfxContext, Pipeline *pPipeline);
void destroyPipelineMaterialPtr(GfxContext *pGfxContext, Material *pMaterial);
void updateMaterialPtr(GfxContext *pGfxContext, Material *pMaterial, uint32_t inputBindingCount, InputBinding *inputBindings);
InputBindingUnion getEmptyInputBindingUnion(GfxContext *pGfxContext, VkDescriptorType vkDescriptorType);

void tknError(char const *const _Format, ...);
void tknWarning(const char *format, ...);
void tknAssert(bool condition, char const *const _Format, ...);
void *tknMalloc(size_t size);
void tknFree(void *ptr);