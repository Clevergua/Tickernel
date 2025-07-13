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

static void createFramebuffer(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, Attachment **attachmentPtrs, RenderPass *pRenderPass, VkFramebuffer *pVkFramebuffer)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkImageView *attachmentVkImageViews = tknMalloc(sizeof(VkImageView) * attachmentCount);
    uint32_t width = 0;
    uint32_t height = 0;
    for (uint32_t j = 0; j < attachmentCount; j++)
    {
        Attachment *pAttachment = attachmentPtrs[j];
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            attachmentVkImageViews[j] = pGraphicsContext->swapchainImageViews[j];
            width = pGraphicsContext->swapchainExtent.width;
            height = pGraphicsContext->swapchainExtent.height;
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            DynamicAttachmentContent DynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
            attachmentVkImageViews[j] = DynamicAttachmentContent.vkImageView;
            width = pGraphicsContext->swapchainExtent.width * DynamicAttachmentContent.scaler;
            height = pGraphicsContext->swapchainExtent.height * DynamicAttachmentContent.scaler;
        }
        else
        {
            FixedAttachmentContent FixedAttachmentContent = pAttachment->attachmentContent.fixedAttachmentContent;
            attachmentVkImageViews[j] = FixedAttachmentContent.vkImageView;
            width = FixedAttachmentContent.width;
            height = FixedAttachmentContent.height;
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
    ASSERT_VK_SUCCESS(vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, pVkFramebuffer));
    tknFree(attachmentVkImageViews);
}
static void destroyFramebuffer(GraphicsContext *pGraphicsContext, VkFramebuffer vkFramebuffer)
{
    vkDestroyFramebuffer(pGraphicsContext->vkDevice, vkFramebuffer, NULL);
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

void createSubpass(GraphicsContext *pGraphicsContext, VkSubpassDescription vkSubpassDescription, TknDynamicArray spvPathDynamicArray, Attachment **attachmentPtrs, Subpass *pSubpass)
{
    // pipelines
    TknDynamicArray pipelinePtrDynamicArray;
    tknCreateDynamicArray(sizeof(Pipeline *), 1, &pipelinePtrDynamicArray);
    // for recreate descriptor set
    uint32_t inputAttachmentReferenceCount = vkSubpassDescription.inputAttachmentCount;
    // for recreate descriptor set
    VkAttachmentReference *inputAttachmentReferences = tknMalloc(sizeof(VkAttachmentReference) * vkSubpassDescription.inputAttachmentCount);
    memcpy(inputAttachmentReferences, vkSubpassDescription.pInputAttachments, sizeof(VkAttachmentReference) * vkSubpassDescription.inputAttachmentCount);
    // for creating descriptor set
    VkDescriptorSetLayout vkDescriptorSetLayout;
    // for creating descriptor pool
    VkDescriptorPool vkDescriptorPool;
    // subpass descriptor set
    VkDescriptorSet vkDescriptorSet;
    TknDynamicArray vkDescriptorPoolSizeDynamicArray;
    tknCreateDynamicArray(sizeof(VkDescriptorPoolSize), 1, &vkDescriptorPoolSizeDynamicArray);
    TknDynamicArray vkDescriptorSetLayoutBindingDynamicArray;
    tknCreateDynamicArray(sizeof(VkDescriptorSetLayoutBinding), 1, &vkDescriptorSetLayoutBindingDynamicArray);
    TknDynamicArray vkWriteDescriptorSetDynamicArray;
    tknCreateDynamicArray(sizeof(VkWriteDescriptorSet), 1, &vkWriteDescriptorSetDynamicArray);
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
                            VkAttachmentReference vkAttachmentReference = inputAttachmentReferences[pSpvReflectDescriptorBinding->input_attachment_index];
                            Attachment *pAttachment = attachmentPtrs[vkAttachmentReference.attachment];
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
                            tknAddToDynamicArray(&vkWriteDescriptorSetDynamicArray, &vkWriteDescriptorSet, vkWriteDescriptorSetDynamicArray.count);
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
        .inputAttachmentReferenceCount = inputAttachmentReferenceCount,
        .inputAttachmentReferences = inputAttachmentReferences,
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
        destroyPipeline(pGraphicsContext, pRenderPass, i, pPipeline);
    }
    tknDestroyDynamicArray(pSubpass->pipelinePtrDynamicArray);
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    ASSERT_VK_SUCCESS(vkFreeDescriptorSets(vkDevice, pSubpass->vkDescriptorPool, 1, &pSubpass->vkDescriptorSet));
    vkDestroyDescriptorPool(vkDevice, pSubpass->vkDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pSubpass->vkDescriptorSetLayout, NULL);
    tknFree(pSubpass->inputAttachmentReferences);
}

void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **attachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass)
{
    RenderPass *pRenderPass = tknMalloc(sizeof(RenderPass));
    pRenderPass->attachmentCount = attachmentCount;
    pRenderPass->attachmentPtrs = tknMalloc(sizeof(Attachment *) * attachmentCount);
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    bool useSwapchain = false;
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        Attachment *pAttachment = attachmentPtrs[i];
        pRenderPass->attachmentPtrs[i] = pAttachment;
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
    ASSERT_VK_SUCCESS(vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pRenderPass->vkRenderPass));

    if (useSwapchain)
    {
        pRenderPass->vkFramebufferCount = pGraphicsContext->swapchainImageCount;
        pRenderPass->vkFramebuffers = tknMalloc(sizeof(VkFramebuffer) * pRenderPass->vkFramebufferCount);
        for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
        {
            createFramebuffer(pGraphicsContext, pRenderPass->vkFramebufferCount, attachmentPtrs, pRenderPass, &pRenderPass->vkFramebuffers[i]);
        }
    }
    else
    {
        pRenderPass->vkFramebufferCount = 1;
        pRenderPass->vkFramebuffers = tknMalloc(sizeof(VkFramebuffer));
        createFramebuffer(pGraphicsContext, pRenderPass->vkFramebufferCount, attachmentPtrs, pRenderPass, &pRenderPass->vkFramebuffers[0]);
    }

    pRenderPass->subpassCount = subpassCount;
    pRenderPass->subpasses = tknMalloc(sizeof(Subpass) * pRenderPass->subpassCount);
    for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[subpassIndex];
        createSubpass(pGraphicsContext, vkSubpassDescriptions[subpassIndex], spvPathDynamicArrays[subpassIndex], attachmentPtrs, pSubpass);
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

    for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
    {
        destroyFramebuffer(pGraphicsContext, pRenderPass->vkFramebuffers[i]);
    }
    tknFree(pRenderPass->vkFramebuffers);

    vkDestroyRenderPass(pGraphicsContext->vkDevice, pRenderPass->vkRenderPass, NULL);
    tknFree(pRenderPass);
}
