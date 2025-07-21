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

static DescriptorBinding getDefaultDescriptorBinding(VkDescriptorType descriptorType)
{
    DescriptorBinding descriptorBinding = {0};
    if (VK_DESCRIPTOR_TYPE_SAMPLER == descriptorType)
    {
        descriptorBinding.samplerDescriptorBinding.pSampler = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == descriptorType)
    {
        descriptorBinding.combinedImageSamplerDescriptorBinding.pImage = NULL;
        descriptorBinding.combinedImageSamplerDescriptorBinding.pSampler = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE == descriptorType)
    {
        descriptorBinding.sampledImageDescriptorBinding.pImage = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_STORAGE_IMAGE == descriptorType)
    {
        descriptorBinding.storageImageDescriptorBinding.pImage = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER == descriptorType)
    {
        descriptorBinding.uniformTexelBufferDescriptorBinding.pBuffer = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER == descriptorType)
    {
        descriptorBinding.storageTexelBufferDescriptorBinding.pBuffer = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER == descriptorType)
    {
        descriptorBinding.uniformBufferDescriptorBinding.pBuffer = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER == descriptorType)
    {
        descriptorBinding.storageBufferDescriptorBinding.pBuffer = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC == descriptorType)
    {
        descriptorBinding.uniformBufferDynamicDescriptorBinding.pBuffer = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC == descriptorType)
    {
        descriptorBinding.storageBufferDynamicDescriptorBinding.pBuffer = NULL;
    }
    else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == descriptorType)
    {
        descriptorBinding.inputAttachmentDescriptorBinding.pAttachment = NULL;
    }
    else
    {
        tknError("Unsupported descriptor type: %d", descriptorType);
    }
    return descriptorBinding;
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

    if (pRenderPass->useSwapchain)
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
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.dynamicAttachmentContent.image.vkImageView;
                }
                else
                {
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.fixedAttachmentContent.image.vkImageView;
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
            ASSERT_VK_SUCCESS(vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pRenderPass->vkFramebuffers[swapchainIndex]));
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
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.dynamicAttachmentContent.image.vkImageView;
            }
            else
            {
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentContent.fixedAttachmentContent.image.vkImageView;
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
        ASSERT_VK_SUCCESS(vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pRenderPass->vkFramebuffers[0]));
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

Subpass createSubpass(GfxContext *pGfxContext, uint32_t inputVkAttachmentReferenceCount, const VkAttachmentReference *inputVkAttachmentReferences, TknDynamicArray spvPathDynamicArray, Attachment **attachmentPtrs)
{
    // for updating descriptor sets
    uint32_t descriptorBindingCount = 0;
    DescriptorBinding *descriptorBindings = NULL;
    VkDescriptorSetLayoutBinding *vkDescriptorSetLayoutBindings = NULL;
    // for creating descriptor set
    VkDescriptorSetLayout vkDescriptorSetLayout;
    // for creating descriptor pool
    VkDescriptorPool vkDescriptorPool;
    // subpass descriptor set
    VkDescriptorSet vkDescriptorSet;
    // pipelines
    TknDynamicArray pipelinePtrDynamicArray = tknCreateDynamicArray(sizeof(Pipeline *), 1);
    TknDynamicArray vkDescriptorPoolSizeDynamicArray = tknCreateDynamicArray(sizeof(VkDescriptorPoolSize), 1);
    for (uint32_t pathIndex = 0; pathIndex < spvPathDynamicArray.count; pathIndex++)
    {
        const char **pSpvPath = tknGetFromDynamicArray(&spvPathDynamicArray, pathIndex);
        SpvReflectShaderModule spvReflectShaderModule = createSpvReflectShaderModule(*pSpvPath);
        for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
            if (TICKERNEL_SUBPASS_DESCRIPTOR_SET == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    if (pSpvReflectDescriptorBinding->binding < descriptorBindingCount)
                    {
                        // Skip, already counted
                    }
                    else
                    {
                        descriptorBindingCount = pSpvReflectDescriptorBinding->binding + 1;
                    }
                }
                descriptorBindings = tknMalloc(sizeof(DescriptorBinding) * descriptorBindingCount);
                vkDescriptorSetLayoutBindings = tknMalloc(sizeof(VkDescriptorSetLayoutBinding) * descriptorBindingCount);
                for (uint32_t binding = 0; binding < descriptorBindingCount; binding++)
                {
                    descriptorBindings[binding] = (DescriptorBinding){0};
                    vkDescriptorSetLayoutBindings[binding] = (VkDescriptorSetLayoutBinding){
                        .binding = binding,
                        .descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM,
                        .descriptorCount = 0,
                        .stageFlags = 0,
                        .pImmutableSamplers = NULL,
                    };
                }
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
                        descriptorBindings[binding] = getDefaultDescriptorBinding(vkDescriptorSetLayoutBinding.descriptorType);

                        if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorSetLayoutBinding.descriptorType)
                        {
                            VkImageLayout vkImageLayout;
                            uint32_t inputAttachmentReferenceIndex;
                            for (inputAttachmentReferenceIndex = 0; inputAttachmentReferenceIndex < inputVkAttachmentReferenceCount; inputAttachmentReferenceIndex++)
                            {
                                VkAttachmentReference vkAttachmentReference = inputVkAttachmentReferences[inputAttachmentReferenceIndex];
                                if (vkAttachmentReference.attachment == pSpvReflectDescriptorBinding->input_attachment_index)
                                {
                                    vkImageLayout = vkAttachmentReference.layout;
                                    break;
                                }
                            }
                            tknAssert(inputAttachmentReferenceIndex < inputVkAttachmentReferenceCount, "Input attachment reference not found for binding %d", pSpvReflectDescriptorBinding->input_attachment_index);
                            VkAttachmentReference vkAttachmentReference = inputVkAttachmentReferences[pSpvReflectDescriptorBinding->input_attachment_index];
                            Attachment *pAttachment = attachmentPtrs[pSpvReflectDescriptorBinding->input_attachment_index];
                            descriptorBindings[binding].inputAttachmentDescriptorBinding.pAttachment = pAttachment;
                        }

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
                printf("Warning: descriptor set %d in subpass shader module %s\n", spvReflectDescriptorSet.set, *pSpvPath);
            }
        }
        destroySpvReflectShaderModule(&spvReflectShaderModule);
    }
    VkDevice vkDevice = pGfxContext->vkDevice;
    VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = descriptorBindingCount,
        .pBindings = vkDescriptorSetLayoutBindings,
    };
    ASSERT_VK_SUCCESS(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout));
    VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = vkDescriptorPoolSizeDynamicArray.count,
        .pPoolSizes = vkDescriptorPoolSizeDynamicArray.array,
        .maxSets = 1,
    };
    ASSERT_VK_SUCCESS(vkCreateDescriptorPool(vkDevice, &vkDescriptorPoolCreateInfo, NULL, &vkDescriptorPool));
    VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vkDescriptorSetLayout,
    };
    ASSERT_VK_SUCCESS(vkAllocateDescriptorSets(vkDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet));

    vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetDynamicArray.count, vkWriteDescriptorSetDynamicArray.array, 0, NULL);

    Subpass subpass = {
        .descriptorBindingCount = descriptorBindingCount,
        .descriptorBindings = descriptorBindings,
        .vkDescriptorSetLayoutBindings = vkDescriptorSetLayoutBindings,
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPool = vkDescriptorPool,
        .vkDescriptorSet = vkDescriptorSet,
        .pipelinePtrDynamicArray = pipelinePtrDynamicArray,
    };

    tknDestroyDynamicArray(vkDescriptorPoolSizeDynamicArray);
    return subpass;
}
void destroySubpass(GfxContext *pGfxContext, Subpass *pSubpass)
{
    for (uint32_t j = 0; j < pSubpass->pipelinePtrDynamicArray.count; j++)
    {
        Pipeline *pPipeline = tknGetFromDynamicArray(&pSubpass->pipelinePtrDynamicArray, j);
        // destroyPipeline(pGfxContext, pPipeline);
    }
    VkDevice vkDevice = pGfxContext->vkDevice;
    ASSERT_VK_SUCCESS(vkFreeDescriptorSets(vkDevice, pSubpass->vkDescriptorPool, 1, &pSubpass->vkDescriptorSet));
    vkDestroyDescriptorPool(vkDevice, pSubpass->vkDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pSubpass->vkDescriptorSetLayout, NULL);
    tknDestroyDynamicArray(pSubpass->pipelinePtrDynamicArray);
    tknFree(pSubpass->descriptorBindings);
    tknFree(pSubpass->vkDescriptorSetLayoutBindings);
}

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex)
{
    RenderPass *pRenderPass = tknMalloc(sizeof(RenderPass));
    Attachment **attachmentPtrs = tknMalloc(sizeof(Attachment *) * attachmentCount);
    Subpass *subpasses = tknMalloc(sizeof(Subpass) * subpassCount);
    *pRenderPass = (RenderPass){
        .vkRenderPass = VK_NULL_HANDLE,
        .vkFramebuffers = NULL,
        .vkFramebufferCount = 0,
        .attachmentCount = attachmentCount,
        .attachmentPtrs = attachmentPtrs,
        .useSwapchain = false,
        .subpassCount = subpassCount,
        .subpasses = subpasses,
    };
    // Create vkRenderPass
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        Attachment *pAttachment = attachmentPtrs[i];
        pRenderPass->attachmentPtrs[i] = pAttachment;
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            vkAttachmentDescriptions[i].format = pGfxContext->surfaceFormat.format;
            pRenderPass->useSwapchain = true;
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            vkAttachmentDescriptions[i].format = pAttachment->attachmentContent.dynamicAttachmentContent.vkFormat;
        }
        else
        {
            vkAttachmentDescriptions[i].format = pAttachment->attachmentContent.fixedAttachmentContent.vkFormat;
        }
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
    ASSERT_VK_SUCCESS(vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pRenderPass->vkRenderPass));
    // Create framebuffers and subpasses
    populateFramebuffers(pGfxContext, pRenderPass);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        pRenderPass->subpasses[subpassIndex] = createSubpass(pGfxContext, vkSubpassDescriptions[subpassIndex].inputAttachmentCount, vkSubpassDescriptions[subpassIndex].pInputAttachments, spvPathDynamicArrays[subpassIndex], attachmentPtrs);
    }
    tknAddToDynamicArray(&pGfxContext->renderPassPtrDynamicArray, &pRenderPass, renderPassIndex);
    return pRenderPass;
}

void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkResult result;
    VkDevice vkDevice = pGfxContext->vkDevice;
    tknRemoveFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, pRenderPass);

    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        destroySubpass(pGfxContext, pSubpass);
    }

    tknFree(pRenderPass->subpasses);

    cleanupFramebuffers(pGfxContext, pRenderPass);

    vkDestroyRenderPass(pGfxContext->vkDevice, pRenderPass->vkRenderPass, NULL);
    tknFree(pRenderPass->attachmentPtrs);
    tknFree(pRenderPass);
}
