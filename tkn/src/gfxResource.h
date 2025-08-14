#pragma once
#include "gfxCore.h"


DescriptorSet *createDescriptorSetPtr(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set);
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet);


void resizeDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
void updateInputAttachmentBindings(GfxContext *pGfxContext, uint32_t inputAttachmentBindingCount, Binding *inputAttachmentBindings);

Material *createMaterialPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet);
void destroyMaterialPtr(GfxContext *pGfxContext, Material *pMaterial);
BindingUnion getNullBindingUnion(VkDescriptorType vkDescriptorType);
void updateInputAttachmentBindings(GfxContext *pGfxContext, uint32_t inputAttachmentBindingCount, Binding *inputAttachmentBindings);
void updateBindings(GfxContext *pGfxContext, uint32_t bindingCount, Binding *bindings);