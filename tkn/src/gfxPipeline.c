#include "gfxPipeline.h"
#include "gfxCommon.h"

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
        descriptorBinding.samplerDescriptorBinding.activeSamplerPtr = NULL;
        descriptorBinding.samplerDescriptorBinding.pendingSamplerPtr = NULL;
    }
    // TODO other types
    else
    {
        tknError("Unsupported descriptor type: %d", descriptorType);
    }
    return descriptorBinding;
}

static DescriptorSet createDescriptorSet(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set)
{
    uint32_t descriptorBindingCount = 0;
    DescriptorBinding *descriptorBindings = NULL;
    VkDescriptorSetLayoutBinding *vkDescriptorSetLayoutBindings = VK_NULL_HANDLE;
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
                    if (pSpvReflectDescriptorBinding->binding < descriptorBindingCount)
                    {
                        // Skip, already counted
                    }
                    else
                    {
                        descriptorBindingCount = pSpvReflectDescriptorBinding->binding + 1;
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
                        descriptorBindings[binding] = getDefaultDescriptorBinding(vkDescriptorSetLayoutBinding.descriptorType);

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
        .bindingCount = descriptorBindingCount,
        .pBindings = vkDescriptorSetLayoutBindings,
    };
    assertVkResult(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout));
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

    DescriptorSet subpassDescriptorSet = {
        .descriptorBindingCount = descriptorBindingCount,
        .descriptorBindings = descriptorBindings,
        .vkDescriptorSetLayoutBindings = vkDescriptorSetLayoutBindings,
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPool = vkDescriptorPool,
        .vkDescriptorSet = vkDescriptorSet,
    };
    return subpassDescriptorSet;
}
static void destroyDescriptorSet(GfxContext *pGfxContext, DescriptorSet descriptorSet)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t binding = 0; binding < descriptorSet.descriptorBindingCount; binding++)
    {
        DescriptorBinding descriptorBinding = descriptorSet.descriptorBindings[binding];
        VkDescriptorType descriptorType = descriptorSet.vkDescriptorSetLayoutBindings[binding].descriptorType;
        if (VK_DESCRIPTOR_TYPE_SAMPLER == descriptorType)
        {
            // bindSampler(pGfxContext, &descriptorSet, binding, NULL);
        }
        // TODO other types
        // else if (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == descriptorType)
        // {
        //     bindCombinedImageSampler(pGfxContext, &descriptorSet, binding, NULL, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE == descriptorType)
        // {
        //     bindSampledImage(pGfxContext, &descriptorSet, binding, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_STORAGE_IMAGE == descriptorType)
        // {
        //     bindStorageImage(pGfxContext, &descriptorSet, binding, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER == descriptorType)
        // {
        //     bindUniformTexelBuffer(pGfxContext, &descriptorSet, binding, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER == descriptorType)
        // {
        //     bindStorageTexelBuffer(pGfxContext, &descriptorSet, binding, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER == descriptorType)
        // {
        //     bindUniformBuffer(pGfxContext, &descriptorSet, binding, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER == descriptorType)
        // {
        //     bindStorageBuffer(pGfxContext, &descriptorSet, binding, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC == descriptorType)
        // {
        //     bindUniformBufferDynamic(pGfxContext, &descriptorSet, binding, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC == descriptorType)
        // {
        //     bindStorageBufferDynamic(pGfxContext, &descriptorSet, binding, NULL);
        // }
        // else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == descriptorType)
        // {
        //     bindInputAttachment(pGfxContext, &descriptorSet, binding, NULL);
        // }
        else
        {
            tknError("Unsupported descriptor type: %d", descriptorType);
        }
    }
    vkFreeDescriptorSets(vkDevice, descriptorSet.vkDescriptorPool, 1, &descriptorSet.vkDescriptorSet);
    vkDestroyDescriptorPool(vkDevice, descriptorSet.vkDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, descriptorSet.vkDescriptorSetLayout, NULL);
    tknFree(descriptorSet.vkDescriptorSetLayoutBindings);
    tknFree(descriptorSet.descriptorBindings);
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

Subpass createSubpass(GfxContext *pGfxContext, uint32_t inputVkAttachmentReferenceCount, const VkAttachmentReference *inputVkAttachmentReferences, uint32_t spvPathCount, const char **spvPaths, Attachment **attachmentPtrs)
{
    TknDynamicArray pipelinePtrDynamicArray = tknCreateDynamicArray(sizeof(Pipeline *), 1);
    SpvReflectShaderModule *spvReflectShaderModules = tknMalloc(sizeof(SpvReflectShaderModule) * spvPathCount);
    for (uint32_t pathIndex = 0; pathIndex < spvPathCount; pathIndex++)
    {
        const char *spvPath = spvPaths[pathIndex];
        spvReflectShaderModules[pathIndex] = createSpvReflectShaderModule(spvPath);
    }
    DescriptorSet subpassDescriptorSet = createDescriptorSet(pGfxContext, spvPathCount, spvReflectShaderModules, TICKERNEL_SUBPASS_DESCRIPTOR_SET);
    for (uint32_t pathIndex = 0; pathIndex < spvPathCount; pathIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[pathIndex];
        SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[TICKERNEL_SUBPASS_DESCRIPTOR_SET];
        if (TICKERNEL_SUBPASS_DESCRIPTOR_SET == spvReflectDescriptorSet.set)
        {
            for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
            {
                SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                uint32_t binding = pSpvReflectDescriptorBinding->binding;
                if (pSpvReflectDescriptorBinding->descriptor_type == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
                {
                    // TODO Update DescriptorSet
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
        destroySpvReflectShaderModule(&spvReflectShaderModule);
    }
    tknFree(spvReflectShaderModules);

    Subpass subpass = {
        .subpassDescriptorSet = subpassDescriptorSet,
        .pipelinePtrDynamicArray = pipelinePtrDynamicArray,
    };
    return subpass;
}
void destroySubpass(GfxContext *pGfxContext, Subpass subpass)
{
    for (uint32_t j = 0; j < subpass.pipelinePtrDynamicArray.count; j++)
    {
        Pipeline *pPipeline = tknGetFromDynamicArray(&subpass.pipelinePtrDynamicArray, j);
        // destroyPipeline(pGfxContext, pPipeline);
    }
    VkDevice vkDevice = pGfxContext->vkDevice;
    destroyDescriptorSet(pGfxContext, subpass.subpassDescriptorSet);
    tknDestroyDynamicArray(subpass.pipelinePtrDynamicArray);
    // assertVkResult(vkFreeDescriptorSets(vkDevice, pSubpass->vkDescriptorPool, 1, &pSubpass->vkDescriptorSet));
    // vkDestroyDescriptorPool(vkDevice, pSubpass->vkDescriptorPool, NULL);
    // vkDestroyDescriptorSetLayout(vkDevice, pSubpass->vkDescriptorSetLayout, NULL);
    // tknDestroyDynamicArray(pSubpass->pipelinePtrDynamicArray);
    // tknFree(pSubpass->descriptorBindings);
    // tknFree(pSubpass->vkDescriptorSetLayoutBindings);
}

RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t spvPathCount, const char **spvPaths, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex)
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
            vkAttachmentDescriptions[i].format = pAttachment->attachmentContent.dynamicAttachmentContent.image.vkFormat;
        }
        else
        {
            vkAttachmentDescriptions[i].format = pAttachment->attachmentContent.fixedAttachmentContent.image.vkFormat;
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
    assertVkResult(vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pRenderPass->vkRenderPass));
    // Create framebuffers and subpasses
    populateFramebuffers(pGfxContext, pRenderPass);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        pRenderPass->subpasses[subpassIndex] = createSubpass(pGfxContext, vkSubpassDescriptions[subpassIndex].inputAttachmentCount, vkSubpassDescriptions[subpassIndex].pInputAttachments, spvPathCount, spvPaths, attachmentPtrs);
    }
    tknAddToDynamicArray(&pGfxContext->renderPassPtrDynamicArray, &pRenderPass, renderPassIndex);
    return pRenderPass;
}
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    tknRemoveFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, pRenderPass);

    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        destroySubpass(pGfxContext, *pSubpass);
    }

    tknFree(pRenderPass->subpasses);

    cleanupFramebuffers(pGfxContext, pRenderPass);

    vkDestroyRenderPass(pGfxContext->vkDevice, pRenderPass->vkRenderPass, NULL);
    tknFree(pRenderPass->attachmentPtrs);
    tknFree(pRenderPass);
}

// void bindSampler(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Sampler *pSampler)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_SAMPLER");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->samplerDescriptorBinding.pSampler == pSampler)
//     {
//         printf("Warning: binding sampler %p to descriptor set %p, binding %d, but it is already bound.\n", pSampler, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->samplerDescriptorBinding.pSampler != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->samplerDescriptorBinding.pSampler->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->samplerDescriptorBinding.pSampler = pSampler;
//         if (pSampler != NULL)
//         {
//             tknAddToHashSet(&pSampler->descriptorBindingPtrHashSet, pDescriptorBinding);
//             VkWriteDescriptorSet vkWriteDescriptorSet = {
//                 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//                 .pNext = NULL,
//                 .dstSet = pDescriptorSet->vkDescriptorSet,
//                 .dstBinding = binding,
//                 .dstArrayElement = 0,
//                 .descriptorCount = 1,
//                 .descriptorType = vkDescriptorSetLayoutBinding.descriptorType,
//                 .pImageInfo = NULL,
//                 .pBufferInfo = NULL,
//                 .pTexelBufferView = NULL,
//             };
//             VkDescriptorImageInfo vkDescriptorImageInfo = {
//                 .sampler = pSampler->vkSampler,
//                 .imageView = VK_NULL_HANDLE,
//                 .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
//             };
//             vkUpdateDescriptorSets(pGfxContext->vkDevice, 1, &vkWriteDescriptorSet, 0, NULL);
//         }
//     }
// }

// void bindCombinedImageSampler(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Sampler *pSampler, Image *pImage)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->combinedImageSamplerDescriptorBinding.pSampler == pSampler &&
//         pDescriptorBinding->combinedImageSamplerDescriptorBinding.pImage == pImage)
//     {
//         printf("Warning: binding combined image sampler %p/%p to descriptor set %p, binding %d, but it is already bound.\n", pSampler, pImage, pDescriptorSet, binding);
//     }
//     else
//     {
//         // Remove old bindings from hash sets
//         if (pDescriptorBinding->combinedImageSamplerDescriptorBinding.pSampler != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->combinedImageSamplerDescriptorBinding.pSampler->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         if (pDescriptorBinding->combinedImageSamplerDescriptorBinding.pImage != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->combinedImageSamplerDescriptorBinding.pImage->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }

//         // Set new bindings
//         pDescriptorBinding->combinedImageSamplerDescriptorBinding.pSampler = pSampler;
//         pDescriptorBinding->combinedImageSamplerDescriptorBinding.pImage = pImage;

//         // Add new bindings to hash sets
//         if (pSampler != NULL)
//         {
//             tknAddToHashSet(&pSampler->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         if (pImage != NULL)
//         {
//             tknAddToHashSet(&pImage->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindSampledImage(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Image *pImage)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->sampledImageDescriptorBinding.pImage == pImage)
//     {
//         printf("Warning: binding sampled image %p to descriptor set %p, binding %d, but it is already bound.\n", pImage, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->sampledImageDescriptorBinding.pImage != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->sampledImageDescriptorBinding.pImage->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->sampledImageDescriptorBinding.pImage = pImage;
//         if (pImage != NULL)
//         {
//             tknAddToHashSet(&pImage->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindStorageImage(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Image *pImage)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_STORAGE_IMAGE");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->storageImageDescriptorBinding.pImage == pImage)
//     {
//         printf("Warning: binding storage image %p to descriptor set %p, binding %d, but it is already bound.\n", pImage, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->storageImageDescriptorBinding.pImage != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->storageImageDescriptorBinding.pImage->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->storageImageDescriptorBinding.pImage = pImage;
//         if (pImage != NULL)
//         {
//             tknAddToHashSet(&pImage->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindUniformTexelBuffer(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->uniformTexelBufferDescriptorBinding.pBuffer == pBuffer)
//     {
//         printf("Warning: binding uniform texel buffer %p to descriptor set %p, binding %d, but it is already bound.\n", pBuffer, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->uniformTexelBufferDescriptorBinding.pBuffer != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->uniformTexelBufferDescriptorBinding.pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->uniformTexelBufferDescriptorBinding.pBuffer = pBuffer;
//         if (pBuffer != NULL)
//         {
//             tknAddToHashSet(&pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindStorageTexelBuffer(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->storageTexelBufferDescriptorBinding.pBuffer == pBuffer)
//     {
//         printf("Warning: binding storage texel buffer %p to descriptor set %p, binding %d, but it is already bound.\n", pBuffer, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->storageTexelBufferDescriptorBinding.pBuffer != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->storageTexelBufferDescriptorBinding.pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->storageTexelBufferDescriptorBinding.pBuffer = pBuffer;
//         if (pBuffer != NULL)
//         {
//             tknAddToHashSet(&pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindUniformBuffer(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->uniformBufferDescriptorBinding.pBuffer == pBuffer)
//     {
//         printf("Warning: binding uniform buffer %p to descriptor set %p, binding %d, but it is already bound.\n", pBuffer, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->uniformBufferDescriptorBinding.pBuffer != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->uniformBufferDescriptorBinding.pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->uniformBufferDescriptorBinding.pBuffer = pBuffer;
//         if (pBuffer != NULL)
//         {
//             tknAddToHashSet(&pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindStorageBuffer(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_STORAGE_BUFFER");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->storageBufferDescriptorBinding.pBuffer == pBuffer)
//     {
//         printf("Warning: binding storage buffer %p to descriptor set %p, binding %d, but it is already bound.\n", pBuffer, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->storageBufferDescriptorBinding.pBuffer != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->storageBufferDescriptorBinding.pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->storageBufferDescriptorBinding.pBuffer = pBuffer;
//         if (pBuffer != NULL)
//         {
//             tknAddToHashSet(&pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindUniformBufferDynamic(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->uniformBufferDynamicDescriptorBinding.pBuffer == pBuffer)
//     {
//         printf("Warning: binding uniform buffer dynamic %p to descriptor set %p, binding %d, but it is already bound.\n", pBuffer, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->uniformBufferDynamicDescriptorBinding.pBuffer != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->uniformBufferDynamicDescriptorBinding.pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->uniformBufferDynamicDescriptorBinding.pBuffer = pBuffer;
//         if (pBuffer != NULL)
//         {
//             tknAddToHashSet(&pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindStorageBufferDynamic(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Buffer *pBuffer)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->storageBufferDynamicDescriptorBinding.pBuffer == pBuffer)
//     {
//         printf("Warning: binding storage buffer dynamic %p to descriptor set %p, binding %d, but it is already bound.\n", pBuffer, pDescriptorSet, binding);
//     }
//     else
//     {
//         if (pDescriptorBinding->storageBufferDynamicDescriptorBinding.pBuffer != NULL)
//         {
//             tknRemoveFromHashSet(&pDescriptorBinding->storageBufferDynamicDescriptorBinding.pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//         pDescriptorBinding->storageBufferDynamicDescriptorBinding.pBuffer = pBuffer;
//         if (pBuffer != NULL)
//         {
//             tknAddToHashSet(&pBuffer->descriptorBindingPtrHashSet, pDescriptorBinding);
//         }
//     }
// }

// void bindInputAttachment(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding)
// {
//     VkDevice vkDevice = pGfxContext->vkDevice;
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     DescriptorBinding descriptorBinding = pDescriptorSet->descriptorBindings[binding];
//     if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorSetLayoutBinding.descriptorType)
//     {
//         Attachment *pAttachment = descriptorBinding.inputAttachmentDescriptorBinding.pAttachment;
//         VkImageView vkImageView;
//         if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
//         {
//             vkImageView = pAttachment->attachmentContent.dynamicAttachmentContent.image.vkImageView;
//         }
//         else if (ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType)
//         {
//             vkImageView = pAttachment->attachmentContent.fixedAttachmentContent.image.vkImageView;
//         }
//         else
//         {
//             // ATTACHMENT_TYPE_SWAPCHAIN
//             tknError("Input attachment cannot be a swapchain attachment");
//         }
//         VkDescriptorImageInfo vkDescriptorImageInfo = {
//             .sampler = VK_NULL_HANDLE,
//             .imageView = vkImageView,
//             .imageLayout = descriptorBinding.inputAttachmentDescriptorBinding.vkImageLayout,
//         };
//         VkWriteDescriptorSet vkWriteDescriptorSet = {
//             .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//             .pNext = NULL,
//             .dstSet = pDescriptorSet->vkDescriptorSet,
//             .dstBinding = vkDescriptorSetLayoutBinding.binding,
//             .dstArrayElement = 0,
//             .descriptorCount = vkDescriptorSetLayoutBinding.descriptorCount,
//             .descriptorType = vkDescriptorSetLayoutBinding.descriptorType,
//             .pImageInfo = &vkDescriptorImageInfo,
//             .pBufferInfo = NULL,
//             .pTexelBufferView = NULL,
//         };
//         vkUpdateDescriptorSets(vkDevice, 1, &vkWriteDescriptorSet, 0, NULL);
//     }
// }
// void bindInputAttachment(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet, uint32_t binding, Attachment *pAttachment)
// {
//     tknAssert(binding < pDescriptorSet->descriptorBindingCount, "Binding index out of range");
//     VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = pDescriptorSet->vkDescriptorSetLayoutBindings[binding];
//     tknAssert(vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, "Binding type mismatch, expected VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT");
//     DescriptorBinding *pDescriptorBinding = &pDescriptorSet->descriptorBindings[binding];
//     if (pDescriptorBinding->inputAttachmentDescriptorBinding.pAttachment == pAttachment)
//     {
//         printf("Warning: binding input attachment %p to descriptor set %p, binding %d, but it is already bound.\n", pAttachment, pDescriptorSet, binding);
//     }
//     else
//     {
//         // Note: Input attachments don't have hash sets like other resources, so we just update the binding
//         Attachment *pBoundAttachment = pDescriptorBinding->inputAttachmentDescriptorBinding.pAttachment;
//         if (pBoundAttachment != NULL)
//         {
//             if (pBoundAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
//             {
//                 tknRemoveFromHashSet(&pBoundAttachment->attachmentContent.dynamicAttachmentContent.image.descriptorBindingPtrHashSet, pDescriptorBinding);
//             }
//             else if (pBoundAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
//             {
//                 tknRemoveFromHashSet(&pBoundAttachment->attachmentContent.fixedAttachmentContent.image.descriptorBindingPtrHashSet, pDescriptorBinding);
//             }
//             else
//             {
//                 tknError("Input attachment cannot be a swapchain attachment");
//             }
//         }
//         else
//         {
//             // nothing
//         }

//         if (pAttachment != NULL)
//         {
//             if (pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
//             {
//                 tknAddToHashSet(&pAttachment->attachmentContent.dynamicAttachmentContent.image.descriptorBindingPtrHashSet, pDescriptorBinding);
//             }
//             else if (pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
//             {
//                 tknAddToHashSet(&pAttachment->attachmentContent.fixedAttachmentContent.image.descriptorBindingPtrHashSet, pDescriptorBinding);
//             }
//             else
//             {
//                 tknError("Input attachment cannot be a swapchain attachment");
//             }
//             VkDescriptorImageInfo imageInfo = {
//                 .sampler = VK_NULL_HANDLE,
//                 .imageView = pAttachment->attachmentContent.fixedAttachmentContent.image.vkImageView,
//                 .imageLayout = pAttachment->attachmentContent.fixedAttachmentContent.image.vkImageLayout,
//             };
//             VkWriteDescriptorSet vkWriteDescriptorSet = {
//                 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//                 .pNext = NULL,
//                 .dstSet = pDescriptorSet->vkDescriptorSet,
//                 .dstBinding = binding,
//                 .dstArrayElement = 0,
//                 .descriptorCount = vkDescriptorSetLayoutBinding.descriptorCount,
//                 .descriptorType = vkDescriptorSetLayoutBinding.descriptorType,
//                 .pImageInfo = &imageInfo,
//                 .pBufferInfo = NULL,
//                 .pTexelBufferView = NULL,
//             };
//             vkUpdateDescriptorSets(pGfxContext->vkDevice, 1, &vkWriteDescriptorSet, 0, NULL);
//         }
//         else
//         {
//             // nothing
//         }
//         pDescriptorBinding->inputAttachmentDescriptorBinding.pAttachment = pAttachment;
//     }
// }
