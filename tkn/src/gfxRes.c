#include "gfxRes.h"

void assertVkResult(VkResult vkResult)
{
    tknAssert(vkResult == VK_SUCCESS, "Vulkan error: %d", vkResult);
}

DescriptorContent getNullDescriptorContent(VkDescriptorType vkDescriptorType)
{
    DescriptorContent descriptorContent = {0};
    // VK_DESCRIPTOR_TYPE_SAMPLER = 0,
    if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
    {
        descriptorContent.samplerDescriptorContent.pSampler = NULL;
    }
    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
    // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER == vkDescriptorType)
    {
        descriptorContent.uniformBufferDescriptorContent.pBuffer = NULL;
    }
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
    // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
    else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorType)
    {
        descriptorContent.inputAttachmentDescriptorContent.pAttachment = NULL;
    }
    else
    {
        tknError("Unsupported descriptor type: %d", vkDescriptorType);
    }
    return descriptorContent;
}


static uint32_t getMemoryTypeIndex(VkPhysicalDevice vkPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags)
{
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &physicalDeviceMemoryProperties);
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
        {
            return i;
        }
    }
    tknError("Failed to get suitable memory type!");
    return UINT32_MAX;
}
static Buffer createBuffer(GfxContext *pGfxContext, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags msemoryPropertyFlags)
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
    void *mapped;
    TknHashSet descriptorPtrHashSet = tknCreateHashSet(1);

    VkDevice vkDevice = pGfxContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGfxContext->vkPhysicalDevice;
    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = bufferSize,
        .usage = bufferUsageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = 0,
    };
    assertVkResult(vkCreateBuffer(vkDevice, &bufferCreateInfo, NULL, &vkBuffer));
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vkDevice, vkBuffer, &memoryRequirements);
    uint32_t memoryTypeIndex = getMemoryTypeIndex(vkPhysicalDevice, memoryRequirements.memoryTypeBits, msemoryPropertyFlags);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    assertVkResult(vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, &vkDeviceMemory));
    assertVkResult(vkBindBufferMemory(vkDevice, vkBuffer, vkDeviceMemory, 0));
    Buffer buffer = {
        .vkBuffer = vkBuffer,
        .vkDeviceMemory = vkDeviceMemory,
        .descriptorPtrHashSet = descriptorPtrHashSet,
    };
    return buffer;
}
static void destroyBuffer(GfxContext *pGfxContext, Buffer buffer)
{
    for (uint32_t i = 0; i < buffer.descriptorPtrHashSet.capacity; i++)
    {
        TknListNode *node = buffer.descriptorPtrHashSet.nodePtrs[i];
        while (node)
        {
            Descriptor *pDescriptor = (Descriptor *)node->value;
            Descriptor newDescriptor = *pDescriptor;
            newDescriptor.descriptorContent = getNullDescriptorContent(pDescriptor->vkDescriptorType);
            updateDescriptors(pGfxContext, 1, &newDescriptor);
            node = node->nextNodePtr;
        }
    }
    VkDevice vkDevice = pGfxContext->vkDevice;
    tknDestroyHashSet(buffer.descriptorPtrHashSet);
    vkUnmapMemory(vkDevice, buffer.vkDeviceMemory);
    vkDestroyBuffer(vkDevice, buffer.vkBuffer, NULL);
    vkFreeMemory(vkDevice, buffer.vkDeviceMemory, NULL);
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

    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pGfxContext->swapchainExtent.width * dynamicAttachmentContent.scaler),
        .height = (uint32_t)(pGfxContext->swapchainExtent.height * dynamicAttachmentContent.scaler),
        .depth = 1,
    };
    Image *pCurrentImage = dynamicAttachmentContent.pImage;
    Image *pNewImage = createImagePtr(pGfxContext, vkExtent3D, pCurrentImage->vkFormat, VK_IMAGE_TILING_OPTIMAL, pCurrentImage->vkImageUsageFlags, pCurrentImage->vkMemoryPropertyFlags, pCurrentImage->vkImageAspectFlags);
    for (uint32_t i = 0; i < pCurrentImage->descriptorPtrHashSet.capacity; i++)
    {
        TknListNode *node = pCurrentImage->descriptorPtrHashSet.nodePtrs[i];
        while (node)
        {
            Descriptor *pDescriptor = (Descriptor *)node->value;
            pDescriptor->descriptorContent.inputAttachmentDescriptorContent.pAttachment->attachmentContent.dynamicAttachmentContent.pImage = pNewImage;
            updateDescriptors(pGfxContext, 1, pDescriptor);
            node = node->nextNodePtr;
        }
    }
    destroyImagePtr(pGfxContext, dynamicAttachmentContent.pImage);
    dynamicAttachmentContent.pImage = pNewImage;
}

Attachment *createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    VkExtent3D vkExtent3D = {
        .width = width,
        .height = height,
        .depth = 1,
    };

    Image *pImage = createImagePtr(pGfxContext, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags);

    FixedAttachmentContent fixedAttachmentContent = {
        .pImage = pImage,
        .width = width,
        .height = height,
    };

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
    uint32_t memoryTypeIndex = getMemoryTypeIndex(vkPhysicalDevice, memoryRequirements.memoryTypeBits, vkMemoryPropertyFlags);
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
            Descriptor newDescriptor = *pDescriptor;
            newDescriptor.descriptorContent = getNullDescriptorContent(pDescriptor->vkDescriptorType);
            updateDescriptors(pGfxContext, 1, &newDescriptor);
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

MappedBuffer *createUniformBufferPtr(GfxContext *pGfxContext, VkDeviceSize size)
{
    MappedBuffer *pMappedBuffer = tknMalloc(sizeof(MappedBuffer));
    Buffer buffer = createBuffer(pGfxContext, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VkDevice vkDevice = pGfxContext->vkDevice;
    void *mapped;
    assertVkResult(vkMapMemory(vkDevice, buffer.vkDeviceMemory, 0, size, 0, &mapped));
    *pMappedBuffer = (MappedBuffer){
        .buffer = buffer,
        .mapped = mapped,
    };
    return pMappedBuffer;
}
void destroyMappedBufferPtr(GfxContext *pGfxContext, MappedBuffer *pMappedBuffer)
{
    Buffer buffer = pMappedBuffer->buffer;
    VkDevice vkDevice = pGfxContext->vkDevice;

    destroyBuffer(pGfxContext, buffer);
    tknFree(pMappedBuffer);
}
void updateMappedBufferPtr(GfxContext *pGfxContext, MappedBuffer *pMappedBuffer, const void *data, VkDeviceSize size)
{
    tknAssert(pMappedBuffer->buffer.vkDeviceMemory != VK_NULL_HANDLE, "Mapped buffer memory is not allocated!");
    tknAssert(size <= pMappedBuffer->buffer.size, "Data size exceeds mapped buffer size!");
    memcpy(pMappedBuffer->mapped, data, size);
}