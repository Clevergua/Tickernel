#pragma once
#include "gfxCore.h"

BindingUnion getNullBindingUnion(VkDescriptorType vkDescriptorType);
DescriptorSet *createDescriptorSetPtr(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set);
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet);
void updateBindings(GfxContext *pGfxContext, uint32_t bindingCount, Binding *bindings);

void resizeDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
void updateInputAttachmentBindings(GfxContext *pGfxContext, uint32_t inputAttachmentBindingCount, Binding *inputAttachmentBindings);

Material *createMaterialPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet);
void destroyMaterialPtr(GfxContext *pGfxContext, Material *pMaterial);