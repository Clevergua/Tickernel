#pragma once
#include "gfxRes.h"

void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void cleanupFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex);
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);

// void bindSampler(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Sampler *pSampler);
// void bindCombinedImageSampler(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Sampler *pSampler, Image *pImage);
// void bindSampledImage(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Image *pImage);
// void bindStorageImage(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Image *pImage);
// void bindUniformTexelBuffer(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer);
// void bindStorageTexelBuffer(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer);
// void bindUniformBuffer(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer);
// void bindStorageBuffer(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer);
// void bindUniformBufferDynamic(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer);
// void bindStorageBufferDynamic(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer);
