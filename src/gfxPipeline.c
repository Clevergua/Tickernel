#include "gfxPipeline.h"

static void createSpvReflectShaderModule(const char *filePath, SpvReflectShaderModule *pSpvReflectShaderModule)
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
}
static void destroySpvReflectShaderModule(SpvReflectShaderModule *pSpvReflectShaderModule)
{
    spvReflectDestroyShaderModule(pSpvReflectShaderModule);
}

void createFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
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
void destroyFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pRenderPass->vkFramebuffers[i], NULL);
    }
    tknFree(pRenderPass->vkFramebuffers);
}

void createSubpass(GfxContext *pGfxContext, uint32_t inputVkAttachmentReferenceCount, const VkAttachmentReference *inputVkAttachmentReferences, TknDynamicArray spvPathDynamicArray, Attachment **attachmentPtrs, Subpass *pSubpass)
{
    // for updating descriptor sets
    TknDynamicArray descriptorBindingDynamicArray;
    tknCreateDynamicArray(sizeof(DescriptorBinding), 1, &descriptorBindingDynamicArray);
    // for creating descriptor set
    VkDescriptorSetLayout vkDescriptorSetLayout;
    // for creating descriptor pool
    VkDescriptorPool vkDescriptorPool;
    // subpass descriptor set
    VkDescriptorSet vkDescriptorSet;
    // pipelines
    TknDynamicArray pipelinePtrDynamicArray;
    tknCreateDynamicArray(sizeof(Pipeline *), 1, &pipelinePtrDynamicArray);

    TknDynamicArray vkDescriptorPoolSizeDynamicArray;
    tknCreateDynamicArray(sizeof(VkDescriptorPoolSize), 1, &vkDescriptorPoolSizeDynamicArray);
    TknDynamicArray vkDescriptorSetLayoutBindingDynamicArray;
    tknCreateDynamicArray(sizeof(VkDescriptorSetLayoutBinding), 1, &vkDescriptorSetLayoutBindingDynamicArray);

    for (uint32_t pathIndex = 0; pathIndex < spvPathDynamicArray.count; pathIndex++)
    {
        const char **pSpvPath;
        tknGetFromDynamicArray(&spvPathDynamicArray, pathIndex, (void **)&pSpvPath);
        SpvReflectShaderModule spvReflectShaderModule;
        createSpvReflectShaderModule(*pSpvPath, &spvReflectShaderModule);
        for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
            if (TICKERNEL_SUBPASS_DESCRIPTOR_SET == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = {
                        .binding = pSpvReflectDescriptorBinding->binding,
                        .descriptorType = (VkDescriptorType)pSpvReflectDescriptorBinding->descriptor_type,
                        .descriptorCount = pSpvReflectDescriptorBinding->count,
                        .stageFlags = (VkShaderStageFlags)spvReflectShaderModule.shader_stage,
                        .pImmutableSamplers = NULL,
                    };
                    uint32_t addedIndex;
                    for (addedIndex = 0; addedIndex < vkDescriptorSetLayoutBindingDynamicArray.count; addedIndex++)
                    {
                        VkDescriptorSetLayoutBinding *pAddedBinding = NULL;
                        tknGetFromDynamicArray(&vkDescriptorSetLayoutBindingDynamicArray, addedIndex, (void **)&pAddedBinding);
                        if (pAddedBinding->binding == vkDescriptorSetLayoutBinding.binding)
                        {
                            tknAssert(pAddedBinding->descriptorType == vkDescriptorSetLayoutBinding.descriptorType, "Incompatible descriptor binding");
                            pAddedBinding->stageFlags |= vkDescriptorSetLayoutBinding.stageFlags;
                            pAddedBinding->descriptorCount = vkDescriptorSetLayoutBinding.descriptorCount > pAddedBinding->descriptorCount ? vkDescriptorSetLayoutBinding.descriptorCount : pAddedBinding->descriptorCount;
                            break;
                        }
                    }
                    if (addedIndex < vkDescriptorSetLayoutBindingDynamicArray.count)
                    {
                        // Binding already exists, skip adding
                        continue;
                    }
                    else
                    {
                        tknAddToDynamicArray(&vkDescriptorSetLayoutBindingDynamicArray, &vkDescriptorSetLayoutBinding, vkDescriptorSetLayoutBindingDynamicArray.count);
                        // Fill vkDescriptorPoolSizeDynamicArray
                        uint32_t poolSizeIndex;
                        for (poolSizeIndex = 0; poolSizeIndex < vkDescriptorPoolSizeDynamicArray.count; poolSizeIndex++)
                        {
                            VkDescriptorPoolSize *pVkDescriptorPoolSize = NULL;
                            tknGetFromDynamicArray(&vkDescriptorPoolSizeDynamicArray, poolSizeIndex, (void **)&pVkDescriptorPoolSize);
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

                        if (vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
                        {
                            VkImageLayout vkImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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
                            VkImageView vkImageView;
                            if (pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                            {
                                vkImageView = pAttachment->attachmentContent.dynamicAttachmentContent.image.vkImageView;
                            }
                            else if (pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                            {
                                vkImageView = pAttachment->attachmentContent.fixedAttachmentContent.image.vkImageView;
                            }
                            else
                            {
                                tknError("Unsupported attachment type: %d\n", pAttachment->attachmentType);
                            }

                            VkDescriptorImageInfo vkDescriptorImageInfo = {
                                .sampler = VK_NULL_HANDLE,
                                .imageView = vkImageView,
                                .imageLayout = vkImageLayout,
                            };
                            VkWriteDescriptorSet vkWriteDescriptorSet = {
                                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                .pNext = NULL,
                                .dstSet = VK_NULL_HANDLE,
                                .dstBinding = vkDescriptorSetLayoutBinding.binding,
                                .dstArrayElement = 0,
                                .descriptorCount = vkDescriptorSetLayoutBinding.descriptorCount,
                                .descriptorType = vkDescriptorSetLayoutBinding.descriptorType,
                                .pImageInfo = &vkDescriptorImageInfo,
                                .pBufferInfo = NULL,
                                .pTexelBufferView = NULL,
                            };
                            tknAddToDynamicArray(&vkWriteDescriptorSetDynamicArray, &vkWriteDescriptorSet, vkWriteDescriptorSetDynamicArray.count);
                            tknAddToDynamicArray(&inputAttachmentIndexDynamicArray, &pSpvReflectDescriptorBinding->input_attachment_index, inputAttachmentIndexDynamicArray.count);
                        }
                    }
                }
                // Skip other sets.
                break;
            }
            else
            {
                // Skip
                printf("Error descriptor set %d in subpass shader module %s\n", spvReflectDescriptorSet.set, *pSpvPath);
            }
        }
        destroySpvReflectShaderModule(&spvReflectShaderModule);
    }
    VkDevice vkDevice = pGfxContext->vkDevice;
    VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = vkDescriptorSetLayoutBindingDynamicArray.count,
        .pBindings = vkDescriptorSetLayoutBindingDynamicArray.array,
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
        .descriptorBindingDynamicArray = descriptorBindingDynamicArray,
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPool = vkDescriptorPool,
        .vkDescriptorSet = vkDescriptorSet,
        .pipelinePtrDynamicArray = pipelinePtrDynamicArray,
    };
    tknDestroyDynamicArray(vkDescriptorSetLayoutBindingDynamicArray);
    tknDestroyDynamicArray(vkDescriptorPoolSizeDynamicArray);
}
void destroySubpass(GfxContext *pGfxContext, Subpass *pSubpass)
{
    for (uint32_t j = 0; j < pSubpass->pipelinePtrDynamicArray.count; j++)
    {
        Pipeline *pPipeline;
        tknGetFromDynamicArray(&pSubpass->pipelinePtrDynamicArray, j, (void **)&pPipeline);
        // destroyPipeline(pGfxContext, pPipeline);
    }
    VkDevice vkDevice = pGfxContext->vkDevice;
    ASSERT_VK_SUCCESS(vkFreeDescriptorSets(vkDevice, pSubpass->vkDescriptorPool, 1, &pSubpass->vkDescriptorSet));
    vkDestroyDescriptorPool(vkDevice, pSubpass->vkDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pSubpass->vkDescriptorSetLayout, NULL);
    tknDestroyDynamicArray(pSubpass->inputAttachmentIndexDynamicArray);
    tknDestroyDynamicArray(pSubpass->vkWriteDescriptorSetDynamicArray);
    tknDestroyDynamicArray(pSubpass->pipelinePtrDynamicArray);
}

void createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass)
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
    createFramebuffers(pGfxContext, pRenderPass);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        createSubpass(pGfxContext, vkSubpassDescriptions[subpassIndex].inputAttachmentCount, vkSubpassDescriptions[subpassIndex].pInputAttachments, spvPathDynamicArrays[subpassIndex], attachmentPtrs, &pRenderPass->subpasses[subpassIndex]);
    }
    tknAddToDynamicArray(&pGfxContext->renderPassPtrDynamicArray, &pRenderPass, renderPassIndex);
    *ppRenderPass = pRenderPass;
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

    destroyFramebuffers(pGfxContext, pRenderPass);

    vkDestroyRenderPass(pGfxContext->vkDevice, pRenderPass->vkRenderPass, NULL);
    tknFree(pRenderPass->attachmentPtrs);
    tknFree(pRenderPass);
}
