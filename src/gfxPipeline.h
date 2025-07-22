#pragma once
#include "gfxRes.h"

void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void cleanupFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex);
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);

