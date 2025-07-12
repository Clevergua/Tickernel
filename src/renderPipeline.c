#include "renderPipeline.h"

static void getMemoryType(VkPhysicalDevice vkPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex)
{
    *memoryTypeIndex = UINT32_MAX;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &physicalDeviceMemoryProperties);
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
        {
            *memoryTypeIndex = i;
            return;
        }
    }
    tknError("Failed to get suitable memory type!");
}

static void createImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage *pVkImage, VkDeviceMemory *pVkDeviceMemory)
{
    VkImageCreateInfo imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = vkFormat,
        .extent = vkExtent3D,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = vkImageTiling,
        .usage = vkImageUsageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = 0,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    ASSERT_VK_SUCCESS(vkCreateImage(vkDevice, &imageCreateInfo, NULL, pVkImage));
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, *pVkImage, &memoryRequirements);
    uint32_t memoryTypeIndex;
    getMemoryType(vkPhysicalDevice, memoryRequirements.memoryTypeBits, vkMemoryPropertyFlags, &memoryTypeIndex);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    ASSERT_VK_SUCCESS(vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pVkDeviceMemory));
    ASSERT_VK_SUCCESS(vkBindImageMemory(vkDevice, *pVkImage, *pVkDeviceMemory, 0));
}
static void destroyImage(VkDevice vkDevice, VkImage vkImage, VkDeviceMemory vkDeviceMemory)
{
    vkDestroyImage(vkDevice, vkImage, NULL);
    vkFreeMemory(vkDevice, vkDeviceMemory, NULL);
}

static void createImageView(VkDevice vkDevice, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView)
{
    VkComponentMapping components = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };
    VkImageSubresourceRange subresourceRange = {
        .aspectMask = imageAspectFlags,
        .levelCount = 1,
        .baseMipLevel = 0,
        .layerCount = 1,
        .baseArrayLayer = 0,
    };
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = components,
        .subresourceRange = subresourceRange,
    };
    ASSERT_VK_SUCCESS(vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, pImageView));
}
static void destroyImageView(VkDevice vkDevice, VkImageView vkImageView)
{
    vkDestroyImageView(vkDevice, vkImageView, NULL);
}

void createDynamicAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment **ppAttachment)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    DynamicAttachmentContent dynamicAttachmentContent = {
        .vkImage = VK_NULL_HANDLE,
        .vkImageView = VK_NULL_HANDLE,
        .vkDeviceMemory = VK_NULL_HANDLE,
        .vkFormat = vkFormat,
        .scaler = scaler,
        .vkImageUsageFlags = vkImageUsageFlags,
        .vkMemoryPropertyFlags = vkMemoryPropertyFlags,
        .vkImageAspectFlags = vkImageAspectFlags,
    };
    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pGraphicsContext->swapchainExtent.width * scaler),
        .height = (uint32_t)(pGraphicsContext->swapchainExtent.height * scaler),
        .depth = 1,
    };
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    createImage(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, &pAttachment->attachmentContent.dynamicAttachmentContent.vkImage, &pAttachment->attachmentContent.dynamicAttachmentContent.vkDeviceMemory);
    createImageView(vkDevice, pAttachment->attachmentContent.dynamicAttachmentContent.vkImage, vkFormat, vkImageAspectFlags, &pAttachment->attachmentContent.dynamicAttachmentContent.vkImageView);
    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_DYNAMIC,
        .attachmentContent.dynamicAttachmentContent = dynamicAttachmentContent,
    };
    *ppAttachment = pAttachment;
}
void destroyDynamicAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment)
{
    DynamicAttachmentContent dynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
    destroyImageView(pGraphicsContext->vkDevice, dynamicAttachmentContent.vkImageView);
    destroyImage(pGraphicsContext->vkDevice, dynamicAttachmentContent.vkImage, dynamicAttachmentContent.vkDeviceMemory);
    tknFree(pAttachment);
}
void resizeDynamicAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment)
{
    DynamicAttachmentContent dynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
    destroyImageView(pGraphicsContext->vkDevice, dynamicAttachmentContent.vkImageView);
    destroyImage(pGraphicsContext->vkDevice, dynamicAttachmentContent.vkImage, dynamicAttachmentContent.vkDeviceMemory);

    // Create a new image and image view with the new scaler
    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pGraphicsContext->swapchainExtent.width * pAttachment->attachmentContent.dynamicAttachmentContent.scaler),
        .height = (uint32_t)(pGraphicsContext->swapchainExtent.height * pAttachment->attachmentContent.dynamicAttachmentContent.scaler),
        .depth = 1,
    };
    createImage(pGraphicsContext->vkDevice, pGraphicsContext->vkPhysicalDevice, vkExtent3D, dynamicAttachmentContent.vkFormat, VK_IMAGE_TILING_OPTIMAL, dynamicAttachmentContent.vkImageUsageFlags, dynamicAttachmentContent.vkMemoryPropertyFlags, &dynamicAttachmentContent.vkImage, &dynamicAttachmentContent.vkDeviceMemory);
    createImageView(pGraphicsContext->vkDevice, dynamicAttachmentContent.vkImage, dynamicAttachmentContent.vkFormat, dynamicAttachmentContent.vkImageAspectFlags, &dynamicAttachmentContent.vkImageView);
}

void createFixedAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment **ppAttachment)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    FixedAttachmentContent fixedAttachmentContent = {
        .vkImage = VK_NULL_HANDLE,
        .vkImageView = VK_NULL_HANDLE,
        .vkDeviceMemory = VK_NULL_HANDLE,
        .vkFormat = vkFormat,
        .width = width,
        .height = height,
    };
    VkExtent3D vkExtent3D = {
        .width = width,
        .height = height,
        .depth = 1,
    };
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    createImage(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, &pAttachment->attachmentContent.fixedAttachmentContent.vkImage, &pAttachment->attachmentContent.fixedAttachmentContent.vkDeviceMemory);
    createImageView(vkDevice, pAttachment->attachmentContent.fixedAttachmentContent.vkImage, vkFormat, vkImageAspectFlags, &pAttachment->attachmentContent.fixedAttachmentContent.vkImageView);
    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_FIXED,
        .attachmentContent.fixedAttachmentContent = fixedAttachmentContent,
    };
    *ppAttachment = pAttachment;
}
void destroyFixedAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    FixedAttachmentContent fixedAttachmentContent = pAttachment->attachmentContent.fixedAttachmentContent;
    destroyImage(vkDevice, fixedAttachmentContent.vkImage, fixedAttachmentContent.vkDeviceMemory);
    destroyImage(vkDevice, fixedAttachmentContent.vkImage, fixedAttachmentContent.vkDeviceMemory);
    tknFree(pAttachment);
}

void getSwapchainAttachments(GraphicsContext *pGraphicsContext, uint32_t *pSwapchainAttachmentCount, Attachment ***pAttachments)
{
    *pSwapchainAttachmentCount = pGraphicsContext->swapchainImageCount;
    *pAttachments = pGraphicsContext->swapchainAttachmentPtrs;
}

void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **pAttachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass)
{
    RenderPass *pRenderPass = tickernelMalloc(sizeof(RenderPass));
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    bool useSwapchain = false;
    pRenderPass->pAttachmentCount = attachmentCount;
    pRenderPass->pAttachments = tickernelMalloc(sizeof(Attachment *) * attachmentCount);
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        pRenderPass->pAttachments[i] = pAttachments[i];
        Attachment *pAttachment = pAttachments[i];
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            useSwapchain = true;
            vkAttachmentDescriptions[i].format = pGraphicsContext->surfaceFormat.format;
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
    VkResult result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pRenderPass->vkRenderPass);
    tryThrowVulkanError(result);

    if (useSwapchain)
    {
        pRenderPass->vkFramebufferCount = pGraphicsContext->swapchainImageCount;
        pRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer) * pRenderPass->vkFramebufferCount);
        for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
        {
            createFramebuffer(pGraphicsContext, pRenderPass->vkFramebufferCount, pAttachments, pRenderPass, &pRenderPass->vkFramebuffers[i]);
        }
    }
    else
    {
        pRenderPass->vkFramebufferCount = 1;
        pRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer));
        createFramebuffer(pGraphicsContext, pRenderPass->vkFramebufferCount, pAttachments, pRenderPass, &pRenderPass->vkFramebuffers[0]);
    }

    pRenderPass->subpassCount = subpassCount;
    pRenderPass->subpasses = tickernelMalloc(sizeof(Subpass) * pRenderPass->subpassCount);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        // pipelines
        TickernelDynamicArray pipelinePtrDynamicArray;
        tickernelCreateDynamicArray(&pipelinePtrDynamicArray, sizeof(Pipeline *), 4);
        // for recreate descriptor set
        uint32_t inputAttachmentReferenceCount = vkSubpassDescriptions[subpassIndex].inputAttachmentCount;
        // for recreate descriptor set
        VkAttachmentReference *inputAttachmentReferences = tickernelMalloc(sizeof(VkAttachmentReference) * vkSubpassDescriptions[subpassIndex].inputAttachmentCount);
        memcpy(inputAttachmentReferences, vkSubpassDescriptions[subpassIndex].pInputAttachments, sizeof(VkAttachmentReference) * vkSubpassDescriptions[subpassIndex].inputAttachmentCount);
        // for creating descriptor set
        VkDescriptorSetLayout vkDescriptorSetLayout;
        // for creating descriptor pool
        VkDescriptorPool vkDescriptorPool;
        // subpass descriptor set
        VkDescriptorSet vkDescriptorSet;
        TickernelDynamicArray pathDynamicArray = spvPathDynamicArrays[subpassIndex];

        TickernelDynamicArray vkDescriptorPoolSizeDynamicArray;
        tickernelCreateDynamicArray(&vkDescriptorPoolSizeDynamicArray, sizeof(VkDescriptorPoolSize), 4);
        TickernelDynamicArray vkDescriptorSetLayoutBindingDynamicArray;
        tickernelCreateDynamicArray(&vkDescriptorSetLayoutBindingDynamicArray, sizeof(VkDescriptorSetLayoutBinding), 4);
        TickernelDynamicArray vkWriteDescriptorSetDynamicArray;
        tickernelCreateDynamicArray(&vkWriteDescriptorSetDynamicArray, sizeof(VkWriteDescriptorSet), 1);
        for (uint32_t pathIndex = 0; pathIndex < pathDynamicArray.count; pathIndex++)
        {
            const char *path = TICKERNEL_GET_FROM_DYNAMIC_ARRAY(&pathDynamicArray, pathIndex, const char *);
            SpvReflectShaderModule spvReflectShaderModule;
            createSpvReflectShaderModule(path, &spvReflectShaderModule);
            for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
            {
                SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
                // TODO check other descriptor sets.
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
                            tickernelGetFromDynamicArray(&vkDescriptorSetLayoutBindingDynamicArray, addedIndex, (void **)&pAddedBinding);
                            if (pAddedBinding->binding == vkDescriptorSetLayoutBinding.binding)
                            {
                                tickernelAssert(pAddedBinding->descriptorType == vkDescriptorSetLayoutBinding.descriptorType, "Incompatible descriptor binding");
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
                            tickernelAddToDynamicArray(&vkDescriptorSetLayoutBindingDynamicArray, &vkDescriptorSetLayoutBinding, vkDescriptorSetLayoutBindingDynamicArray.count);
                            // Fill vkDescriptorPoolSizeDynamicArray
                            uint32_t poolSizeIndex;
                            for (poolSizeIndex = 0; poolSizeIndex < vkDescriptorPoolSizeDynamicArray.count; poolSizeIndex++)
                            {
                                VkDescriptorPoolSize *pVkDescriptorPoolSize = NULL;
                                tickernelGetFromDynamicArray(&vkDescriptorPoolSizeDynamicArray, poolSizeIndex, (void **)&pVkDescriptorPoolSize);
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
                                tickernelAddToDynamicArray(&vkDescriptorPoolSizeDynamicArray, &vkDescriptorPoolSize, vkDescriptorPoolSizeDynamicArray.count);
                            }

                            if (vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
                            {
                                VkAttachmentReference vkAttachmentReference = inputAttachmentReferences[pSpvReflectDescriptorBinding->input_attachment_index];
                                Attachment *pAttachment = pRenderPass->pAttachments[vkAttachmentReference.attachment];
                                VkImageView vkImageView;
                                if (pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
                                {
                                    vkImageView = pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage.vkImageView;
                                }
                                else if (pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                                {
                                    vkImageView = pAttachment->attachmentContent.fixedAttachmentContent.graphicsImage.vkImageView;
                                }
                                else
                                {
                                    tickernelError("Unsupported attachment type: %d\n", pAttachment->attachmentType);
                                }

                                VkDescriptorImageInfo vkDescriptorImageInfo = {
                                    .sampler = VK_NULL_HANDLE,
                                    .imageView = vkImageView,
                                    .imageLayout = vkAttachmentReference.layout,
                                };
                                VkWriteDescriptorSet vkWriteDescriptorSet = {
                                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                    .pNext = NULL,
                                    .dstSet = VK_NULL_HANDLE,
                                    .dstBinding = vkDescriptorSetLayoutBinding.binding,
                                    .dstArrayElement = 0,
                                    .descriptorCount = 1,
                                    .descriptorType = vkDescriptorSetLayoutBinding.descriptorType,
                                    .pImageInfo = &vkDescriptorImageInfo,
                                    .pBufferInfo = NULL,
                                    .pTexelBufferView = NULL,
                                };
                                tickernelAddToDynamicArray(&vkWriteDescriptorSetDynamicArray, &vkWriteDescriptorSet, vkWriteDescriptorSetDynamicArray.count);
                            }
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
            destroySpvReflectShaderModule(&spvReflectShaderModule);
        }
        VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = vkDescriptorSetLayoutBindingDynamicArray.count,
            .pBindings = vkDescriptorSetLayoutBindingDynamicArray.array,
        };
        result = vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout);
        tryThrowVulkanError(result);
        VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = vkDescriptorPoolSizeDynamicArray.count,
            .pPoolSizes = vkDescriptorPoolSizeDynamicArray.array,
            .maxSets = 1,
        };
        result = vkCreateDescriptorPool(vkDevice, &vkDescriptorPoolCreateInfo, NULL, &vkDescriptorPool);
        tryThrowVulkanError(result);

        VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = vkDescriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &vkDescriptorSetLayout,
        };
        result = vkAllocateDescriptorSets(vkDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet);
        tryThrowVulkanError(result);

        vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetDynamicArray.count, vkWriteDescriptorSetDynamicArray.array, 0, NULL);

        Subpass subpass = {
            .pipelinePtrDynamicArray = pipelinePtrDynamicArray,
            .inputAttachmentReferenceCount = inputAttachmentReferenceCount,
            .inputAttachmentReferences = inputAttachmentReferences,
            .vkDescriptorSetLayout = vkDescriptorSetLayout,
            .vkDescriptorPool = vkDescriptorPool,
            .vkDescriptorSet = vkDescriptorSet,
        };
        tickernelDestroyDynamicArray(vkDescriptorSetLayoutBindingDynamicArray);
        tickernelDestroyDynamicArray(vkDescriptorPoolSizeDynamicArray);
        pRenderPass->subpasses[subpassIndex] = subpass;
    }

    tickernelAddToDynamicArray(&pGraphicsContext->renderPassPtrDynamicArray, &pRenderPass, renderPassIndex);
    *ppRenderPass = pRenderPass;
}

void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass)
{
}
