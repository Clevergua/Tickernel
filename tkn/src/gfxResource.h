#pragma once
#include "gfxCore.h"

DescriptorContent getNullDescriptorContent(VkDescriptorType vkDescriptorType);
DescriptorSet *createDescriptorSetPtr(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set);
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet);
void updateDescriptors(GfxContext *pGfxContext, uint32_t descriptorCount, Descriptor *descriptors);

void resizeDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
void updateInputAttachmentDescriptors(GfxContext *pGfxContext, uint32_t inputAttachmentDescriptorCount, Descriptor *inputAttachmentDescriptors);

Material *createMaterialPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet);
void destroyMaterialPtr(GfxContext *pGfxContext, Material *pMaterial);