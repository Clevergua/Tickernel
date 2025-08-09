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
        SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[TICKERNEL_SUBPASS_DESCRIPTOR_SET];
        if (TICKERNEL_SUBPASS_DESCRIPTOR_SET == spvReflectDescriptorSet.set)
        {
            for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
            {
                SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                uint32_t binding = pSpvReflectDescriptorBinding->binding;
                if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == (VkDescriptorType)pSpvReflectDescriptorBinding->descriptor_type)
                {
                    Descriptor descriptor = {
                        .vkDescriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                        .descriptorContent.inputAttachmentDescriptorContent.pAttachment = attachmentPtrs[pSpvReflectDescriptorBinding->input_attachment_index],
                        .pDescriptorSet = pSubpassDescriptorSet,
                        .binding = binding,
                    };
                    tknAddToDynamicArray(&inputAttachmentDescriptorDynamicArray, &descriptor, inputAttachmentDescriptorDynamicArray.count);
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
    bool useSwapchain = false;
    VkRenderPass vkRenderPass = VK_NULL_HANDLE;
    // Create vkRenderPass
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        Attachment *pAttachment = inputAttachmentPtrs[i];
        attachmentPtrs[i] = pAttachment;
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            vkAttachmentDescriptions[i].format = pGfxContext->surfaceFormat.format;
            useSwapchain = true;
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            vkAttachmentDescriptions[i].format = pAttachment->attachmentContent.dynamicAttachmentContent.pImage->vkFormat;
        }
        else
        {
            vkAttachmentDescriptions[i].format = pAttachment->attachmentContent.fixedAttachmentContent.pImage->vkFormat;
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
    assertVkResult(vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &vkRenderPass));
    *pRenderPass = (RenderPass){
        .vkRenderPass = vkRenderPass,
        .vkFramebuffers = NULL,
        .vkFramebufferCount = 0,
        .attachmentCount = attachmentCount,
        .attachmentPtrs = attachmentPtrs,
        .useSwapchain = useSwapchain,
        .subpassCount = subpassCount,
        .subpasses = subpasses,
    };
    // Create framebuffers and subpasses
    populateFramebuffers(pGfxContext, pRenderPass);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        pRenderPass->subpasses[subpassIndex] = createSubpass(pGfxContext, vkSubpassDescriptions[subpassIndex].inputAttachmentCount, vkSubpassDescriptions[subpassIndex].pInputAttachments, spvPathCounts[subpassIndex], spvPathsArray[subpassIndex], attachmentPtrs);
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
