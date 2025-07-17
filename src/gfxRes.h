#pragma once
#include "gfxCommon.h"

void createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment **ppAttachment);
void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);

void createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment **ppAttachment);
void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
void resizeDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);

void getSwapchainAttachmentPtr(GfxContext *pGfxContext, uint32_t *pSwapchainAttachmentCount, Attachment **pAttachments);

void createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, Image **ppImage);
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage);


