#pragma once
#include "gfxRes.h"



void createFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void destroyFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);

void createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass);
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);
