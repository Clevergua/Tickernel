#pragma once
#include "gfxRes.h"

void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void cleanupFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);

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
