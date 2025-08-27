#include "gfxPipeline.h"

static Subpass createSubpass(GfxContext *pGfxContext, uint32_t subpassIndex, uint32_t attachmentCount, Attachment **attachmentPtrs, uint32_t inputVkAttachmentReferenceCount, const VkAttachmentReference *inputVkAttachmentReferences, uint32_t spvPathCount, const char **spvPaths)
{
    uint32_t inputAttachmentBindingCount = 0;
    Binding *inputAttachmentBindings = tknMalloc(sizeof(Binding) * inputVkAttachmentReferenceCount);

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
    DescriptorSet *pSubpassDescriptorSet = createDescriptorSetPtr(pGfxContext, spvPathCount, spvReflectShaderModules, TKN_SUBPASS_DESCRIPTOR_SET);
    Material *pMaterial = createMaterialPtr(pGfxContext, pSubpassDescriptorSet);
    // Bind attachments
    // TODO bind others
    for (uint32_t spvPathIndex = 0; spvPathIndex < spvPathCount; spvPathIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[spvPathIndex];
        for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
            if (TKN_SUBPASS_DESCRIPTOR_SET == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    if (SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == pSpvReflectDescriptorBinding->descriptor_type)
                    {
                        uint32_t binding = pSpvReflectDescriptorBinding->binding;
                        uint32_t inputAttachmentIndex = pSpvReflectDescriptorBinding->input_attachment_index;
                        Binding *pBinding = &pMaterial->bindings[binding];
                        if (NULL == pBinding->bindingUnion.inputAttachmentBinding.pAttachment)
                        {
                            tknAssert(inputAttachmentIndex < attachmentCount, "Input attachment index %u out of bounds", inputAttachmentIndex);
                            pBinding->bindingUnion.inputAttachmentBinding.pAttachment = attachmentPtrs[inputAttachmentIndex];
                            pBinding->bindingUnion.inputAttachmentBinding.vkImageLayout = inputAttachmentIndexToVkImageLayout[inputAttachmentIndex];
                            if (ATTACHMENT_TYPE_DYNAMIC == attachmentPtrs[inputAttachmentIndex]->attachmentType)
                            {
                                tknAddToHashSet(&attachmentPtrs[inputAttachmentIndex]->attachmentUnion.dynamicAttachment.bindingPtrHashSet, pBinding);
                            }
                            else if (ATTACHMENT_TYPE_FIXED == attachmentPtrs[inputAttachmentIndex]->attachmentType)
                            {
                                tknAddToHashSet(&attachmentPtrs[inputAttachmentIndex]->attachmentUnion.fixedAttachment.bindingPtrHashSet, pBinding);
                            }
                            else
                            {
                                tknError("Input attachment %u is not dynamic or fixed attachment", inputAttachmentIndex);
                            }
                            inputAttachmentBindings[inputAttachmentBindingCount] = *pBinding;
                            inputAttachmentBindingCount++;
                        }
                        else
                        {
                            tknAssert(pBinding->bindingUnion.inputAttachmentBinding.pAttachment == attachmentPtrs[inputAttachmentIndex],
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
    updateInputAttachmentBindings(pGfxContext, inputAttachmentBindingCount, inputAttachmentBindings);
    tknFree(inputAttachmentBindings);
    TknDynamicArray pipelinePtrDynamicArray = tknCreateDynamicArray(sizeof(Pipeline *), TKN_DEFAULT_COLLECTION_SIZE);

    Subpass subpass = {
        .pSubpassDescriptorSet = pSubpassDescriptorSet,
        .pipelinePtrDynamicArray = pipelinePtrDynamicArray,
    };
    return subpass;
}
static void destroySubpass(GfxContext *pGfxContext, Subpass subpass)
{
    for (uint32_t pipelinePtrIndex = 0; pipelinePtrIndex < subpass.pipelinePtrDynamicArray.count; pipelinePtrIndex++)
    {
        Pipeline *pPipeline = *(Pipeline **)tknGetFromDynamicArray(&subpass.pipelinePtrDynamicArray, pipelinePtrIndex);
        destroyPipelinePtr(pGfxContext, pPipeline);
    }
    tknAssert(subpass.pSubpassDescriptorSet->materialPtrDynamicArray.count == 1, "Subpass must have exactly one material");
    Material *pMaterial = *(Material **)tknGetFromDynamicArray(&subpass.pSubpassDescriptorSet->materialPtrDynamicArray, 0);
    for (uint32_t descriptorIndex = 0; descriptorIndex < subpass.pSubpassDescriptorSet->descriptorCount; descriptorIndex++)
    {
        Binding *pBinding = &pMaterial->bindings[descriptorIndex];
        if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == pBinding->vkDescriptorType)
        {
            Attachment *pAttachment = pBinding->bindingUnion.inputAttachmentBinding.pAttachment;
            if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
            {
                tknRemoveFromHashSet(&pAttachment->attachmentUnion.dynamicAttachment.bindingPtrHashSet, pBinding);
            }
            else if (ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType)
            {
                tknRemoveFromHashSet(&pAttachment->attachmentUnion.fixedAttachment.bindingPtrHashSet, pBinding);
            }
            else
            {
                tknError("Input attachment is swapchain attachment");
            }
        }
        else
        {
            
        }
    }
    destroyDescriptorSetPtr(pGfxContext, subpass.pSubpassDescriptorSet);
    tknDestroyDynamicArray(subpass.pipelinePtrDynamicArray);
}
static void updateVkVertexInputAttributeDescriptions(VertexInputLayout vertexInputLayout, uint32_t attributeIndex, SpvReflectInterfaceVariable spvReflectInterfaceVariable, uint32_t binding, VkVertexInputAttributeDescription *vkVertexInputAttributeDescriptions, uint32_t *pVkVertexInputAttributeDescriptionCount)
{
    SpvReflectTypeFlagBits typeFlags = spvReflectInterfaceVariable.type_description->type_flags;
    uint32_t location = spvReflectInterfaceVariable.location;
    VkFormat vkFormat = (VkFormat)spvReflectInterfaceVariable.format;
    if (typeFlags & SPV_REFLECT_TYPE_FLAG_ARRAY)
    {
        uint32_t itemCount = 1;
        for (uint32_t dimIndex = 0; dimIndex < spvReflectInterfaceVariable.array.dims_count; dimIndex++)
        {
            itemCount *= spvReflectInterfaceVariable.array.dims[dimIndex];
        }
        if (typeFlags & SPV_REFLECT_TYPE_FLAG_MATRIX)
        {
            uint32_t vectorCount = spvReflectInterfaceVariable.numeric.matrix.column_count * itemCount;
            for (uint32_t vectorIndex = 0; vectorIndex < vectorCount; vectorIndex++)
            {
                vkVertexInputAttributeDescriptions[*pVkVertexInputAttributeDescriptionCount] = (VkVertexInputAttributeDescription){
                    .location = location + vectorIndex,
                    .binding = binding,
                    .format = vkFormat,
                    .offset = vertexInputLayout.offsets[attributeIndex] + vectorIndex * getSizeOfVkFormat(vkFormat),
                };
                (*pVkVertexInputAttributeDescriptionCount)++;
            }
        }
        else
        {
            for (uint32_t itemIndex = 0; itemIndex < itemCount; itemIndex++)
            {
                vkVertexInputAttributeDescriptions[*pVkVertexInputAttributeDescriptionCount] = (VkVertexInputAttributeDescription){
                    .location = location + itemIndex,
                    .binding = binding,
                    .format = vkFormat,
                    .offset = vertexInputLayout.offsets[attributeIndex] + itemIndex * getSizeOfVkFormat(vkFormat),
                };
                (*pVkVertexInputAttributeDescriptionCount)++;
            }
        }
    }
    else
    {
        if (typeFlags & SPV_REFLECT_TYPE_FLAG_MATRIX)
        {
            uint32_t vectorCount = spvReflectInterfaceVariable.numeric.matrix.column_count;
            for (uint32_t vectorIndex = 0; vectorIndex < vectorCount; vectorIndex++)
            {
                vkVertexInputAttributeDescriptions[*pVkVertexInputAttributeDescriptionCount] = (VkVertexInputAttributeDescription){
                    .location = location + vectorIndex,
                    .binding = binding,
                    .format = vkFormat,
                    .offset = vertexInputLayout.offsets[attributeIndex] + vectorIndex * getSizeOfVkFormat(vkFormat),
                };
                (*pVkVertexInputAttributeDescriptionCount)++;
            }
        }
        else
        {
            vkVertexInputAttributeDescriptions[*pVkVertexInputAttributeDescriptionCount] = (VkVertexInputAttributeDescription){
                .location = location,
                .binding = binding,
                .format = vkFormat,
                .offset = vertexInputLayout.offsets[attributeIndex],
            };
            (*pVkVertexInputAttributeDescriptionCount)++;
        }
    }
}

VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (uint32_t i = 0; i < candidateCount; i++)
    {
        VkFormat format = candidates[i];
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(pGfxContext->vkPhysicalDevice, format, &props);
        if (VK_IMAGE_TILING_LINEAR == tiling)
        {
            if ((props.linearTilingFeatures & features) == features)
            {
                return format;
            }
        }
        else if (VK_IMAGE_TILING_OPTIMAL == tiling)
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
    TknDynamicArray materialPtrDynamicArray = tknCreateDynamicArray(sizeof(Material *), TKN_MIN_COLLECTION_SIZE);
    VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
    TknDynamicArray vkDescriptorPoolSizeDynamicArray = tknCreateDynamicArray(sizeof(VkDescriptorPoolSize), TKN_DEFAULT_COLLECTION_SIZE);
    uint32_t descriptorCount = 0;
    VkDescriptorType *vkDescriptorTypes = NULL;

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
                tknError("Skipping descriptor set %d because of target set %d\n ", spvReflectDescriptorSet.set, set);
            }
        }
    }

    vkDescriptorTypes = tknMalloc(sizeof(VkDescriptorType) * descriptorCount);
    VkDescriptorSetLayoutBinding *vkDescriptorSetLayoutBindings = tknMalloc(sizeof(VkDescriptorSetLayoutBinding) * descriptorCount);
    for (uint32_t binding = 0; binding < descriptorCount; binding++)
    {
        vkDescriptorTypes[binding] = VK_DESCRIPTOR_TYPE_MAX_ENUM;
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
                    if (VK_DESCRIPTOR_TYPE_MAX_ENUM == vkDescriptorSetLayoutBindings[binding].descriptorType)
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
                        vkDescriptorTypes[binding] = vkDescriptorType;

                        uint32_t poolSizeIndex;
                        for (poolSizeIndex = 0; poolSizeIndex < vkDescriptorPoolSizeDynamicArray.count; poolSizeIndex++)
                        {
                            VkDescriptorPoolSize *pVkDescriptorPoolSize = tknGetFromDynamicArray(&vkDescriptorPoolSizeDynamicArray, poolSizeIndex);
                            if (vkDescriptorType == pVkDescriptorPoolSize->type)
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
                            tknAddToDynamicArray(&vkDescriptorPoolSizeDynamicArray, &vkDescriptorPoolSize);
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

    *pDescriptorSet = (DescriptorSet){
        .materialPtrDynamicArray = materialPtrDynamicArray,
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPoolSizeDynamicArray = vkDescriptorPoolSizeDynamicArray,
        .descriptorCount = descriptorCount,
        .vkDescriptorTypes = vkDescriptorTypes,
    };
    return pDescriptorSet;
}

void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet)
{
    for (uint32_t i = 0; i < pDescriptorSet->materialPtrDynamicArray.count; i++)
    {
        Material *pMaterial = *(Material **)tknGetFromDynamicArray(&pDescriptorSet->materialPtrDynamicArray, i);
        destroyMaterialPtr(pGfxContext, pMaterial);
    }
    tknDestroyDynamicArray(pDescriptorSet->materialPtrDynamicArray);
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkDestroyDescriptorSetLayout(vkDevice, pDescriptorSet->vkDescriptorSetLayout, NULL);
    tknDestroyDynamicArray(pDescriptorSet->vkDescriptorPoolSizeDynamicArray);
    tknFree(pDescriptorSet->vkDescriptorTypes);
    tknFree(pDescriptorSet);
}
void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    uint32_t width = UINT32_MAX;
    uint32_t height = UINT32_MAX;
    uint32_t attachmentCount = pRenderPass->attachmentCount;
    Attachment **attachmentPtrs = pRenderPass->attachmentPtrs;
    SwapchainAttachment *pSwapchainUnion = &pGfxContext->pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    uint32_t swapchainWidth = pSwapchainUnion->swapchainExtent.width;
    uint32_t swapchainHeight = pSwapchainUnion->swapchainExtent.height;
    for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
    {
        Attachment *pAttachment = attachmentPtrs[attachmentIndex];
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            if (UINT32_MAX == width && UINT32_MAX == height)
            {
                width = swapchainWidth;
                height = swapchainHeight;
            }
            else
            {
                tknAssert(width == swapchainWidth && height == swapchainHeight, "Swapchain attachment size mismatch!");
            }
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            DynamicAttachment dynamicUnion = pAttachment->attachmentUnion.dynamicAttachment;
            uint32_t dynamicWidth = swapchainWidth * dynamicUnion.scaler;
            uint32_t dynamicHeight = swapchainHeight * dynamicUnion.scaler;
            if (UINT32_MAX == width && UINT32_MAX == height)
            {
                width = dynamicWidth;
                height = dynamicHeight;
            }
            else
            {
                tknAssert(width == dynamicWidth && height == dynamicHeight, "Dynamic attachment size mismatch!");
            }
        }
        else
        {
            FixedAttachment fixedUnion = pAttachment->attachmentUnion.fixedAttachment;
            if (UINT32_MAX == width && UINT32_MAX == height)
            {
                width = fixedUnion.width;
                height = fixedUnion.height;
            }
            else
            {
                tknAssert(width == fixedUnion.width && height == fixedUnion.height, "Fixed attachment size mismatch!");
            }
        }
    }

    tknAssert(UINT32_MAX != width && UINT32_MAX != height, "No valid attachment found to determine framebuffer size");
    pRenderPass->renderArea = (VkRect2D){
        .offset = {0, 0},
        .extent = {width, height},
    };
    Attachment *pSwapchainAttachment = getSwapchainAttachmentPtr(pGfxContext);
    if (tknContainsInHashSet(&pSwapchainAttachment->renderPassPtrHashSet, pRenderPass))
    {
        uint32_t swapchainImageCount = pSwapchainUnion->swapchainImageCount;
        pRenderPass->vkFramebufferCount = swapchainImageCount;
        pRenderPass->vkFramebuffers = tknMalloc(sizeof(VkFramebuffer) * swapchainImageCount);
        VkImageView *attachmentVkImageViews = tknMalloc(sizeof(VkImageView) * attachmentCount);
        for (uint32_t swapchainIndex = 0; swapchainIndex < swapchainImageCount; swapchainIndex++)
        {
            for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
            {
                Attachment *pAttachment = attachmentPtrs[attachmentIndex];
                if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
                {
                    attachmentVkImageViews[attachmentIndex] = pSwapchainUnion->swapchainImageViews[swapchainIndex];
                }
                else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
                {
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentUnion.dynamicAttachment.vkImageView;
                }
                else
                {
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentUnion.fixedAttachment.vkImageView;
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
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentUnion.dynamicAttachment.vkImageView;
            }
            else
            {
                // ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentUnion.fixedAttachment.vkImageView;
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

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, VkClearValue *vkClearValues, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t *spvPathCounts, const char ***spvPathsArray, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies)
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
        vkAttachmentDescriptions[attachmentIndex].format = pAttachment->vkFormat;
        tknAddToHashSet(&pAttachment->renderPassPtrHashSet, pRenderPass);
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
        .vkClearValues = vkClearValues,
        .subpassCount = subpassCount,
        .subpasses = subpasses,
    };

    // Create framebuffers and subpasses
    populateFramebuffers(pGfxContext, pRenderPass);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        pRenderPass->subpasses[subpassIndex] = createSubpass(pGfxContext, subpassIndex, attachmentCount, attachmentPtrs, vkSubpassDescriptions[subpassIndex].inputAttachmentCount, vkSubpassDescriptions[subpassIndex].pInputAttachments, spvPathCounts[subpassIndex], spvPathsArray[subpassIndex]);
    }
    tknAddToDynamicArray(&pGfxContext->renderPassPtrDynamicArray, &pRenderPass);
    return pRenderPass;
}
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    tknRemoveFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, &pRenderPass);
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
        tknRemoveFromHashSet(&pAttachment->renderPassPtrHashSet, pRenderPass);
    }
    tknFree(pRenderPass->attachmentPtrs);
    tknFree(pRenderPass);
}

Pipeline *createPipelinePtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex, uint32_t spvPathCount, const char **spvPaths, VertexInputLayout *pMeshVertexInputLayout, VertexInputLayout *pInstanceVertexInputLayout, VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo, VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo, VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo, VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo, VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo, VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo)
{
    Pipeline *pPipeline = tknMalloc(sizeof(Pipeline));
    SpvReflectShaderModule *spvReflectShaderModules = tknMalloc(sizeof(SpvReflectShaderModule) * spvPathCount);
    VkShaderStageFlagBits vkShaderStageFlagBits = 0;
    VkPipelineShaderStageCreateInfo *pipelineShaderStageCreateInfos = tknMalloc(sizeof(VkPipelineShaderStageCreateInfo) * spvPathCount);
    uint32_t vkVertexInputBindingDescriptionCount = 0;
    VkVertexInputBindingDescription *vkVertexInputBindingDescriptions = NULL;
    uint32_t vkVertexInputAttributeDescriptionCount = 0;
    VkVertexInputAttributeDescription *vkVertexInputAttributeDescriptions = NULL;
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t spvPathIndex = 0; spvPathIndex < spvPathCount; spvPathIndex++)
    {
        spvReflectShaderModules[spvPathIndex] = createSpvReflectShaderModule(spvPaths[spvPathIndex]);
    }
    DescriptorSet *pPipelineDescriptorSet = createDescriptorSetPtr(pGfxContext, spvPathCount, spvReflectShaderModules, TKN_GLOBAL_DESCRIPTOR_SET);
    for (uint32_t spvPathIndex = 0; spvPathIndex < spvPathCount; spvPathIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[spvPathIndex];
        if (VK_SHADER_STAGE_VERTEX_BIT == spvReflectShaderModule.shader_stage)
        {
            if (pMeshVertexInputLayout->attributeCount > 0 || pInstanceVertexInputLayout->attributeCount > 0)
            {
                vkVertexInputBindingDescriptionCount = MAX_VERTEX_BINDING_DESCRIPTION;
                vkVertexInputBindingDescriptions = tknMalloc(sizeof(VkVertexInputBindingDescription) * vkVertexInputBindingDescriptionCount);
                vkVertexInputBindingDescriptions[VERTEX_BINDING_DESCRIPTION] = (VkVertexInputBindingDescription){
                    .binding = VERTEX_BINDING_DESCRIPTION,
                    .stride = pMeshVertexInputLayout->stride,
                    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                };
                vkVertexInputBindingDescriptions[INSTANCE_BINDING_DESCRIPTION] = (VkVertexInputBindingDescription){
                    .binding = INSTANCE_BINDING_DESCRIPTION,
                    .stride = pInstanceVertexInputLayout->stride,
                    .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
                };
                uint32_t vkVertexInputAttributeDescriptionCount = 0;
                VkVertexInputAttributeDescription *vkVertexInputAttributeDescriptions = tknMalloc(sizeof(VkVertexInputAttributeDescription) * pGfxContext->vkPhysicalDeviceProperties.limits.maxVertexInputAttributes);
                for (uint32_t inputVariableIndex = 0; inputVariableIndex < spvReflectShaderModule.input_variable_count; inputVariableIndex++)
                {
                    SpvReflectInterfaceVariable *pSpvReflectInterfaceVariable = spvReflectShaderModule.input_variables[inputVariableIndex];

                    uint32_t attributeIndex;
                    for (attributeIndex = 0; attributeIndex < pMeshVertexInputLayout->attributeCount; attributeIndex++)
                    {
                        if (0 == strcmp(pSpvReflectInterfaceVariable->name, pMeshVertexInputLayout->names[attributeIndex]))
                        {
                            updateVkVertexInputAttributeDescriptions(*pMeshVertexInputLayout, attributeIndex, *pSpvReflectInterfaceVariable, VERTEX_BINDING_DESCRIPTION, vkVertexInputAttributeDescriptions, &vkVertexInputAttributeDescriptionCount);
                            break;
                        }
                    }
                    if (attributeIndex < pMeshVertexInputLayout->attributeCount)
                    {
                        continue;
                    }
                    else
                    {
                        attributeIndex = 0;
                        for (attributeIndex = 0; attributeIndex < pInstanceVertexInputLayout->attributeCount; attributeIndex++)
                        {
                            updateVkVertexInputAttributeDescriptions(*pInstanceVertexInputLayout, attributeIndex, *pSpvReflectInterfaceVariable, INSTANCE_BINDING_DESCRIPTION, vkVertexInputAttributeDescriptions, &vkVertexInputAttributeDescriptionCount);
                            break;
                        }
                        tknAssert(attributeIndex < pInstanceVertexInputLayout->attributeCount, "Attribute not found");
                    }
                }
            }
            else
            {
                // Skip
            }
        }
        if ((vkShaderStageFlagBits & spvReflectShaderModule.shader_stage) == 0)
        {
            tknError("Incompatible shader stage");
        }
        else
        {
            vkShaderStageFlagBits |= spvReflectShaderModule.shader_stage;
            VkShaderModuleCreateInfo vkShaderModuleCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .codeSize = spvReflectGetCodeSize(&spvReflectShaderModule),
                .pCode = spvReflectGetCode(&spvReflectShaderModule),
            };
            VkShaderModule shaderModule;
            assertVkResult(vkCreateShaderModule(vkDevice, &vkShaderModuleCreateInfo, NULL, &shaderModule));
            pipelineShaderStageCreateInfos[spvPathIndex] = (VkPipelineShaderStageCreateInfo){
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .stage = (VkShaderStageFlagBits)spvReflectShaderModule.shader_stage,
                .module = shaderModule,
                .pName = spvReflectShaderModule.entry_point_name,
                .pSpecializationInfo = NULL,
            };
        }
        destroySpvReflectShaderModule(&spvReflectShaderModules[spvPathIndex]);
    }
    tknFree(spvReflectShaderModules);
    VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = vkVertexInputBindingDescriptionCount,
        .pVertexBindingDescriptions = vkVertexInputBindingDescriptions,
        .vertexAttributeDescriptionCount = vkVertexInputAttributeDescriptionCount,
        .pVertexAttributeDescriptions = vkVertexInputAttributeDescriptions,
    };
    VkPipelineLayout vkPipelineLayout;
    VkDescriptorSetLayout *vkDescriptorSetLayouts = tknMalloc(sizeof(VkDescriptorSetLayout) * TKN_MAX_DESCRIPTOR_SET);
    vkDescriptorSetLayouts[TKN_GLOBAL_DESCRIPTOR_SET] = pGfxContext->pGlobalDescriptorSet->vkDescriptorSetLayout;
    vkDescriptorSetLayouts[TKN_SUBPASS_DESCRIPTOR_SET] = pRenderPass->subpasses[subpassIndex].pSubpassDescriptorSet->vkDescriptorSetLayout;
    vkDescriptorSetLayouts[TKN_PIPELINE_DESCRIPTOR_SET] = pPipelineDescriptorSet->vkDescriptorSetLayout;
    VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = TKN_MAX_DESCRIPTOR_SET,
        .pSetLayouts = vkDescriptorSetLayouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };
    assertVkResult(vkCreatePipelineLayout(vkDevice, &vkPipelineLayoutCreateInfo, NULL, &vkPipelineLayout));
    VkPipeline vkPipeline = VK_NULL_HANDLE;
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = spvPathCount,
        .pStages = pipelineShaderStageCreateInfos,
        .pVertexInputState = &vkPipelineVertexInputStateCreateInfo,
        .pInputAssemblyState = &vkPipelineInputAssemblyStateCreateInfo,
        .pTessellationState = NULL,
        .pViewportState = &vkPipelineViewportStateCreateInfo,
        .pRasterizationState = &vkPipelineRasterizationStateCreateInfo,
        .pMultisampleState = &vkPipelineMultisampleStateCreateInfo,
        .pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo,
        .pColorBlendState = &vkPipelineColorBlendStateCreateInfo,
        .pDynamicState = &vkPipelineDynamicStateCreateInfo,
        .layout = vkPipelineLayout,
        .renderPass = pRenderPass->vkRenderPass,
        .subpass = subpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };
    tknFree(vkVertexInputBindingDescriptions);
    tknFree(vkVertexInputAttributeDescriptions);
    for (uint32_t pipelineShaderStageCreateInfoIndex = 0; pipelineShaderStageCreateInfoIndex < spvPathCount; pipelineShaderStageCreateInfoIndex++)
    {
        vkDestroyShaderModule(vkDevice, pipelineShaderStageCreateInfos[pipelineShaderStageCreateInfoIndex].module, NULL);
    }
    tknFree(vkDescriptorSetLayouts);
    tknFree(pipelineShaderStageCreateInfos);
    assertVkResult(vkCreateGraphicsPipelines(vkDevice, NULL, 1, &vkGraphicsPipelineCreateInfo, NULL, &vkPipeline));

    *pPipeline = (Pipeline){
        .pPipelineDescriptorSet = pPipelineDescriptorSet,
        .vkPipeline = vkPipeline,
        .vkPipelineLayout = vkPipelineLayout,
        .pRenderPass = pRenderPass,
        .subpassIndex = subpassIndex,
        .pMeshVertexInputLayout = pMeshVertexInputLayout,
        .pInstanceVertexInputLayout = pInstanceVertexInputLayout,
    };
    tknAddToDynamicArray(&pRenderPass->subpasses[subpassIndex].pipelinePtrDynamicArray, pPipeline);
    tknAddToHashSet(&pMeshVertexInputLayout->referencePtrHashSet, pPipeline);
    tknAddToHashSet(&pInstanceVertexInputLayout->referencePtrHashSet, pPipeline);
    return pPipeline;
}
void destroyPipelinePtr(GfxContext *pGfxContext, Pipeline *pPipeline)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    tknRemoveFromDynamicArray(&pPipeline->pRenderPass->subpasses[pPipeline->subpassIndex].pipelinePtrDynamicArray, pPipeline);
    tknRemoveFromHashSet(&pPipeline->pMeshVertexInputLayout->referencePtrHashSet, pPipeline);
    tknRemoveFromHashSet(&pPipeline->pInstanceVertexInputLayout->referencePtrHashSet, pPipeline);
    destroyDescriptorSetPtr(pGfxContext, pPipeline->pPipelineDescriptorSet);
    vkDestroyPipeline(vkDevice, pPipeline->vkPipeline, NULL);
    vkDestroyPipelineLayout(vkDevice, pPipeline->vkPipelineLayout, NULL);
    tknFree(pPipeline);
}