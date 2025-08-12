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

static Subpass createSubpass(GfxContext *pGfxContext, uint32_t inputVkAttachmentReferenceCount, const VkAttachmentReference *inputVkAttachmentReferences, uint32_t spvPathCount, const char **spvPaths, Attachment **attachmentPtrs)
{
    TknDynamicArray pipelinePtrDynamicArray = tknCreateDynamicArray(sizeof(Pipeline *), 1);
    SpvReflectShaderModule *spvReflectShaderModules = tknMalloc(sizeof(SpvReflectShaderModule) * spvPathCount);
    for (uint32_t pathIndex = 0; pathIndex < spvPathCount; pathIndex++)
    {
        const char *spvPath = spvPaths[pathIndex];
        spvReflectShaderModules[pathIndex] = createSpvReflectShaderModule(spvPath);
    }
    DescriptorSet *pSubpassDescriptorSet = createDescriptorSetPtr(pGfxContext, spvPathCount, spvReflectShaderModules, TICKERNEL_SUBPASS_DESCRIPTOR_SET);
    // Collect all INPUT_ATTACHMENT descriptors for batch update
    TknDynamicArray inputAttachmentDescriptorDynamicArray = tknCreateDynamicArray(sizeof(Descriptor), 4);

    for (uint32_t pathIndex = 0; pathIndex < spvPathCount; pathIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[pathIndex];
        for (uint32_t descriptorSetIndex = 0; descriptorSetIndex < spvReflectShaderModule.descriptor_set_count; descriptorSetIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[descriptorSetIndex];
            if (TICKERNEL_SUBPASS_DESCRIPTOR_SET == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    uint32_t binding = pSpvReflectDescriptorBinding->binding;
                    if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == (VkDescriptorType)pSpvReflectDescriptorBinding->descriptor_type)
                    {
                        InputAttachmentDescriptorContent inputAttachmentDescriptorContent = {
                            .pAttachment = attachmentPtrs[pSpvReflectDescriptorBinding->input_attachment_index],
                            .vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        };
                        Descriptor descriptor = {
                            .vkDescriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                            .descriptorContent.inputAttachmentDescriptorContent = inputAttachmentDescriptorContent,
                            .pDescriptorSet = pSubpassDescriptorSet,
                            .binding = binding,
                        };
                        uint32_t inputVkAttachmentReferenceIndex;
                        for (inputVkAttachmentReferenceIndex = 0; inputVkAttachmentReferenceIndex < inputVkAttachmentReferenceCount; inputVkAttachmentReferenceIndex++)
                        {
                            VkAttachmentReference ref = inputVkAttachmentReferences[inputVkAttachmentReferenceIndex];
                            if (ref.attachment == pSpvReflectDescriptorBinding->input_attachment_index)
                            {
                                pSubpassDescriptorSet->descriptors[binding].descriptorContent.inputAttachmentDescriptorContent.vkImageLayout = ref.layout;
                                break;
                            }
                        }
                        tknAssert(inputVkAttachmentReferenceIndex < inputVkAttachmentReferenceCount, "Input attachment reference not found for binding %d in shader %s", binding, spvPaths[pathIndex]);
                        tknAddToDynamicArray(&inputAttachmentDescriptorDynamicArray, &descriptor, inputAttachmentDescriptorDynamicArray.count);
                    }
                }
                // Skip other sets.
                break;
            }
            else
            {
                // Skip
                printf("Warning: Descriptor set %d\n", spvReflectDescriptorSet.set);
            }
            destroySpvReflectShaderModule(&spvReflectShaderModule);
        }
    }

    // Batch update all INPUT_ATTACHMENT descriptors
    if (inputAttachmentDescriptorDynamicArray.count > 0)
    {
        updateDescriptors(pGfxContext, inputAttachmentDescriptorDynamicArray.count, inputAttachmentDescriptorDynamicArray.array);
    }
    tknDestroyDynamicArray(inputAttachmentDescriptorDynamicArray);
    tknFree(spvReflectShaderModules);

    Subpass subpass = {
        .pSubpassDescriptorSet = pSubpassDescriptorSet,
        .pipelinePtrDynamicArray = pipelinePtrDynamicArray,
    };
    return subpass;
}
static void destroySubpass(GfxContext *pGfxContext, Subpass subpass)
{
    for (uint32_t j = 0; j < subpass.pipelinePtrDynamicArray.count; j++)
    {
        Pipeline *pPipeline = tknGetFromDynamicArray(&subpass.pipelinePtrDynamicArray, j);
        // destroyPipeline(pGfxContext, pPipeline);
    }
    destroyDescriptorSetPtr(pGfxContext, subpass.pSubpassDescriptorSet);
    tknDestroyDynamicArray(subpass.pipelinePtrDynamicArray);
    // assertVkResult(vkFreeDescriptorSets(vkDevice, pSubpass->vkDescriptorPool, 1, &pSubpass->vkDescriptorSet));
    // vkDestroyDescriptorPool(vkDevice, pSubpass->vkDescriptorPool, NULL);
    // vkDestroyDescriptorSetLayout(vkDevice, pSubpass->vkDescriptorSetLayout, NULL);
    // tknDestroyDynamicArray(pSubpass->pipelinePtrDynamicArray);
    // tknFree(pSubpass->descriptors);
    // tknFree(pSubpass->vkDescriptorSetLayoutBindings);
}

bool hasSwapchain(RenderPass renderPass)
{
    for (uint32_t attachmentPtrIndex = 0; attachmentPtrIndex < renderPass.attachmentCount; attachmentPtrIndex++)
    {
        Attachment *pAttachment = renderPass.attachmentPtrs[attachmentPtrIndex];
        if (pAttachment->attachmentType == ATTACHMENT_TYPE_SWAPCHAIN)
        {
            return true;
        }
    }
    return false;
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
                tknError("Descriptor set %d\n", spvReflectDescriptorSet.set);
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
            .pDescriptorSet = pDescriptorSet,
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
                            .pDescriptorSet = pDescriptorSet,
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
            // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
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
            else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorType)
            {
                InputAttachmentDescriptorContent newDescriptorContent = newDescriptor.descriptorContent.inputAttachmentDescriptorContent;
                Descriptor *pCurrentDescriptor = &pCurrentDescriptorSet->descriptors[binding];
                Attachment *pCurrentAttachment = pCurrentDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment;
                Attachment *pNewAttachment = newDescriptorContent.pAttachment;
                if (pCurrentAttachment == pNewAttachment)
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
                        if (pCurrentAttachment->attachmentType == ATTACHMENT_TYPE_SWAPCHAIN)
                        {
                            // Swapchain attachments are handled at render time.
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
                        else if (pCurrentAttachment->attachmentType == ATTACHMENT_TYPE_SWAPCHAIN)
                        {
                            // Swapchain attachments are handled at render time.
                        }
                        else
                        {
                            tknError("Unsupported attachment type: %d", pNewAttachment->attachmentType);
                        }
                        pCurrentDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment = newDescriptorContent.pAttachment;
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
                        printf("Input attachment binding %d, image view: %p, layout: %d\n", binding, vkDescriptorImageInfos[vkWriteDescriptorSetCount].imageView, pCurrentDescriptor->descriptorContent.inputAttachmentDescriptorContent.vkImageLayout);

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

    if (hasSwapchain(*pRenderPass))
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
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.dynamicAttachmentContent.pImage->vkImageView;
                }
                else
                {
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.fixedAttachmentContent.pImage->vkImageView;
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
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.dynamicAttachmentContent.pImage->vkImageView;
            }
            else
            {
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.fixedAttachmentContent.pImage->vkImageView;
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
            vkAttachmentDescriptions[attachmentIndex].format = pAttachment->attachmentContent.dynamicAttachmentContent.pImage->vkFormat;
        }
        else
        {
            vkAttachmentDescriptions[attachmentIndex].format = pAttachment->attachmentContent.fixedAttachmentContent.pImage->vkFormat;
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
        pRenderPass->subpasses[subpassIndex] = createSubpass(pGfxContext, vkSubpassDescriptions[subpassIndex].inputAttachmentCount, vkSubpassDescriptions[subpassIndex].pInputAttachments, spvPathCounts[subpassIndex], spvPathsArray[subpassIndex], attachmentPtrs);
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
