#pragma once
#include <stdbool.h>
#include "vulkan/vulkan.h"

#define TKN_ARRAY_COUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))

typedef struct GfxContext GfxContext;
typedef struct RenderPass RenderPass;
typedef struct Pipeline Pipeline;
typedef struct Material Material;
typedef struct Mesh Mesh;
typedef struct
{
    const char *name;
    VkFormat vkFormat;
    uint32_t count;
} MeshAttributeLayout;

typedef struct
{
    uint32_t vertexLayoutCount;
    MeshAttributeLayout *vertexLayouts;

    uint32_t instanceLayoutCount;
    MeshAttributeLayout *instanceLayouts;

    VkIndexType vkIndexType;
} MeshLayout;

typedef struct Attachment Attachment;
typedef struct Image Image;
typedef struct Sampler Sampler;
typedef struct UniformBuffer UniformBuffer;

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

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, VkClearValue *vkClearValues, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t *spvPathCounts, const char ***spvPathsArray, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies);
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);

Pipeline *createPipelinePtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex, uint32_t spvPathCount, const char **spvPaths, const MeshLayout *pMeshLayout, VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo, VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo, VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo, VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo, VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo, VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo);
void destroyPipelinePtr(GfxContext *pGfxContext, Pipeline *pPipeline);

Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags);
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage);

UniformBuffer *createUniformBufferPtr(GfxContext *pGfxContext, VkDeviceSize vkDeviceSize);
void destroyUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer);
void updateUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer, const void *data, VkDeviceSize vkDeviceSize);

Mesh *createMeshPtr(GfxContext *pGfxContext, void *vertices, uint32_t vertexCount, VkDeviceSize vertexSize, void *indices, uint32_t indexCount, VkIndexType vkIndexType, void *instances, uint32_t maxInstanceCount, VkDeviceSize instanceSize);
void destroyMeshPtr(GfxContext *pGfxContext, Mesh *pMesh);

void tknError(char const *const _Format, ...);
void tknWarning(const char *format, ...);
void tknAssert(bool condition, char const *const _Format, ...);
void *tknMalloc(size_t size);
void tknFree(void *ptr);