#include "gfxPipeline.h"

static SpvReflectShaderModule createSpvReflectShaderModule(const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        tknError("Failed to open file: %s\n", filePath);
    }
    fseek(file, 0, SEEK_END);
    size_t shaderSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (shaderSize % 4 != 0)
    {
        fclose(file);
        tknError("Invalid SPIR-V file size: %s\n", filePath);
    }
    void *shaderCode = tknMalloc(shaderSize);
    size_t bytesRead = fread(shaderCode, 1, shaderSize, file);

    fclose(file);

    if (bytesRead != shaderSize)
    {
        tknError("Failed to read entire file: %s\n", filePath);
    }
    SpvReflectShaderModule spvReflectShaderModule;
    SpvReflectResult spvReflectResult = spvReflectCreateShaderModule(shaderSize, shaderCode, &spvReflectShaderModule);
    tknAssert(spvReflectResult == SPV_REFLECT_RESULT_SUCCESS, "Failed to reflect shader module: %s", filePath);
    tknFree(shaderCode);

    return spvReflectShaderModule;
}
static void destroySpvReflectShaderModule(SpvReflectShaderModule *pSpvReflectShaderModule)
{
    spvReflectDestroyShaderModule(pSpvReflectShaderModule);
}

static Subpass createSubpass(GfxContext *pGfxContext, uint32_t subpassIndex, uint32_t attachmentCount, Attachment **attachmentPtrs, uint32_t inputVkAttachmentReferenceCount, const VkAttachmentReference *inputVkAttachmentReferences, uint32_t spvPathCount, const char **spvPaths)
{
    TknDynamicArray pipelinePtrDynamicArray = tknCreateDynamicArray(sizeof(Pipeline *), 1);
    uint32_t inputAttachmentDescriptorCount = 0;
    Descriptor *inputAttachmentDescriptors = tknMalloc(sizeof(Descriptor) * inputVkAttachmentReferenceCount);

    VkImageLayout *inputAttachmentIndexToVkImageLayout = tknMalloc(sizeof(VkImageLayout) * attachmentCount);
    for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
    {
        inputAttachmentIndexToVkImageLayout[attachmentIndex] = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    for (uint32_t inputVkAttachmentReferenceIndex = 0; inputVkAttachmentReferenceIndex < inputVkAttachmentReferenceCount; inputVkAttachmentReferenceIndex++)
    {
        tknAssert(inputVkAttachmentReferences[inputVkAttachmentReferenceIndex].attachment < attachmentCount, "Input attachment reference index %u out of bounds", inputVkAttachmentReferenceIndex);
        inputAttachmentIndexToVkImageLayout[inputVkAttachmentReferences[inputVkAttachmentReferenceIndex].attachment] = inputVkAttachmentReferences[inputVkAttachmentReferenceIndex].layout;
    }
    SpvReflectShaderModule *spvReflectShaderModules = tknMalloc(sizeof(SpvReflectShaderModule) * spvPathCount);
    for (uint32_t spvPathIndex = 0; spvPathIndex < spvPathCount; spvPathIndex++)
    {
        const char *spvPath = spvPaths[spvPathIndex];
        spvReflectShaderModules[spvPathIndex] = createSpvReflectShaderModule(spvPath);
    }
    DescriptorSet *pSubpassDescriptorSet = createDescriptorSetPtr(pGfxContext, spvPathCount, spvReflectShaderModules, TICKERNEL_SUBPASS_DESCRIPTOR_SET);
    for (uint32_t spvPathIndex = 0; spvPathIndex < spvPathCount; spvPathIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[spvPathIndex];
        for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
            if (TICKERNEL_SUBPASS_DESCRIPTOR_SET == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    if (pSpvReflectDescriptorBinding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
                    {
                        uint32_t binding = pSpvReflectDescriptorBinding->binding;
                        uint32_t inputAttachmentIndex = pSpvReflectDescriptorBinding->input_attachment_index;
                        Descriptor *pDescriptor = &pSubpassDescriptorSet->descriptors[binding];
                        if (pDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment == NULL)
                        {
                            tknAssert(inputAttachmentIndex < attachmentCount, "Input attachment index %u out of bounds", inputAttachmentIndex);
                            pDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment = attachmentPtrs[inputAttachmentIndex];
                            pDescriptor->descriptorContent.inputAttachmentDescriptorContent.vkImageLayout = inputAttachmentIndexToVkImageLayout[inputAttachmentIndex];
                            if (attachmentPtrs[inputAttachmentIndex]->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                            {
                                tknAddToHashSet(&attachmentPtrs[inputAttachmentIndex]->attachmentContent.dynamicAttachmentContent.descriptorPtrHashSet, &pDescriptor);
                            }
                            else if (attachmentPtrs[inputAttachmentIndex]->attachmentType == ATTACHMENT_TYPE_FIXED)
                            {
                                tknAddToHashSet(&attachmentPtrs[inputAttachmentIndex]->attachmentContent.fixedAttachmentContent.descriptorPtrHashSet, &pDescriptor);
                            }
                            else
                            {
                                tknError("Input attachment %u is not dynamic or fixed attachment", inputAttachmentIndex);
                            }
                            inputAttachmentDescriptors[inputAttachmentDescriptorCount] = *pDescriptor;
                            inputAttachmentDescriptorCount++;
                        }
                        else
                        {
                            tknAssert(pDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment == attachmentPtrs[inputAttachmentIndex],
                                      "Input attachment %u already set for binding %u in subpass descriptor set", inputAttachmentIndex, binding);
                        }
                    }
                    else
                    {
                        // Skip
                    }
                }
            }
        }
        destroySpvReflectShaderModule(&spvReflectShaderModules[spvPathIndex]);
    }
    tknFree(spvReflectShaderModules);
    tknFree(inputAttachmentIndexToVkImageLayout);
    updateInputAttachmentDescriptors(pGfxContext, inputAttachmentDescriptorCount, inputAttachmentDescriptors);
    tknFree(inputAttachmentDescriptors);

    Subpass subpass = {
        .pSubpassDescriptorSet = pSubpassDescriptorSet,
        .pipelinePtrDynamicArray = pipelinePtrDynamicArray,
        .subpassIndex = subpassIndex,
    };
    return subpass;
}
static void destroySubpass(GfxContext *pGfxContext, Subpass subpass)
{
    // for (uint32_t pipelinePtrIndex = 0; pipelinePtrIndex < subpass.pipelinePtrDynamicArray.count; pipelinePtrIndex++)
    // {
    //     Pipeline *pPipeline = tknGetFromDynamicArray(&subpass.pipelinePtrDynamicArray, pipelinePtrIndex);
    //     // destroyPipeline(pGfxContext, pPipeline);
    // }
    for (uint32_t descriptorIndex = 0; descriptorIndex < subpass.pSubpassDescriptorSet->descriptorCount; descriptorIndex++)
    {
        Descriptor *pDescriptor = &subpass.pSubpassDescriptorSet->descriptors[descriptorIndex];
        if (pDescriptor->vkDescriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
        {
            Attachment *pAttachment = pDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment;
            if (pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
            {
                tknRemoveFromHashSet(&pAttachment->attachmentContent.dynamicAttachmentContent.descriptorPtrHashSet, pDescriptor);
            }
            else if (pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
            {
                tknRemoveFromHashSet(&pAttachment->attachmentContent.fixedAttachmentContent.descriptorPtrHashSet, pDescriptor);
            }
            else
            {
                tknError("Input attachment is swapchian attachment");
            }
        }
    }

    destroyDescriptorSetPtr(pGfxContext, subpass.pSubpassDescriptorSet);
    tknDestroyDynamicArray(subpass.pipelinePtrDynamicArray);
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
    DescriptorSet *pDescriptorSet = tknMalloc(sizeof(DescriptorSet));
    uint32_t descriptorCount = 0;
    Descriptor *descriptors = NULL;
    VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
    TknDynamicArray vkDescriptorPoolSizeDynamicArray = tknCreateDynamicArray(sizeof(VkDescriptorPoolSize), 1);

    for (uint32_t spvReflectShaderModuleIndex = 0; spvReflectShaderModuleIndex < spvReflectShaderModuleCount; spvReflectShaderModuleIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[spvReflectShaderModuleIndex];
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
                tknError("Descriptor set %d\n", spvReflectDescriptorSet.set);
            }
        }
    }
    descriptors = tknMalloc(sizeof(Descriptor) * descriptorCount);
    VkDescriptorSetLayoutBinding *vkDescriptorSetLayoutBindings = tknMalloc(sizeof(VkDescriptorSetLayoutBinding) * descriptorCount);
    for (uint32_t binding = 0; binding < descriptorCount; binding++)
    {
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
                        VkDescriptorType vkDescriptorType = (VkDescriptorType)pSpvReflectDescriptorBinding->descriptor_type;

                        VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = {
                            .binding = binding,
                            .descriptorType = vkDescriptorType,
                            .descriptorCount = pSpvReflectDescriptorBinding->count,
                            .stageFlags = (VkShaderStageFlags)spvReflectShaderModule.shader_stage,
                            .pImmutableSamplers = NULL,
                        };
                        vkDescriptorSetLayoutBindings[binding] = vkDescriptorSetLayoutBinding;

                        descriptors[binding] = (Descriptor){
                            .vkDescriptorType = vkDescriptorType,
                            .descriptorContent = getNullDescriptorContent(vkDescriptorType),
                            .pDescriptorSet = pDescriptorSet,
                            .binding = binding,
                        };

                        uint32_t poolSizeIndex;
                        for (poolSizeIndex = 0; poolSizeIndex < vkDescriptorPoolSizeDynamicArray.count; poolSizeIndex++)
                        {
                            VkDescriptorPoolSize *pVkDescriptorPoolSize = tknGetFromDynamicArray(&vkDescriptorPoolSizeDynamicArray, poolSizeIndex);
                            if (pVkDescriptorPoolSize->type == vkDescriptorType)
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
                tknError("Descriptor set %d\n", spvReflectDescriptorSet.set);
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

    *pDescriptorSet = (DescriptorSet){
        .descriptorCount = descriptorCount,
        .descriptors = descriptors,
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPool = vkDescriptorPool,
        .vkDescriptorSet = vkDescriptorSet,
    };
    return pDescriptorSet;
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
                tknError("Uniform buffer not yet implemented");
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
    uint32_t descriptorCount = 0;
    Descriptor *descriptors = tknMalloc(sizeof(Descriptor) * pDescriptorSet->descriptorCount);
    for (uint32_t binding = 0; binding < pDescriptorSet->descriptorCount; binding++)
    {
        VkDescriptorType descriptorType = pDescriptorSet->descriptors[binding].vkDescriptorType;
        if (descriptorType != VK_DESCRIPTOR_TYPE_MAX_ENUM && descriptorType != VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
        {
            descriptors[descriptorCount] = (Descriptor){
                .vkDescriptorType = descriptorType,
                .descriptorContent = getNullDescriptorContent(descriptorType),
                .pDescriptorSet = pDescriptorSet,
                .binding = binding,
            };
            descriptorCount++;
        }
        else
        {
            // Skip
        }
    }
    updateDescriptors(pGfxContext, descriptorCount, descriptors);
    tknFree(descriptors);

    vkFreeDescriptorSets(vkDevice, pDescriptorSet->vkDescriptorPool, 1, &pDescriptorSet->vkDescriptorSet);
    vkDestroyDescriptorPool(vkDevice, pDescriptorSet->vkDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pDescriptorSet->vkDescriptorSetLayout, NULL);
    tknFree(pDescriptorSet->descriptors);
    tknFree(pDescriptorSet);
}

void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    uint32_t width = UINT32_MAX;
    uint32_t height = UINT32_MAX;
    uint32_t attachmentCount = pRenderPass->attachmentCount;
    Attachment **attachmentPtrs = pRenderPass->attachmentPtrs;
    for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
    {
        Attachment *pAttachment = attachmentPtrs[attachmentIndex];
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            if (width == UINT32_MAX && height == UINT32_MAX)
            {
                width = pGfxContext->swapchainExtent.width;
                height = pGfxContext->swapchainExtent.height;
            }
            else
            {
                tknAssert(width == pGfxContext->swapchainExtent.width && height == pGfxContext->swapchainExtent.height, "Swapchain attachment size mismatch!");
            }
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            DynamicAttachmentContent dynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
            if (width == UINT32_MAX && height == UINT32_MAX)
            {
                width = pGfxContext->swapchainExtent.width * dynamicAttachmentContent.scaler;
                height = pGfxContext->swapchainExtent.height * dynamicAttachmentContent.scaler;
            }
            else
            {
                tknAssert(width == pGfxContext->swapchainExtent.width * dynamicAttachmentContent.scaler && height == pGfxContext->swapchainExtent.height * dynamicAttachmentContent.scaler, "Dynamic attachment size mismatch!");
            }
        }
        else
        {
            FixedAttachmentContent fixedAttachmentContent = pAttachment->attachmentContent.fixedAttachmentContent;
            if (width == UINT32_MAX && height == UINT32_MAX)
            {
                width = fixedAttachmentContent.width;
                height = fixedAttachmentContent.height;
            }
            else
            {
                tknAssert(width == fixedAttachmentContent.width && height == fixedAttachmentContent.height, "Fixed attachment size mismatch!");
            }
        }
    }

    Attachment *pSwapchainAttachment = getSwapchainAttachmentPtr(pGfxContext);
    if (tknContainsInHashSet(&pSwapchainAttachment->renderPassPtrHashSet, pRenderPass))
    {
        pRenderPass->vkFramebufferCount = pGfxContext->swapchainImageCount;
        pRenderPass->vkFramebuffers = tknMalloc(sizeof(VkFramebuffer) * pGfxContext->swapchainImageCount);
        VkImageView *attachmentVkImageViews = tknMalloc(sizeof(VkImageView) * attachmentCount);
        for (uint32_t swapchainIndex = 0; swapchainIndex < pGfxContext->swapchainImageCount; swapchainIndex++)
        {
            for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
            {
                Attachment *pAttachment = attachmentPtrs[attachmentIndex];
                if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
                {
                    attachmentVkImageViews[attachmentIndex] = pGfxContext->swapchainImageViews[swapchainIndex];
                }
                else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
                {
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.dynamicAttachmentContent.vkImageView;
                }
                else
                {
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.fixedAttachmentContent.vkImageView;
                }
            }
            VkFramebufferCreateInfo vkFramebufferCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .renderPass = pRenderPass->vkRenderPass,
                .attachmentCount = attachmentCount,
                .pAttachments = attachmentVkImageViews,
                .width = width,
                .height = height,
                .layers = 1,
            };
            assertVkResult(vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pRenderPass->vkFramebuffers[swapchainIndex]));
        }
        tknFree(attachmentVkImageViews);
    }
    else
    {
        pRenderPass->vkFramebufferCount = 1;
        pRenderPass->vkFramebuffers = tknMalloc(sizeof(VkFramebuffer));
        VkImageView *attachmentVkImageViews = tknMalloc(sizeof(VkImageView) * attachmentCount);
        for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
        {
            Attachment *pAttachment = attachmentPtrs[attachmentIndex];
            if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
            {
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.dynamicAttachmentContent.vkImageView;
            }
            else
            {
                // ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.fixedAttachmentContent.vkImageView;
            }
        }
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pRenderPass->vkRenderPass,
            .attachmentCount = attachmentCount,
            .pAttachments = attachmentVkImageViews,
            .width = width,
            .height = height,
            .layers = 1,
        };
        assertVkResult(vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pRenderPass->vkFramebuffers[0]));
        tknFree(attachmentVkImageViews);
    }
}
void cleanupFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pRenderPass->vkFramebuffers[i], NULL);
    }
    tknFree(pRenderPass->vkFramebuffers);
}
void repopulateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    cleanupFramebuffers(pGfxContext, pRenderPass);
    populateFramebuffers(pGfxContext, pRenderPass);
}

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t *spvPathCounts, const char ***spvPathsArray, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex)
{
    RenderPass *pRenderPass = tknMalloc(sizeof(RenderPass));
    Attachment **attachmentPtrs = tknMalloc(sizeof(Attachment *) * attachmentCount);
    Subpass *subpasses = tknMalloc(sizeof(Subpass) * subpassCount);
    VkRenderPass vkRenderPass = VK_NULL_HANDLE;
    // Create vkRenderPass
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
    {
        Attachment *pAttachment = inputAttachmentPtrs[attachmentIndex];
        attachmentPtrs[attachmentIndex] = pAttachment;
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            vkAttachmentDescriptions[attachmentIndex].format = pGfxContext->surfaceFormat.format;
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            vkAttachmentDescriptions[attachmentIndex].format = pAttachment->attachmentContent.dynamicAttachmentContent.vkFormat;
        }
        else
        {
            vkAttachmentDescriptions[attachmentIndex].format = pAttachment->attachmentContent.fixedAttachmentContent.vkFormat;
        }
        tknAddToHashSet(&pAttachment->renderPassPtrHashSet, &pRenderPass);
    }
    VkRenderPassCreateInfo vkRenderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .attachmentCount = attachmentCount,
        .pAttachments = vkAttachmentDescriptions,
        .subpassCount = subpassCount,
        .pSubpasses = vkSubpassDescriptions,
        .dependencyCount = vkSubpassDependencyCount,
        .pDependencies = vkSubpassDependencies,
    };
    assertVkResult(vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &vkRenderPass));
    *pRenderPass = (RenderPass){
        .vkRenderPass = vkRenderPass,
        .vkFramebuffers = NULL,
        .vkFramebufferCount = 0,
        .attachmentCount = attachmentCount,
        .attachmentPtrs = attachmentPtrs,
        .subpassCount = subpassCount,
        .subpasses = subpasses,
    };

    // Create framebuffers and subpasses
    populateFramebuffers(pGfxContext, pRenderPass);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        pRenderPass->subpasses[subpassIndex] = createSubpass(pGfxContext, subpassIndex, attachmentCount, attachmentPtrs, vkSubpassDescriptions[subpassIndex].inputAttachmentCount, vkSubpassDescriptions[subpassIndex].pInputAttachments, spvPathCounts[subpassIndex], spvPathsArray[subpassIndex]);
    }
    // tknAddToDynamicArray(&pGfxContext->renderPassPtrDynamicArray, &pRenderPass, renderPassIndex);
    return pRenderPass;
}
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    // tknRemoveFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, &pRenderPass);
    cleanupFramebuffers(pGfxContext, pRenderPass);
    vkDestroyRenderPass(pGfxContext->vkDevice, pRenderPass->vkRenderPass, NULL);

    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        destroySubpass(pGfxContext, *pSubpass);
    }
    tknFree(pRenderPass->subpasses);
    for (uint32_t i = 0; i < pRenderPass->attachmentCount; i++)
    {
        Attachment *pAttachment = pRenderPass->attachmentPtrs[i];
        tknRemoveFromHashSet(&pAttachment->renderPassPtrHashSet, &pRenderPass);
    }
    tknFree(pRenderPass->attachmentPtrs);
    tknFree(pRenderPass);
}
