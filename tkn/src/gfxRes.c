#include "gfxRes.h"
#include "gfxCommon.h"
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

Attachment *createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pGfxContext->swapchainExtent.width * scaler),
        .height = (uint32_t)(pGfxContext->swapchainExtent.height * scaler),
        .depth = 1,
    };
    Image *pImage = createImagePtr(pGfxContext, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags);
    DynamicAttachmentContent dynamicAttachmentContent = {
        .pImage = pImage,
        .scaler = scaler,
    };
    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_DYNAMIC,
        .attachmentContent.dynamicAttachmentContent = dynamicAttachmentContent,
    };
    return pAttachment;
}
void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment)
{
    tknAssert(pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC, "Attachment type mismatch!");
    DynamicAttachmentContent dynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
    destroyImagePtr(pGfxContext, dynamicAttachmentContent.pImage);
    tknFree(pAttachment);
}
void resizeDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment)
{
    tknAssert(pAttachment->attachmentType == ATTACHMENT_TYPE_DYNAMIC, "Attachment type mismatch!");
    DynamicAttachmentContent dynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
    destroyImagePtr(pGfxContext, dynamicAttachmentContent.pImage);
    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pGfxContext->swapchainExtent.width * dynamicAttachmentContent.scaler),
        .height = (uint32_t)(pGfxContext->swapchainExtent.height * dynamicAttachmentContent.scaler),
        .depth = 1,
    };
    dynamicAttachmentContent.pImage = createImagePtr(pGfxContext, vkExtent3D, dynamicAttachmentContent.pImage->vkFormat, VK_IMAGE_TILING_OPTIMAL, dynamicAttachmentContent.pImage->vkImageUsageFlags, dynamicAttachmentContent.pImage->vkMemoryPropertyFlags, dynamicAttachmentContent.pImage->vkImageAspectFlags);
    // TODO 引用关系被删除了需要加回来
}

Attachment *createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    FixedAttachmentContent fixedAttachmentContent = {
        .pImage = NULL,
        .width = width,
        .height = height,
    };
    VkExtent3D vkExtent3D = {
        .width = width,
        .height = height,
        .depth = 1,
    };

    pAttachment->attachmentContent.fixedAttachmentContent.pImage = createImagePtr(pGfxContext, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags);

    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_FIXED,
        .attachmentContent.fixedAttachmentContent = fixedAttachmentContent,
    };
    return pAttachment;
}
void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment)
{
    tknAssert(pAttachment->attachmentType == ATTACHMENT_TYPE_FIXED, "Attachment type mismatch!");
    VkDevice vkDevice = pGfxContext->vkDevice;
    FixedAttachmentContent fixedAttachmentContent = pAttachment->attachmentContent.fixedAttachmentContent;
    destroyImagePtr(pGfxContext, fixedAttachmentContent.pImage);
    tknFree(pAttachment);
}
Attachment *getSwapchainAttachmentPtr(GfxContext *pGfxContext)
{
    return pGfxContext->swapchainAttachmentPtr;
}

Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags)
{
    Image *pImage = tknMalloc(sizeof(Image));
    VkDevice vkDevice = pGfxContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGfxContext->vkPhysicalDevice;

    VkImage vkImage;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;

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
    assertVkResult(vkCreateImage(vkDevice, &imageCreateInfo, NULL, &vkImage));
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, vkImage, &memoryRequirements);
    uint32_t memoryTypeIndex;
    getMemoryType(vkPhysicalDevice, memoryRequirements.memoryTypeBits, vkMemoryPropertyFlags, &memoryTypeIndex);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    assertVkResult(vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, &vkDeviceMemory));
    assertVkResult(vkBindImageMemory(vkDevice, vkImage, vkDeviceMemory, 0));

    VkComponentMapping components = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };
    VkImageSubresourceRange subresourceRange = {
        .aspectMask = vkImageAspectFlags,
        .levelCount = 1,
        .baseMipLevel = 0,
        .layerCount = 1,
        .baseArrayLayer = 0,
    };
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = vkImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = vkFormat,
        .components = components,
        .subresourceRange = subresourceRange,
    };
    assertVkResult(vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, &vkImageView));

    Image image = {
        .vkImage = vkImage,
        .vkDeviceMemory = vkDeviceMemory,
        .vkImageView = vkImageView,
        .vkExtent3D = vkExtent3D,
        .vkFormat = vkFormat,
        .vkImageTiling = vkImageTiling,
        .vkImageUsageFlags = vkImageUsageFlags,
        .vkMemoryPropertyFlags = vkMemoryPropertyFlags,
        .vkImageAspectFlags = vkImageAspectFlags,
        .descriptorPtrHashSet = tknCreateHashSet(1),
    };
    *pImage = image;

    return pImage;
}
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage)
{
    for (uint32_t i = 0; i < pImage->descriptorPtrHashSet.capacity; i++)
    {
        TknListNode *node = pImage->descriptorPtrHashSet.nodePtrs[i];
        while (node)
        {
            Descriptor *pDescriptor = (Descriptor *)node->value;
            DescriptorBinding descriptorBinding = (DescriptorBinding){
                .vkDescriptorType = pDescriptor->vkDescriptorType,
                .descriptorContent = getNullDescriptorContent(pDescriptor->vkDescriptorType),
                .binding = pDescriptor->binding,
            };
            updateDescriptorSetPtr(pGfxContext, pDescriptor->pDescriptorSet, 1, &descriptorBinding);
            node = node->nextNodePtr;
        }
    }
    VkDevice vkDevice = pGfxContext->vkDevice;
    tknDestroyHashSet(pImage->descriptorPtrHashSet);
    vkDestroyImageView(vkDevice, pImage->vkImageView, NULL);
    vkDestroyImage(vkDevice, pImage->vkImage, NULL);
    vkFreeMemory(vkDevice, pImage->vkDeviceMemory, NULL);

    tknFree(pImage);
}
