#include "gfxCommon.h"

void assertVkResult(VkResult vkResult)
{
    tknAssert(vkResult == VK_SUCCESS, "Vulkan error: %d", vkResult);
}

VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (uint32_t i = 0; i < candidateCount; i++)
    {
        VkFormat format = candidates[i];
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(pGfxContext->vkPhysicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR)
        {
            if ((props.linearTilingFeatures & features) == features)
            {
                return format;
            }
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL)
        {
            if ((props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
    }
    fprintf(stderr, "Error: No supported format found for the given requirements\n");
    return VK_FORMAT_MAX_ENUM;
}

DescriptorContent getNullDescriptorContent(VkDescriptorType vkDescriptorType)
{
    DescriptorContent descriptorContent = {0};
    if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
    {
        descriptorContent.samplerDescriptorContent.pSampler = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorType)
    {
        descriptorContent.inputAttachmentDescriptorContent.pAttachment = NULL;
    }
    else
    {
        tknError("Unsupported descriptor type: %d", vkDescriptorType);
    }
    return descriptorContent;
}

DescriptorSet *createDescriptorSetPtr(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set)
{
    DescriptorSet *pSubpassDescriptorSet = tknMalloc(sizeof(DescriptorSet));
    uint32_t descriptorCount = 0;
    Descriptor *descriptors = NULL;
    VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
    TknDynamicArray vkDescriptorPoolSizeDynamicArray = tknCreateDynamicArray(sizeof(VkDescriptorPoolSize), 1);
    for (uint32_t moduleIndex = 0; moduleIndex < spvReflectShaderModuleCount; moduleIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[moduleIndex];
        for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
            if (set == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    if (pSpvReflectDescriptorBinding->binding < descriptorCount)
                    {
                        // Skip, already counted
                    }
                    else
                    {
                        descriptorCount = pSpvReflectDescriptorBinding->binding + 1;
                    }
                }
                // Skip other sets.
                break;
            }
            else
            {
                // Skip
                printf("Warning: descriptor set %d\n", spvReflectDescriptorSet.set);
            }
        }
    }
    descriptors = tknMalloc(sizeof(Descriptor) * descriptorCount);
    VkDescriptorSetLayoutBinding *vkDescriptorSetLayoutBindings = tknMalloc(sizeof(VkDescriptorSetLayoutBinding) * descriptorCount);
    for (uint32_t binding = 0; binding < descriptorCount; binding++)
    {
        descriptors[binding] = (Descriptor){
            .vkDescriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM, // Set later
            .descriptorContent = {0},                        // Set later
            .pDescriptorSet = pSubpassDescriptorSet,
            .binding = binding,
        };
        vkDescriptorSetLayoutBindings[binding] = (VkDescriptorSetLayoutBinding){
            .binding = binding,
            .descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM,
            .descriptorCount = 0,
            .stageFlags = 0,
            .pImmutableSamplers = NULL,
        };
    }
    for (uint32_t moduleIndex = 0; moduleIndex < spvReflectShaderModuleCount; moduleIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[moduleIndex];
        for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
            if (set == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    uint32_t binding = pSpvReflectDescriptorBinding->binding;
                    if (vkDescriptorSetLayoutBindings[binding].descriptorType == VK_DESCRIPTOR_TYPE_MAX_ENUM)
                    {
                        VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = {
                            .binding = binding,
                            .descriptorType = (VkDescriptorType)pSpvReflectDescriptorBinding->descriptor_type,
                            .descriptorCount = pSpvReflectDescriptorBinding->count,
                            .stageFlags = (VkShaderStageFlags)spvReflectShaderModule.shader_stage,
                            .pImmutableSamplers = NULL,
                        };
                        vkDescriptorSetLayoutBindings[binding] = vkDescriptorSetLayoutBinding;
                        descriptors[binding] = (Descriptor){
                            .vkDescriptorType = vkDescriptorSetLayoutBinding.descriptorType,
                            .descriptorContent = getNullDescriptorContent(vkDescriptorSetLayoutBinding.descriptorType),
                            .pDescriptorSet = pSubpassDescriptorSet,
                            .binding = binding};

                        uint32_t poolSizeIndex;
                        for (poolSizeIndex = 0; poolSizeIndex < vkDescriptorPoolSizeDynamicArray.count; poolSizeIndex++)
                        {
                            VkDescriptorPoolSize *pVkDescriptorPoolSize = tknGetFromDynamicArray(&vkDescriptorPoolSizeDynamicArray, poolSizeIndex);
                            if (pVkDescriptorPoolSize->type == vkDescriptorSetLayoutBinding.descriptorType)
                            {
                                pVkDescriptorPoolSize->descriptorCount += vkDescriptorSetLayoutBinding.descriptorCount;
                                break;
                            }
                            else
                            {
                                // Skip
                            }
                        }
                        if (poolSizeIndex < vkDescriptorPoolSizeDynamicArray.count)
                        {
                            // Pool size already exists, skip adding
                        }
                        else
                        {
                            VkDescriptorPoolSize vkDescriptorPoolSize = {
                                .type = vkDescriptorSetLayoutBinding.descriptorType,
                                .descriptorCount = vkDescriptorSetLayoutBinding.descriptorCount,
                            };
                            tknAddToDynamicArray(&vkDescriptorPoolSizeDynamicArray, &vkDescriptorPoolSize, vkDescriptorPoolSizeDynamicArray.count);
                        }
                    }
                    else
                    {
                        tknAssert(vkDescriptorSetLayoutBindings[binding].descriptorType == (VkDescriptorType)pSpvReflectDescriptorBinding->descriptor_type, "Incompatible descriptor binding");
                        vkDescriptorSetLayoutBindings[binding].stageFlags |= (VkShaderStageFlags)spvReflectShaderModule.shader_stage;
                        vkDescriptorSetLayoutBindings[binding].descriptorCount = pSpvReflectDescriptorBinding->count > vkDescriptorSetLayoutBindings[binding].descriptorCount ? pSpvReflectDescriptorBinding->count : vkDescriptorSetLayoutBindings[binding].descriptorCount;
                    }
                }
                // Skip other sets.
                break;
            }
            else
            {
                // Skip
                printf("Warning: descriptor set %d\n", spvReflectDescriptorSet.set);
            }
        }
    }
    VkDevice vkDevice = pGfxContext->vkDevice;
    VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = descriptorCount,
        .pBindings = vkDescriptorSetLayoutBindings,
    };
    assertVkResult(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout));
    tknFree(vkDescriptorSetLayoutBindings);
    VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = vkDescriptorPoolSizeDynamicArray.count,
        .pPoolSizes = vkDescriptorPoolSizeDynamicArray.array,
        .maxSets = 1,
    };
    assertVkResult(vkCreateDescriptorPool(vkDevice, &vkDescriptorPoolCreateInfo, NULL, &vkDescriptorPool));
    tknDestroyDynamicArray(vkDescriptorPoolSizeDynamicArray);

    VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vkDescriptorSetLayout,
    };
    assertVkResult(vkAllocateDescriptorSets(vkDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet));

    *pSubpassDescriptorSet = (DescriptorSet){
        .descriptorCount = descriptorCount,
        .descriptors = descriptors,
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPool = vkDescriptorPool,
        .vkDescriptorSet = vkDescriptorSet,
    };
    return pSubpassDescriptorSet;
}
void updateDescriptors(GfxContext *pGfxContext, uint32_t newDescriptorCount, Descriptor *newDescriptors)
{
    if (newDescriptorCount > 0)
    {
        DescriptorSet *pCurrentDescriptorSet = newDescriptors[0].pDescriptorSet;
        tknAssert(pCurrentDescriptorSet != NULL, "Descriptor does not have a valid descriptor set");
        uint32_t vkWriteDescriptorSetCount = 0;
        VkWriteDescriptorSet *vkWriteDescriptorSets = tknMalloc(sizeof(VkWriteDescriptorSet) * newDescriptorCount);
        VkDescriptorImageInfo *vkDescriptorImageInfos = tknMalloc(sizeof(VkDescriptorImageInfo) * newDescriptorCount);
        for (uint32_t descriptorIndex = 0; descriptorIndex < newDescriptorCount; descriptorIndex++)
        {
            Descriptor newDescriptor = newDescriptors[descriptorIndex];
            tknAssert(newDescriptor.pDescriptorSet == pCurrentDescriptorSet, "All descriptors must belong to the same descriptor set");
            uint32_t binding = newDescriptor.binding;
            tknAssert(binding < pCurrentDescriptorSet->descriptorCount, "Invalid binding index");
            VkDescriptorType vkDescriptorType = pCurrentDescriptorSet->descriptors[binding].vkDescriptorType;
            tknAssert(vkDescriptorType == newDescriptor.vkDescriptorType, "Incompatible descriptor type");
            if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
            {
                Sampler *pNewSampler = newDescriptor.descriptorContent.samplerDescriptorContent.pSampler;
                Descriptor *pCurrentDescriptor = &pCurrentDescriptorSet->descriptors[binding];
                Sampler *pCurrentSampler = pCurrentDescriptor->descriptorContent.samplerDescriptorContent.pSampler;
                if (pNewSampler == pCurrentSampler)
                {
                    // No change, skip
                }
                else
                {
                    if (pCurrentSampler == NULL)
                    {
                        // Nothing
                    }
                    else
                    {
                        // Current sampler deref descriptor
                        tknRemoveFromHashSet(&pCurrentSampler->descriptorPtrHashSet, &pCurrentDescriptor);
                    }

                    // Descriptor ref new sampler
                    pCurrentDescriptor->descriptorContent.samplerDescriptorContent.pSampler = pNewSampler;
                    if (pNewSampler == NULL)
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
                        tknAddToHashSet(&pNewSampler->descriptorPtrHashSet, &pCurrentDescriptor);
                        vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                            .sampler = pNewSampler->vkSampler,
                            .imageView = VK_NULL_HANDLE,
                            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        };
                    }
                    VkWriteDescriptorSet vkWriteDescriptorSet = {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = pCurrentDescriptorSet->vkDescriptorSet,
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
            else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorType)
            {
                InputAttachmentDescriptorContent newDescriptorContent = newDescriptor.descriptorContent.inputAttachmentDescriptorContent;
                Descriptor *pCurrentDescriptor = &pCurrentDescriptorSet->descriptors[binding];
                Attachment *pCurrentAttachment = pCurrentDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment;
                Attachment *pNewAttachment = newDescriptorContent.pAttachment;
                if (pNewAttachment->attachmentType == ATTACHMENT_TYPE_SWAPCHAIN)
                {
                    // Swapchain attachments are handled at render time.
                }
                else
                {
                    if (pNewAttachment == pCurrentAttachment)
                    {
                        // No change, skip
                    }
                    else
                    {
                        // Remove reference from old attachment
                        if (pCurrentAttachment == NULL)
                        {
                            // Nothing
                        }
                        else
                        {
                            if (pCurrentAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                            {
                                tknRemoveFromHashSet(&pCurrentAttachment->attachmentContent.dynamicAttachmentContent.pImage->descriptorPtrHashSet, pCurrentDescriptor);
                            }
                            else if (pCurrentAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                            {
                                tknRemoveFromHashSet(&pCurrentAttachment->attachmentContent.fixedAttachmentContent.pImage->descriptorPtrHashSet, pCurrentDescriptor);
                            }
                            else
                            {
                                tknError("Unsupported attachment type: %d", pNewAttachment->attachmentType);
                            }
                        }
                        // Add reference to new attachment
                        if (pNewAttachment == NULL)
                        {
                            // Nothing
                        }
                        else
                        {
                            if (pNewAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                            {
                                tknAddToHashSet(&pNewAttachment->attachmentContent.dynamicAttachmentContent.pImage->descriptorPtrHashSet, pCurrentDescriptor);
                            }
                            else if (pNewAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                            {
                                tknAddToHashSet(&pNewAttachment->attachmentContent.fixedAttachmentContent.pImage->descriptorPtrHashSet, pCurrentDescriptor);
                            }
                            else
                            {
                                tknError("Unsupported attachment type: %d", pNewAttachment->attachmentType);
                            }
                        }
                        pCurrentDescriptor->descriptorContent.inputAttachmentDescriptorContent = newDescriptorContent;
                        VkImageView imageView;
                        if (pNewAttachment != NULL)
                        {
                            if (pNewAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                            {
                                imageView = pNewAttachment->attachmentContent.dynamicAttachmentContent.pImage->vkImageView;
                            }
                            else if (pNewAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                            {
                                imageView = pNewAttachment->attachmentContent.fixedAttachmentContent.pImage->vkImageView;
                            }
                            else
                            {
                                tknError("Unsupported attachment type: %d", pNewAttachment->attachmentType);
                            }
                        }
                        else
                        {
                            imageView = VK_NULL_HANDLE;
                        }
                        vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                            .sampler = VK_NULL_HANDLE,
                            .imageView = imageView,
                            .imageLayout = pCurrentDescriptor->descriptorContent.inputAttachmentDescriptorContent.vkImageLayout,
                        };
                        VkWriteDescriptorSet vkWriteDescriptorSet = {
                            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                            .dstSet = pCurrentDescriptorSet->vkDescriptorSet,
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
            }
            // TODO other types
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
        tknFree(vkDescriptorImageInfos);
        tknFree(vkWriteDescriptorSets);
    }
    else
    {
        printf("Warning: No descriptors to update\n");
        return;
    }
}
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    Descriptor *descriptors = tknMalloc(sizeof(Descriptor) * pDescriptorSet->descriptorCount);
    for (uint32_t binding = 0; binding < pDescriptorSet->descriptorCount; binding++)
    {
        VkDescriptorType descriptorType = pDescriptorSet->descriptors[binding].vkDescriptorType;
        descriptors[binding] = (Descriptor){
            .vkDescriptorType = descriptorType,
            .descriptorContent = getNullDescriptorContent(descriptorType),
            .pDescriptorSet = pDescriptorSet,
            .binding = binding,
        };
    }
    updateDescriptors(pGfxContext, pDescriptorSet->descriptorCount, descriptors);
    tknFree(descriptors);

    vkFreeDescriptorSets(vkDevice, pDescriptorSet->vkDescriptorPool, 1, &pDescriptorSet->vkDescriptorSet);
    vkDestroyDescriptorPool(vkDevice, pDescriptorSet->vkDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pDescriptorSet->vkDescriptorSetLayout, NULL);
    tknFree(pDescriptorSet->descriptors);
    tknFree(pDescriptorSet);
}