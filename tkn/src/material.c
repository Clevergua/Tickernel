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
            .bindingUnion = {0},
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
    TknHashSet drawCallPtrHashSet = tknCreateHashSet(sizeof(DrawCall *));
    *pMaterial = (Material){
        .vkDescriptorSet = vkDescriptorSet,
        .bindingCount = descriptorCount,
        .bindings = bindings,
        .vkDescriptorPool = vkDescriptorPool,
        .pDescriptorSet = pDescriptorSet,
        .drawCallPtrHashSet = drawCallPtrHashSet,
    };
    tknAddToHashSet(&pDescriptorSet->materialPtrHashSet, &pMaterial);
    return pMaterial;
}
void destroyMaterialPtr(GfxContext *pGfxContext, Material *pMaterial)
{
    tknAssert(0 == pMaterial->drawCallPtrHashSet.count, "Material still has draw calls attached!");
    VkDevice vkDevice = pGfxContext->vkDevice;
    uint32_t inputBindingCount = 0;
    InputBinding *inputBindings = tknMalloc(sizeof(InputBinding) * pMaterial->bindingCount);
    for (uint32_t binding = 0; binding < pMaterial->bindingCount; binding++)
    {
        Binding *pBinding = &pMaterial->bindings[binding];
        VkDescriptorType vkDescriptorType = pBinding->vkDescriptorType;
        if (vkDescriptorType != VK_DESCRIPTOR_TYPE_MAX_ENUM && vkDescriptorType != VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
        {
            if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
            {
                Sampler *pSampler = pBinding->bindingUnion.samplerBinding.pSampler;
                if (NULL == pSampler)
                {
                    // Nothing
                }
                else
                {
                    // Current sampler deref descriptor
                    tknRemoveFromHashSet(&pSampler->bindingPtrHashSet, &pBinding);
                }
            }
            else if (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == vkDescriptorType)
            {
                Sampler *pSampler = pBinding->bindingUnion.combinedImageSamplerBinding.pSampler;
                Image *pImage = pBinding->bindingUnion.combinedImageSamplerBinding.pImage;
                if (NULL == pSampler && NULL == pImage)
                {
                    // Nothing
                }
                else
                {
                    // Current sampler deref descriptor
                    tknRemoveFromHashSet(&pSampler->bindingPtrHashSet, &pBinding);
                    tknRemoveFromHashSet(&pImage->bindingPtrHashSet, &pBinding);
                }
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
                UniformBuffer *pUniformBuffer = pBinding->bindingUnion.uniformBufferBinding.pUniformBuffer;
                if (NULL == pUniformBuffer)
                {
                    // Nothing
                }
                else
                {
                    // Current uniform buffer deref descriptor
                    tknRemoveFromHashSet(&pUniformBuffer->bindingPtrHashSet, &pBinding);
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
            else
            {
                tknError("Unsupported descriptor type: %d", vkDescriptorType);
            }
        }
        else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorType)
        {
            tknAssert(pBinding->bindingUnion.inputAttachmentBinding.pAttachment == NULL, "Input attachment bindings must be unbound before destroying a material");
        }
        else
        {
            // Skip
        }
    }
    if (inputBindingCount > 0)
    {
        updateMaterialPtr(pGfxContext, pMaterial, inputBindingCount, inputBindings);
    }
    tknFree(inputBindings);

    tknRemoveFromHashSet(&pMaterial->pDescriptorSet->materialPtrHashSet, &pMaterial);
    tknDestroyHashSet(pMaterial->drawCallPtrHashSet);
    // Destroying the descriptor pool automatically frees all descriptor sets allocated from it
    vkDestroyDescriptorPool(vkDevice, pMaterial->vkDescriptorPool, NULL);
    tknFree(pMaterial->bindings);
    tknFree(pMaterial);
}

void bindAttachmentsToMaterialPtr(GfxContext *pGfxContext, Material *pMaterial)
{
    uint32_t vkWriteDescriptorSetCount = 0;
    for (uint32_t binding = 0; binding < pMaterial->bindingCount; binding++)
    {
        Binding *pBinding = &pMaterial->bindings[binding];
        if (pBinding->vkDescriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
        {
            vkWriteDescriptorSetCount++;
        }
        else
        {
            // Skip
        }
    }
    if (vkWriteDescriptorSetCount > 0)
    {
        uint32_t vkWriteDescriptorSetIndex = 0;
        VkWriteDescriptorSet *vkWriteDescriptorSets = tknMalloc(sizeof(VkWriteDescriptorSet) * vkWriteDescriptorSetCount);
        VkDescriptorImageInfo *vkDescriptorImageInfos = tknMalloc(sizeof(VkDescriptorImageInfo) * vkWriteDescriptorSetCount);
        VkDescriptorType vkDescriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        for (uint32_t binding = 0; binding < pMaterial->bindingCount; binding++)
        {
            Binding *pBinding = &pMaterial->bindings[binding];
            if (pBinding->vkDescriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
            {
                Attachment *pInputAttachment = pBinding->bindingUnion.inputAttachmentBinding.pAttachment;
                tknAssert(pInputAttachment != NULL, "Binding %d is not bound to an attachment", binding);
                VkImageView vkImageView = VK_NULL_HANDLE;
                if (ATTACHMENT_TYPE_DYNAMIC == pInputAttachment->attachmentType)
                {
                    vkImageView = pInputAttachment->attachmentUnion.dynamicAttachment.vkImageView;
                    tknAddToHashSet(&pInputAttachment->attachmentUnion.dynamicAttachment.bindingPtrHashSet, &pBinding);
                }
                else if (ATTACHMENT_TYPE_FIXED == pInputAttachment->attachmentType)
                {
                    vkImageView = pInputAttachment->attachmentUnion.fixedAttachment.vkImageView;
                    tknAddToHashSet(&pInputAttachment->attachmentUnion.fixedAttachment.bindingPtrHashSet, &pBinding);
                }
                else
                {
                    tknError("Swapchain attachment cannot be used as input attachment (attachment type: %d)", pInputAttachment->attachmentType);
                }

                vkDescriptorImageInfos[vkWriteDescriptorSetIndex] = (VkDescriptorImageInfo){
                    .sampler = VK_NULL_HANDLE,
                    .imageView = vkImageView,
                    .imageLayout = pBinding->bindingUnion.inputAttachmentBinding.vkImageLayout,
                };

                vkWriteDescriptorSets[vkWriteDescriptorSetIndex] = (VkWriteDescriptorSet){
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = pMaterial->vkDescriptorSet,
                    .dstBinding = binding,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vkDescriptorType,
                    .pImageInfo = &vkDescriptorImageInfos[vkWriteDescriptorSetIndex],
                    .pBufferInfo = VK_NULL_HANDLE,
                    .pTexelBufferView = VK_NULL_HANDLE,
                };
                vkWriteDescriptorSetIndex++;
            }
            else
            {
                // Skip
            }
        }
        VkDevice vkDevice = pGfxContext->vkDevice;
        vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetCount, vkWriteDescriptorSets, 0, NULL);
        tknFree(vkDescriptorImageInfos);
        tknFree(vkWriteDescriptorSets);
    }
    else
    {
        return;
    }
}
void unbindAttachmentsFromMaterialPtr(GfxContext *pGfxContext, Material *pMaterial)
{
    uint32_t vkWriteDescriptorSetCount = 0;
    for (uint32_t binding = 0; binding < pMaterial->bindingCount; binding++)
    {
        Binding *pBinding = &pMaterial->bindings[binding];
        if (pBinding->vkDescriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
        {
            vkWriteDescriptorSetCount++;
        }
        else
        {
            // Skip
        }
    }
    if (vkWriteDescriptorSetCount > 0)
    {
        uint32_t vkWriteDescriptorSetIndex = 0;
        VkWriteDescriptorSet *vkWriteDescriptorSets = tknMalloc(sizeof(VkWriteDescriptorSet) * vkWriteDescriptorSetCount);
        VkDescriptorImageInfo *vkDescriptorImageInfos = tknMalloc(sizeof(VkDescriptorImageInfo) * vkWriteDescriptorSetCount);
        for (uint32_t binding = 0; binding < pMaterial->bindingCount; binding++)
        {
            Binding *pBinding = &pMaterial->bindings[binding];
            if (pBinding->vkDescriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
            {
                Attachment *pAttachment = pBinding->bindingUnion.inputAttachmentBinding.pAttachment;
                tknAssert(pAttachment != NULL, "Binding %d is not bound to an attachment", binding);
                pBinding->bindingUnion.inputAttachmentBinding.pAttachment = NULL;

                if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
                {
                    tknRemoveFromHashSet(&pAttachment->attachmentUnion.dynamicAttachment.bindingPtrHashSet, &pBinding);
                }
                else if (ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType)
                {
                    tknRemoveFromHashSet(&pAttachment->attachmentUnion.fixedAttachment.bindingPtrHashSet, &pBinding);
                }
                else
                {
                    tknError("Swapchain attachment cannot be used as input attachment (attachment type: %d)", pAttachment->attachmentType);
                }
                VkImageView vkImageView = pGfxContext->pEmptyImage->vkImageView;
                vkDescriptorImageInfos[vkWriteDescriptorSetIndex] = (VkDescriptorImageInfo){
                    .sampler = VK_NULL_HANDLE,
                    .imageView = vkImageView,
                    .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
                };
                vkWriteDescriptorSets[vkWriteDescriptorSetIndex] = (VkWriteDescriptorSet){
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = pMaterial->vkDescriptorSet,
                    .dstBinding = binding,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .pImageInfo = &vkDescriptorImageInfos[vkWriteDescriptorSetIndex],
                    .pBufferInfo = VK_NULL_HANDLE,
                    .pTexelBufferView = VK_NULL_HANDLE,
                };
                vkWriteDescriptorSetIndex++;
            }
            else
            {
                // Skip
            }
        }
        VkDevice vkDevice = pGfxContext->vkDevice;
        vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetCount, vkWriteDescriptorSets, 0, NULL);
        tknFree(vkDescriptorImageInfos);
        tknFree(vkWriteDescriptorSets);
    }
    else
    {
        return;
    }
}
void updateAttachmentOfMaterialPtr(GfxContext *pGfxContext, Binding *pBinding)
{
    tknAssert(pBinding->vkDescriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, "Binding is not an input attachment");
    tknAssert(pBinding->bindingUnion.inputAttachmentBinding.pAttachment != NULL, "Binding is not bound to an attachment");

    Attachment *pAttachment = pBinding->bindingUnion.inputAttachmentBinding.pAttachment;
    VkImageView vkImageView = VK_NULL_HANDLE;

    if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
    {
        vkImageView = pAttachment->attachmentUnion.dynamicAttachment.vkImageView;
    }
    else if (ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType)
    {
        vkImageView = pAttachment->attachmentUnion.fixedAttachment.vkImageView;
    }
    else
    {
        tknError("Swapchain attachment cannot be used as input attachment (attachment type: %d)", pAttachment->attachmentType);
    }

    VkDescriptorImageInfo vkDescriptorImageInfo = {
        .sampler = VK_NULL_HANDLE,
        .imageView = vkImageView,
        .imageLayout = pBinding->bindingUnion.inputAttachmentBinding.vkImageLayout,
    };
    VkWriteDescriptorSet vkWriteDescriptorSet = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = pBinding->pMaterial->vkDescriptorSet,
        .dstBinding = pBinding->binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .pImageInfo = &vkDescriptorImageInfo,
        .pBufferInfo = VK_NULL_HANDLE,
        .pTexelBufferView = VK_NULL_HANDLE,
    };
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkUpdateDescriptorSets(vkDevice, 1, &vkWriteDescriptorSet, 0, NULL);
}

Material *getGlobalMaterialPtr(GfxContext *pGfxContext)
{
    tknAssert(pGfxContext->pGlobalDescriptorSet != NULL, "Global descriptor set is NULL");
    TknHashSet materialPtrHashSet = pGfxContext->pGlobalDescriptorSet->materialPtrHashSet;
    tknAssert(materialPtrHashSet.count == 1, "Material pointer hashset count is not 1");
    for (uint32_t nodeIndex = 0; nodeIndex < materialPtrHashSet.capacity; nodeIndex++)
    {
        TknListNode *node = materialPtrHashSet.nodePtrs[nodeIndex];
        if (node)
        {
            Material *pMaterial = *(Material **)node->data;
            return pMaterial;
        }
        else
        {
            // Continue searching
        }
    }
    tknError("Failed to find global material");
    return NULL;
}
Material *getSubpassMaterialPtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex)
{
    tknAssert(pRenderPass != NULL, "Render pass is NULL");
    tknAssert(subpassIndex < pRenderPass->subpassCount, "Subpass index is out of bounds");
    tknAssert(pRenderPass->subpasses[subpassIndex].pSubpassDescriptorSet != NULL, "Subpass descriptor set is NULL");
    tknAssert(pRenderPass->subpasses[subpassIndex].pSubpassDescriptorSet->materialPtrHashSet.count == 1, "Material pointer hashset count is not 1");
    TknHashSet materialPtrHashSet = pRenderPass->subpasses[subpassIndex].pSubpassDescriptorSet->materialPtrHashSet;
    for (uint32_t nodeIndex = 0; nodeIndex < materialPtrHashSet.capacity; nodeIndex++)
    {
        TknListNode *node = materialPtrHashSet.nodePtrs[nodeIndex];
        if (node)
        {
            Material *pMaterial = *(Material **)node->data;
            return pMaterial;
        }
        else
        {
            // Continue searching
        }
    }
    tknError("Failed to find subpass material");
    return NULL;
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

void updateMaterialPtr(GfxContext *pGfxContext, Material *pMaterial, uint32_t inputBindingCount, InputBinding *inputBindings)
{
    if (inputBindingCount > 0)
    {
        tknAssert(NULL != pMaterial, "Material must not be NULL");
        uint32_t vkWriteDescriptorSetCount = 0;
        VkWriteDescriptorSet *vkWriteDescriptorSets = tknMalloc(sizeof(VkWriteDescriptorSet) * inputBindingCount);
        VkDescriptorImageInfo *vkDescriptorImageInfos = tknMalloc(sizeof(VkDescriptorImageInfo) * inputBindingCount);
        VkDescriptorBufferInfo *vkDescriptorBufferInfos = tknMalloc(sizeof(VkDescriptorBufferInfo) * inputBindingCount);
        for (uint32_t bindingIndex = 0; bindingIndex < inputBindingCount; bindingIndex++)
        {
            InputBinding inputBinding = inputBindings[bindingIndex];
            uint32_t binding = inputBinding.binding;
            tknAssert(binding < pMaterial->bindingCount, "Invalid binding index");
            VkDescriptorType vkDescriptorType = pMaterial->bindings[binding].vkDescriptorType;
            tknAssert(vkDescriptorType == inputBinding.vkDescriptorType, "Incompatible descriptor type");
            Binding *pBinding = &pMaterial->bindings[binding];
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
                Sampler *pInputSampler = inputBinding.inputBindingUnion.samplerBinding.pSampler;
                Sampler *pSampler = pBinding->bindingUnion.samplerBinding.pSampler;
                if (pInputSampler == pSampler)
                {
                    // No change, skip
                }
                else
                {
                    if (NULL == pSampler)
                    {
                        // Nothing
                    }
                    else
                    {
                        // Current sampler deref descriptor
                        tknRemoveFromHashSet(&pSampler->bindingPtrHashSet, &pBinding);
                    }

                    pBinding->bindingUnion.samplerBinding.pSampler = pInputSampler;
                    if (NULL == pInputSampler)
                    {
                        tknError("Cannot bind NULL sampler");
                    }
                    else
                    {
                        // New sampler ref descriptor
                        tknAddToHashSet(&pInputSampler->bindingPtrHashSet, &pBinding);
                        vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                            .sampler = pInputSampler->vkSampler,
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
                Sampler *pInputSampler = inputBinding.inputBindingUnion.combinedImageSamplerBinding.pSampler;
                Image *pInputImage = inputBinding.inputBindingUnion.combinedImageSamplerBinding.pImage;
                Sampler *pSampler = pBinding->bindingUnion.combinedImageSamplerBinding.pSampler;
                Image *pImage = pBinding->bindingUnion.combinedImageSamplerBinding.pImage;
                
                if (pInputSampler == pSampler && pInputImage == pImage)
                {
                    // No change, skip
                }
                else
                {
                    // Remove old references
                    if (NULL != pSampler)
                    {
                        tknRemoveFromHashSet(&pSampler->bindingPtrHashSet, &pBinding);
                    }
                    if (NULL != pImage)
                    {
                        tknRemoveFromHashSet(&pImage->bindingPtrHashSet, &pBinding);
                    }

                    // Update bindings
                    pBinding->bindingUnion.combinedImageSamplerBinding.pSampler = pInputSampler;
                    pBinding->bindingUnion.combinedImageSamplerBinding.pImage = pInputImage;
                    
                    if (NULL == pInputSampler || NULL == pInputImage)
                    {
                        tknError("Cannot bind NULL sampler or image in combined image sampler");
                    }
                    else
                    {
                        // Add new references
                        tknAddToHashSet(&pInputSampler->bindingPtrHashSet, &pBinding);
                        tknAddToHashSet(&pInputImage->bindingPtrHashSet, &pBinding);
                        
                        vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                            .sampler = pInputSampler->vkSampler,
                            .imageView = pInputImage->vkImageView,
                            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
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
                UniformBuffer *pInputUniformBuffer = inputBinding.inputBindingUnion.uniformBufferBinding.pUniformBuffer;
                UniformBuffer *pUniformBuffer = pBinding->bindingUnion.uniformBufferBinding.pUniformBuffer;
                if (pInputUniformBuffer == pUniformBuffer)
                {
                    // No change, skip
                }
                else
                {
                    if (NULL == pUniformBuffer)
                    {
                        // Nothing
                    }
                    else
                    {
                        // Current uniform buffer deref descriptor
                        tknRemoveFromHashSet(&pUniformBuffer->bindingPtrHashSet, &pBinding);
                    }
                    pBinding->bindingUnion.uniformBufferBinding.pUniformBuffer = pInputUniformBuffer;
                    if (NULL == pInputUniformBuffer)
                    {
                        tknError("Cannot bind NULL uniform buffer");
                    }
                    else
                    {
                        // New uniform buffer ref descriptor
                        tknAddToHashSet(&pInputUniformBuffer->bindingPtrHashSet, &pBinding);
                        vkDescriptorBufferInfos[vkWriteDescriptorSetCount] = (VkDescriptorBufferInfo){
                            .buffer = pInputUniformBuffer->vkBuffer,
                            .offset = 0,
                            .range = pInputUniformBuffer->size,
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
        tknWarning("No bindings to update");
        return;
    }
}

InputBindingUnion getEmptyInputBindingUnion(GfxContext *pGfxContext, VkDescriptorType vkDescriptorType)
{
    InputBindingUnion emptyUnion = {0};
    // Create appropriate empty binding union based on descriptor type
    switch (vkDescriptorType)
    {
    case VK_DESCRIPTOR_TYPE_SAMPLER:
        emptyUnion.samplerBinding.pSampler = pGfxContext->pEmptySampler;
        break;
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        emptyUnion.combinedImageSamplerBinding.pSampler = pGfxContext->pEmptySampler;
        emptyUnion.combinedImageSamplerBinding.pImage = pGfxContext->pEmptyImage;
        break;
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        emptyUnion.uniformBufferBinding.pUniformBuffer = pGfxContext->pEmptyUniformBuffer;
        break;

    default:
        // For unsupported types, default to uniform buffer as a safe fallback
        emptyUnion.uniformBufferBinding.pUniformBuffer = pGfxContext->pEmptyUniformBuffer;
        tknWarning("Unsupported descriptor type %d in getEmptyInputBindingUnion, using uniform buffer fallback", vkDescriptorType);
        break;
    }

    return emptyUnion;
}