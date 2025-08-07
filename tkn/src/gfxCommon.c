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
        descriptors[binding] = (Descriptor){0};
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
                        descriptors[binding].vkDescriptorType = vkDescriptorSetLayoutBinding.descriptorType;
                        descriptors[binding].pDescriptorSet = pSubpassDescriptorSet;
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
void updateDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t bindingCount, DescriptorBinding *bindings)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    uint32_t vkWriteDescriptorSetCount = 0;
    VkWriteDescriptorSet *vkWriteDescriptorSets = tknMalloc(sizeof(VkWriteDescriptorSet) * bindingCount);
    VkDescriptorImageInfo *vkDescriptorImageInfos = tknMalloc(sizeof(VkDescriptorImageInfo) * bindingCount);

    for (uint32_t bindingIndex = 0; bindingIndex < bindingCount; bindingIndex++)
    {
        DescriptorBinding descriptorBinding = bindings[bindingIndex];
        uint32_t binding = descriptorBinding.binding;
        tknAssert(binding < pDescriptorSet->descriptorCount, "Invalid binding index");
        VkDescriptorType vkDescriptorType = pDescriptorSet->descriptors[binding].vkDescriptorType;
        tknAssert(vkDescriptorType == descriptorBinding.vkDescriptorType, "Incompatible descriptor type");
        if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
        {
            SamplerDescriptorContent newDescriptorContent = descriptorBinding.descriptorContent.samplerDescriptorContent;
            Descriptor *pDescriptor = &pDescriptorSet->descriptors[binding];
            Sampler *pSampler = pDescriptor->descriptorContent.samplerDescriptorContent.pSampler;
            if (newDescriptorContent.pSampler == pSampler)
            {
                // No change, skip
            }
            else
            {
                if (pSampler == NULL)
                {
                    // Nothing
                }
                else
                {
                    // Remove reference
                    tknRemoveFromHashSet(&pSampler->descriptorPtrHashSet, &pDescriptor);
                }
                // Add reference
                tknAddToHashSet(&newDescriptorContent.pSampler->descriptorPtrHashSet, &pDescriptor);
                pDescriptor->descriptorContent.samplerDescriptorContent = newDescriptorContent;
                vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                    .sampler = newDescriptorContent.pSampler->vkSampler,
                    .imageView = VK_NULL_HANDLE,
                    .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                };
                VkWriteDescriptorSet vkWriteDescriptorSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = pDescriptorSet->vkDescriptorSet,
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
            InputAttachmentDescriptorContent newDescriptorContent = descriptorBinding.descriptorContent.inputAttachmentDescriptorContent;
            Descriptor *pDescriptor = &pDescriptorSet->descriptors[binding];
            Attachment *pAttachment = pDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment;
            if (newDescriptorContent.pAttachment == pAttachment &&
                newDescriptorContent.vkImageLayout == pDescriptor->descriptorContent.inputAttachmentDescriptorContent.vkImageLayout)
            {
                // No change, skip
            }
            else
            {
                if (pAttachment != NULL)
                {
                    // Remove reference from old attachment
                    if (pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                    {
                        tknRemoveFromHashSet(&pAttachment->attachmentContent.dynamicAttachmentContent.image.descriptorPtrHashSet, &pDescriptor);
                    }
                    else if (pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                    {
                        tknRemoveFromHashSet(&pAttachment->attachmentContent.fixedAttachmentContent.image.descriptorPtrHashSet, &pDescriptor);
                    }
                }
                if (newDescriptorContent.pAttachment != NULL)
                {
                    // Add reference to new attachment
                    if (newDescriptorContent.pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                    {
                        tknAddToHashSet(&newDescriptorContent.pAttachment->attachmentContent.dynamicAttachmentContent.image.descriptorPtrHashSet, &pDescriptor);
                    }
                    else if (newDescriptorContent.pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                    {
                        tknAddToHashSet(&newDescriptorContent.pAttachment->attachmentContent.fixedAttachmentContent.image.descriptorPtrHashSet, &pDescriptor);
                    }
                }
                pDescriptor->descriptorContent.inputAttachmentDescriptorContent = newDescriptorContent;
                VkImageView imageView = VK_NULL_HANDLE;
                if (newDescriptorContent.pAttachment != NULL)
                {
                    if (newDescriptorContent.pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                    {
                        imageView = newDescriptorContent.pAttachment->attachmentContent.dynamicAttachmentContent.image.vkImageView;
                    }
                    else if (newDescriptorContent.pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                    {
                        imageView = newDescriptorContent.pAttachment->attachmentContent.fixedAttachmentContent.image.vkImageView;
                    }
                    // SWAPCHAIN type needs special handling - might be handled at render time
                }
                vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                    .sampler = VK_NULL_HANDLE,
                    .imageView = imageView,
                    .imageLayout = newDescriptorContent.vkImageLayout,
                };
                VkWriteDescriptorSet vkWriteDescriptorSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = pDescriptorSet->vkDescriptorSet,
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
        // TODO other types
        else
        {
            tknError("Unsupported descriptor type: %d", vkDescriptorType);
        }
    }

    if (vkWriteDescriptorSetCount > 0)
    {
        vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetCount, vkWriteDescriptorSets, 0, NULL);
    }
    tknFree(vkDescriptorImageInfos);
    tknFree(vkWriteDescriptorSets);
}
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    DescriptorBinding *descriptorBindings = tknMalloc(sizeof(DescriptorBinding) * pDescriptorSet->descriptorCount);
    for (uint32_t binding = 0; binding < pDescriptorSet->descriptorCount; binding++)
    {
        VkDescriptorType descriptorType = pDescriptorSet->descriptors[binding].vkDescriptorType;
        descriptorBindings[binding].binding = binding;
        descriptorBindings[binding].vkDescriptorType = descriptorType;
        if (VK_DESCRIPTOR_TYPE_SAMPLER == descriptorType)
        {
            descriptorBindings[binding].descriptorContent.samplerDescriptorContent.pSampler = NULL;
        }
        else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == descriptorType)
        {
            descriptorBindings[binding].descriptorContent.inputAttachmentDescriptorContent.pAttachment = NULL;
        }
        else
        {
            tknError("Unsupported descriptor type: %d", descriptorType);
        }
    }
    updateDescriptorSetPtr(pGfxContext, pDescriptorSet, pDescriptorSet->descriptorCount, descriptorBindings);
    tknFree(descriptorBindings);
    vkFreeDescriptorSets(vkDevice, pDescriptorSet->vkDescriptorPool, 1, &pDescriptorSet->vkDescriptorSet);
    vkDestroyDescriptorPool(vkDevice, pDescriptorSet->vkDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pDescriptorSet->vkDescriptorSetLayout, NULL);
    tknFree(pDescriptorSet->descriptors);

    tknFree(pDescriptorSet);
}