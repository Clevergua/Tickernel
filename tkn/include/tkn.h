#pragma once
#include <stdbool.h>
#include "vulkan/vulkan.h"

#define TKN_ARRAY_COUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))

typedef struct GfxContext GfxContext;
typedef struct RenderPass RenderPass;
typedef struct Pipeline Pipeline;
typedef struct Descriptor Descriptor;

typedef struct Attachment Attachment;
typedef struct Image Image;
typedef struct Sampler Sampler;
typedef struct Buffer Buffer;
typedef struct MappedBuffer MappedBuffer;

VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

GfxContext *createGfxContextPtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, uint32_t spvPathCount, const char **spvPaths);
void waitGfxContextPtr(GfxContext *pGfxContext);
void updateGfxContextPtr(GfxContext *pGfxContext, VkExtent2D swapchainExtent);
void destroyGfxContextPtr(GfxContext *pGfxContext);

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t *spvPathCounts, const char ***spvPathsArray, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies);
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);

Attachment *createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkImageAspectFlags vkImageAspectFlags, float scaler);
void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);

Attachment *createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height);
void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);

Attachment *getSwapchainAttachmentPtr(GfxContext *pGfxContext);

Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags);
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage);

MappedBuffer *createUniformBufferPtr(GfxContext *pGfxContext, VkDeviceSize size);
void destroyMappedBufferPtr(GfxContext *pGfxContext, MappedBuffer *pMappedBuffer);
void updateMappedBufferPtr(GfxContext *pGfxContext, MappedBuffer *pMappedBuffer, const void *data, VkDeviceSize size);

void tknError(char const *const _Format, ...);
void tknAssert(bool condition, char const *const _Format, ...);
void tknSleep(uint32_t milliseconds);
void *tknMalloc(size_t size);
void tknFree(void *ptr);