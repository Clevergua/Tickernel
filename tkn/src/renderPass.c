#include "gfxCore.h"

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
