#pragma once
#include <stdbool.h>
#include "vulkan/vulkan.h"

typedef struct GfxContext GfxContext;
typedef struct RenderPass RenderPass;
typedef struct Attachment Attachment;
typedef struct Image Image;
typedef struct Sampler Sampler;

VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

GfxContext *createGfxContextPtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent);
void waitGfxContextPtr(GfxContext *pGfxContext);
void updateGfxContextPtr(GfxContext *pGfxContext, VkExtent2D swapchainExtent);
void destroyGfxContextPtr(GfxContext *pGfxContext);

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t spvPathCount, const char **spvPaths, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex);
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);

Attachment *createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler);
void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);

Attachment *createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height);
void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);

Attachment *getSwapchainAttachmentPtr(GfxContext *pGfxContext);

Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags);
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage);

void tknError(char const *const _Format, ...);
void tknAssert(bool condition, char const *const _Format, ...);
void tknSleep(uint32_t milliseconds);
void *tknMalloc(size_t size);
void tknFree(void *ptr);