#pragma once
#include "gfxRes.h"

bool hasSwapchain(RenderPass renderPass);
void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void cleanupFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void updateInputAttachmentDescriptors(GfxContext *pGfxContext, uint32_t attachmentCount, Attachment **attachmentPtrs, const VkImageLayout *inputAttachmentIndexToVkImageLayout, uint32_t inputAttachmentDescriptorCount, Descriptor *inputAttachmentDescriptors);