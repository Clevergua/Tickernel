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

void createFramebuffers(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
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
                width = pGraphicsContext->swapchainExtent.width;
                height = pGraphicsContext->swapchainExtent.height;
            }
            else
            {
                tknAssert(width == pGraphicsContext->swapchainExtent.width && height == pGraphicsContext->swapchainExtent.height, "Swapchain attachment size mismatch!");
            }
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            DynamicAttachmentContent dynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
            if (width == UINT32_MAX && height == UINT32_MAX)
            {
                width = pGraphicsContext->swapchainExtent.width * dynamicAttachmentContent.scaler;
                height = pGraphicsContext->swapchainExtent.height * dynamicAttachmentContent.scaler;
            }
            else
            {
                tknAssert(width == pGraphicsContext->swapchainExtent.width * dynamicAttachmentContent.scaler && height == pGraphicsContext->swapchainExtent.height * dynamicAttachmentContent.scaler, "Dynamic attachment size mismatch!");
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
        pRenderPass->vkFramebufferCount = pGraphicsContext->swapchainImageCount;
        pRenderPass->vkFramebuffers = tknMalloc(sizeof(VkFramebuffer) * pGraphicsContext->swapchainImageCount);
        VkImageView *attachmentVkImageViews = tknMalloc(sizeof(VkImageView) * attachmentCount);
        for (uint32_t swapchainIndex = 0; swapchainIndex < pGraphicsContext->swapchainImageCount; swapchainIndex++)
        {
            for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
            {
                Attachment *pAttachment = attachmentPtrs[attachmentIndex];
                if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
                {
                    attachmentVkImageViews[attachmentIndex] = pGraphicsContext->swapchainImageViews[swapchainIndex];
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
        ASSERT_VK_SUCCESS(vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pRenderPass->vkFramebuffers[0]));
        tknFree(attachmentVkImageViews);
    }
}
void destroyFramebuffers(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pRenderPass->vkFramebuffers[i], NULL);
    }
    tknFree(pRenderPass->vkFramebuffers);
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

void getSwapchainAttachments(GraphicsContext *pGraphicsContext, Attachment **pAttachments)
{
    *pAttachments = pGraphicsContext->swapchainAttachmentPtr;
}

void createSubpass(GraphicsContext *pGraphicsContext, uint32_t inputVkAttachmentReferenceCount, const VkAttachmentReference *inputVkAttachmentReferences, TknDynamicArray spvPathDynamicArray, Attachment **attachmentPtrs, Subpass *pSubpass)
{
    // pipelines
    TknDynamicArray pipelinePtrDynamicArray;
    tknCreateDynamicArray(sizeof(Pipeline *), 1, &pipelinePtrDynamicArray);
    // for creating descriptor set
    VkDescriptorSetLayout vkDescriptorSetLayout;
    // for creating descriptor pool
    VkDescriptorPool vkDescriptorPool;
    // subpass descriptor set
    VkDescriptorSet vkDescriptorSet;
    // for rewriting descriptor sets
    TknDynamicArray vkWriteDescriptorSetDynamicArray;
    tknCreateDynamicArray(sizeof(VkWriteDescriptorSet), 1, &vkWriteDescriptorSetDynamicArray);
    // for rewriting descriptor sets
    TknDynamicArray inputAttachmentIndexDynamicArray;
    tknCreateDynamicArray(sizeof(uint32_t), 1, &inputAttachmentIndexDynamicArray);

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
                                vkImageView = pAttachment->attachmentContent.dynamicAttachmentContent.vkImageView;
                            }
                            else if (pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED)
                            {
                                vkImageView = pAttachment->attachmentContent.fixedAttachmentContent.vkImageView;
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
                                .descriptorCount = 1,
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
    VkDevice vkDevice = pGraphicsContext->vkDevice;
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
        .pipelinePtrDynamicArray = pipelinePtrDynamicArray,
        .vkWriteDescriptorSetDynamicArray = vkWriteDescriptorSetDynamicArray,
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPool = vkDescriptorPool,
        .vkDescriptorSet = vkDescriptorSet,
    };
    tknDestroyDynamicArray(vkDescriptorSetLayoutBindingDynamicArray);
    tknDestroyDynamicArray(vkDescriptorPoolSizeDynamicArray);
}
void destroySubpass(GraphicsContext *pGraphicsContext, Subpass *pSubpass)
{
    for (uint32_t j = 0; j < pSubpass->pipelinePtrDynamicArray.count; j++)
    {
        Pipeline *pPipeline;
        tknGetFromDynamicArray(&pSubpass->pipelinePtrDynamicArray, j, (void **)&pPipeline);
        // destroyPipeline(pGraphicsContext, pPipeline);
    }
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    ASSERT_VK_SUCCESS(vkFreeDescriptorSets(vkDevice, pSubpass->vkDescriptorPool, 1, &pSubpass->vkDescriptorSet));
    vkDestroyDescriptorPool(vkDevice, pSubpass->vkDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pSubpass->vkDescriptorSetLayout, NULL);
    tknDestroyDynamicArray(pSubpass->inputAttachmentIndexDynamicArray);
    tknDestroyDynamicArray(pSubpass->vkWriteDescriptorSetDynamicArray);
    tknDestroyDynamicArray(pSubpass->pipelinePtrDynamicArray);
}

void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass)
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
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        Attachment *pAttachment = attachmentPtrs[i];
        pRenderPass->attachmentPtrs[i] = pAttachment;
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            vkAttachmentDescriptions[i].format = pGraphicsContext->surfaceFormat.format;
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
    createFramebuffers(pGraphicsContext, pRenderPass);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        createSubpass(pGraphicsContext, vkSubpassDescriptions[subpassIndex].inputAttachmentCount, vkSubpassDescriptions[subpassIndex].pInputAttachments, spvPathDynamicArrays[subpassIndex], attachmentPtrs, &pRenderPass->subpasses[subpassIndex]);
    }
    tknAddToDynamicArray(&pGraphicsContext->renderPassPtrDynamicArray, &pRenderPass, renderPassIndex);
    *ppRenderPass = pRenderPass;
}

void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass)
{
    VkResult result;
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    tknRemoveFromDynamicArray(&pGraphicsContext->renderPassPtrDynamicArray, pRenderPass);

    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        destroySubpass(pGraphicsContext, pSubpass);
    }

    tknFree(pRenderPass->subpasses);

    destroyFramebuffers(pGraphicsContext, pRenderPass);

    vkDestroyRenderPass(pGraphicsContext->vkDevice, pRenderPass->vkRenderPass, NULL);
    tknFree(pRenderPass->attachmentPtrs);
    tknFree(pRenderPass);
}
