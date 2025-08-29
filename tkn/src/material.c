#include "gfxCore.h"
Material *createMaterialPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet)
{
    Material *pMaterial = tknMalloc(sizeof(Material));
    VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
    uint32_t descriptorCount = pDescriptorSet->descriptorCount;
    Binding *bindings = tknMalloc(sizeof(Binding) * descriptorCount);
    VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;

    for (uint32_t descriptorIndex = 0; descriptorIndex < descriptorCount; descriptorIndex++)
    {
        VkDescriptorType vkDescriptorType = pDescriptorSet->vkDescriptorTypes[descriptorIndex];
        bindings[descriptorIndex] = (Binding){
            .vkDescriptorType = vkDescriptorType,
            .bindingUnion = getNullBindingUnion(vkDescriptorType),
            .pMaterial = pMaterial,
            .binding = descriptorIndex,
        };
    }
    VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = pDescriptorSet->vkDescriptorPoolSizeDynamicArray.count,
        .pPoolSizes = pDescriptorSet->vkDescriptorPoolSizeDynamicArray.array,
        .maxSets = 1,
    };
    VkDevice vkDevice = pGfxContext->vkDevice;
    assertVkResult(vkCreateDescriptorPool(vkDevice, &vkDescriptorPoolCreateInfo, NULL, &vkDescriptorPool));

    VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pDescriptorSet->vkDescriptorSetLayout,
    };
    assertVkResult(vkAllocateDescriptorSets(vkDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet));
    *pMaterial = (Material){
        .vkDescriptorSet = vkDescriptorSet,
        .bindingCount = descriptorCount,
        .bindings = bindings,
        .vkDescriptorPool = vkDescriptorPool,
        .pDescriptorSet = pDescriptorSet,
    };
    tknAddToDynamicArray(&pDescriptorSet->materialPtrDynamicArray, &pMaterial);
    return pMaterial;
}
void destroyMaterialPtr(GfxContext *pGfxContext, Material *pMaterial)
{
    tknRemoveFromDynamicArray(&pMaterial->pDescriptorSet->materialPtrDynamicArray, &pMaterial);
    VkDevice vkDevice = pGfxContext->vkDevice;
    uint32_t descriptorCount = 0;
    Binding *bindings = tknMalloc(sizeof(Binding) * pMaterial->bindingCount);
    for (uint32_t binding = 0; binding < pMaterial->bindingCount; binding++)
    {
        VkDescriptorType descriptorType = pMaterial->bindings[binding].vkDescriptorType;
        if (descriptorType != VK_DESCRIPTOR_TYPE_MAX_ENUM && descriptorType != VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
        {
            bindings[descriptorCount] = (Binding){
                .vkDescriptorType = descriptorType,
                .bindingUnion = getNullBindingUnion(descriptorType),
                .pMaterial = pMaterial,
                .binding = binding,
            };
            descriptorCount++;
        }
        else
        {
            // Skip
        }
    }
    if (descriptorCount > 0)
    {
        updateBindings(pGfxContext, descriptorCount, bindings);
    }

    tknFree(pMaterial->bindings);
    vkDestroyDescriptorPool(vkDevice, pMaterial->vkDescriptorPool, NULL);
    tknFree(pMaterial);
}

BindingUnion getNullBindingUnion(VkDescriptorType vkDescriptorType)
{
    BindingUnion bindingUnion = {0};
    // VK_DESCRIPTOR_TYPE_SAMPLER = 0,
    if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
    {
        bindingUnion.samplerBinding.pSampler = NULL;
    }
    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
    // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER == vkDescriptorType)
    {
        bindingUnion.uniformBufferBinding.pUniformBuffer = NULL;
    }
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
    // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
    else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorType)
    {
        bindingUnion.inputAttachmentBinding.pAttachment = NULL;
        bindingUnion.inputAttachmentBinding.vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    else
    {
        tknError("Unsupported descriptor type: %d", vkDescriptorType);
    }
    return bindingUnion;
}

void updateBindings(GfxContext *pGfxContext, uint32_t bindingCount, Binding *bindings)
{
    if (bindingCount > 0)
    {
        Material *pMaterial = bindings[0].pMaterial;
        tknAssert(NULL != pMaterial, "Material must not be NULL");
        uint32_t vkWriteDescriptorSetCount = 0;
        VkWriteDescriptorSet *vkWriteDescriptorSets = tknMalloc(sizeof(VkWriteDescriptorSet) * bindingCount);
        VkDescriptorImageInfo *vkDescriptorImageInfos = tknMalloc(sizeof(VkDescriptorImageInfo) * bindingCount);
        VkDescriptorBufferInfo *vkDescriptorBufferInfos = tknMalloc(sizeof(VkDescriptorBufferInfo) * bindingCount);
        for (uint32_t bindingIndex = 0; bindingIndex < bindingCount; bindingIndex++)
        {
            Binding newBinding = bindings[bindingIndex];
            tknAssert(newBinding.pMaterial == pMaterial, "All bindings must belong to the same descriptor set");
            uint32_t binding = newBinding.binding;
            tknAssert(binding < pMaterial->bindingCount, "Invalid binding index");
            VkDescriptorType vkDescriptorType = pMaterial->bindings[binding].vkDescriptorType;
            tknAssert(vkDescriptorType == newBinding.vkDescriptorType, "Incompatible descriptor type");
            Binding *pOldBinding = &pMaterial->bindings[binding];
            // VK_DESCRIPTOR_TYPE_SAMPLER = 0,
            // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
            // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
            // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
            // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
            // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
            // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
            // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
            // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
            // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
            // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10
            if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
            {
                Sampler *pNewSampler = newBinding.bindingUnion.samplerBinding.pSampler;
                Sampler *pCurrentSampler = pOldBinding->bindingUnion.samplerBinding.pSampler;
                if (pNewSampler == pCurrentSampler)
                {
                    // No change, skip
                }
                else
                {
                    if (NULL == pCurrentSampler)
                    {
                        // Nothing
                    }
                    else
                    {
                        // Current sampler deref descriptor
                        tknRemoveFromHashSet(&pCurrentSampler->bindingPtrHashSet, pOldBinding);
                    }

                    pOldBinding->bindingUnion.samplerBinding.pSampler = pNewSampler;
                    if (NULL == pNewSampler)
                    {
                        vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                            .sampler = NULL,
                            .imageView = VK_NULL_HANDLE,
                            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        };
                    }
                    else
                    {
                        // New sampler ref descriptor
                        tknAddToHashSet(&pNewSampler->bindingPtrHashSet, pOldBinding);
                        vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                            .sampler = pNewSampler->vkSampler,
                            .imageView = VK_NULL_HANDLE,
                            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        };
                    }
                    VkWriteDescriptorSet vkWriteDescriptorSet = {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = pMaterial->vkDescriptorSet,
                        .dstBinding = binding,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = vkDescriptorType,
                        .pImageInfo = &vkDescriptorImageInfos[vkWriteDescriptorSetCount],
                        .pBufferInfo = NULL,
                        .pTexelBufferView = NULL,
                    };
                    vkWriteDescriptorSets[vkWriteDescriptorSetCount] = vkWriteDescriptorSet;
                    vkWriteDescriptorSetCount++;
                }
            }
            else if (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == vkDescriptorType)
            {
                tknError("Combined image sampler not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE == vkDescriptorType)
            {
                tknError("Sampled image not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_STORAGE_IMAGE == vkDescriptorType)
            {
                tknError("Storage image not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER == vkDescriptorType)
            {
                tknError("Uniform texel buffer not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER == vkDescriptorType)
            {
                tknError("Storage texel buffer not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER == vkDescriptorType)
            {
                UniformBuffer *pNewUniformBuffer = newBinding.bindingUnion.uniformBufferBinding.pUniformBuffer;
                Binding *pCurrentBinding = &pMaterial->bindings[binding];
                UniformBuffer *pCurrentUniformBuffer = pCurrentBinding->bindingUnion.uniformBufferBinding.pUniformBuffer;
                if (pNewUniformBuffer == pCurrentUniformBuffer)
                {
                    // No change, skip
                }
                else
                {
                    if (NULL == pCurrentUniformBuffer)
                    {
                        // Nothing
                    }
                    else
                    {
                        // Current uniform buffer deref descriptor
                        tknRemoveFromHashSet(&pCurrentUniformBuffer->bindingPtrHashSet, pCurrentBinding);
                    }

                    pCurrentBinding->bindingUnion.uniformBufferBinding.pUniformBuffer = pNewUniformBuffer;
                    if (NULL == pNewUniformBuffer)
                    {
                        vkDescriptorBufferInfos[vkWriteDescriptorSetCount] = (VkDescriptorBufferInfo){
                            .buffer = VK_NULL_HANDLE,
                            .offset = 0,
                            .range = VK_WHOLE_SIZE,
                        };
                    }
                    else
                    {
                        // New uniform buffer ref descriptor
                        tknAddToHashSet(&pNewUniformBuffer->bindingPtrHashSet, pCurrentBinding);
                        vkDescriptorBufferInfos[vkWriteDescriptorSetCount] = (VkDescriptorBufferInfo){
                            .buffer = pNewUniformBuffer->vkBuffer,
                            .offset = 0,
                            .range = pNewUniformBuffer->size,
                        };
                    }
                    VkWriteDescriptorSet vkWriteDescriptorSet = {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = pMaterial->vkDescriptorSet,
                        .dstBinding = binding,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = vkDescriptorType,
                        .pImageInfo = NULL,
                        .pBufferInfo = &vkDescriptorBufferInfos[vkWriteDescriptorSetCount],
                        .pTexelBufferView = NULL,
                    };
                    vkWriteDescriptorSets[vkWriteDescriptorSetCount] = vkWriteDescriptorSet;
                    vkWriteDescriptorSetCount++;
                }
            }
            else if (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER == vkDescriptorType)
            {
                tknError("Storage buffer not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC == vkDescriptorType)
            {
                tknError("Uniform buffer dynamic not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC == vkDescriptorType)
            {
                tknError("Storage buffer dynamic not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorType)
            {
                Attachment *pOldAttachment = pOldBinding->bindingUnion.inputAttachmentBinding.pAttachment;
                Attachment *pNewAttachment = newBinding.bindingUnion.inputAttachmentBinding.pAttachment;
                if (pOldAttachment == pNewAttachment)
                {
                    // Skip
                }
                else
                {
                    if (pOldAttachment == NULL)
                    {
                        // Skip
                    }
                    else
                    {
                        if (ATTACHMENT_TYPE_DYNAMIC == pOldAttachment->attachmentType)
                        {
                            tknRemoveFromHashSet(&pOldAttachment->attachmentUnion.dynamicAttachment.bindingPtrHashSet, pOldBinding);
                        }
                        else if (ATTACHMENT_TYPE_FIXED == pOldAttachment->attachmentType)
                        {
                            tknRemoveFromHashSet(&pOldAttachment->attachmentUnion.fixedAttachment.bindingPtrHashSet, pOldBinding);
                        }
                        else
                        {
                            tknError("Swapchain attachment cannot be used as input attachment (attachment type: %d)", pOldAttachment->attachmentType);
                        }
                    }

                    if (pNewAttachment == NULL)
                    {
                        VkImageView vkImageView = VK_NULL_HANDLE;
                        vkDescriptorImageInfos[bindingIndex] = (VkDescriptorImageInfo){
                            .sampler = VK_NULL_HANDLE,
                            .imageView = vkImageView,
                            .imageLayout = pOldBinding->bindingUnion.inputAttachmentBinding.vkImageLayout,
                        };
                        vkWriteDescriptorSets[bindingIndex] = (VkWriteDescriptorSet){
                            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                            .dstSet = pMaterial->vkDescriptorSet,
                            .dstBinding = binding,
                            .dstArrayElement = 0,
                            .descriptorCount = 1,
                            .descriptorType = vkDescriptorType,
                            .pImageInfo = &vkDescriptorImageInfos[bindingIndex],
                            .pBufferInfo = VK_NULL_HANDLE,
                            .pTexelBufferView = VK_NULL_HANDLE,
                        };
                    }
                    else
                    {
                        VkImageView vkImageView = VK_NULL_HANDLE;
                        if (ATTACHMENT_TYPE_DYNAMIC == pNewAttachment->attachmentType)
                        {
                            vkImageView = pNewAttachment->attachmentUnion.dynamicAttachment.vkImageView;
                            tknAddToHashSet(&pNewAttachment->attachmentUnion.dynamicAttachment.bindingPtrHashSet, pOldBinding);
                        }
                        else if (ATTACHMENT_TYPE_FIXED == pNewAttachment->attachmentType)
                        {
                            vkImageView = pNewAttachment->attachmentUnion.fixedAttachment.vkImageView;
                            tknAddToHashSet(&pNewAttachment->attachmentUnion.fixedAttachment.bindingPtrHashSet, pOldBinding);
                        }
                        else
                        {
                            tknError("Swapchain attachment cannot be used as input attachment (attachment type: %d)", pNewAttachment->attachmentType);
                        }

                        vkDescriptorImageInfos[bindingIndex] = (VkDescriptorImageInfo){
                            .sampler = VK_NULL_HANDLE,
                            .imageView = vkImageView,
                            .imageLayout = pOldBinding->bindingUnion.inputAttachmentBinding.vkImageLayout,
                        };
                        vkWriteDescriptorSets[bindingIndex] = (VkWriteDescriptorSet){
                            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                            .dstSet = pMaterial->vkDescriptorSet,
                            .dstBinding = binding,
                            .dstArrayElement = 0,
                            .descriptorCount = 1,
                            .descriptorType = vkDescriptorType,
                            .pImageInfo = &vkDescriptorImageInfos[bindingIndex],
                            .pBufferInfo = VK_NULL_HANDLE,
                            .pTexelBufferView = VK_NULL_HANDLE,
                        };
                    }
                    pOldBinding->bindingUnion.inputAttachmentBinding.pAttachment = pNewAttachment;
                }
            }
            else
            {
                tknError("Unsupported descriptor type: %d", vkDescriptorType);
            }
        }
        if (vkWriteDescriptorSetCount > 0)
        {
            VkDevice vkDevice = pGfxContext->vkDevice;
            vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetCount, vkWriteDescriptorSets, 0, NULL);
        }
        tknFree(vkDescriptorBufferInfos);
        tknFree(vkDescriptorImageInfos);
        tknFree(vkWriteDescriptorSets);
    }
    else
    {
        printf("Warning: No bindings to update\n");
        return;
    }
}

Material *getGlobalMaterialPtr(GfxContext *pGfxContext)
{
    tknAssert(pGfxContext->pGlobalDescriptorSet != NULL, "Global descriptor set is NULL");
    tknAssert(pGfxContext->pGlobalDescriptorSet->materialPtrDynamicArray.count == 1, "Material pointer dynamic array count is not 1");
    Material *pMaterial = *(Material **)tknGetFromDynamicArray(&pGfxContext->pGlobalDescriptorSet->materialPtrDynamicArray, 0);
    return pMaterial;
}
Material *getSubpassMaterialPtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex)
{
    tknAssert(pRenderPass != NULL, "Render pass is NULL");
    tknAssert(subpassIndex < pRenderPass->subpassCount, "Subpass index is out of bounds");
    tknAssert(pRenderPass->subpasses[subpassIndex].pSubpassDescriptorSet != NULL, "Subpass descriptor set is NULL");
    tknAssert(pRenderPass->subpasses[subpassIndex].pSubpassDescriptorSet->materialPtrDynamicArray.count == 1, "Material pointer dynamic array count is not 1");
    Material *pMaterial = *(Material **)tknGetFromDynamicArray(&pRenderPass->subpasses[subpassIndex].pSubpassDescriptorSet->materialPtrDynamicArray, 0);
    return pMaterial;
}
Material *createPipelineMaterialPtr(GfxContext *pGfxContext, Pipeline *pPipeline)
{
    Material *pMaterial = createMaterialPtr(pGfxContext, pPipeline->pPipelineDescriptorSet);
    return pMaterial;
}
void destroyPipelineMaterialPtr(GfxContext *pGfxContext, Material *pMaterial)
{
    destroyMaterialPtr(pGfxContext, pMaterial);
}
