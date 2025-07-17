#include "renderResources.h"

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

static void createImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, Image *pImage)
{
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
    ASSERT_VK_SUCCESS(vkCreateImage(vkDevice, &imageCreateInfo, NULL, &vkImage));
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
    ASSERT_VK_SUCCESS(vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, &vkDeviceMemory));
    ASSERT_VK_SUCCESS(vkBindImageMemory(vkDevice, vkImage, vkDeviceMemory, 0));

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
    ASSERT_VK_SUCCESS(vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, &vkImageView));

    *pImage = (Image){
        .vkImage = vkImage,
        .vkDeviceMemory = vkDeviceMemory,
        .vkImageView = vkImageView,
    };
}
static void destroyImage(VkDevice vkDevice, Image image)
{
    vkDestroyImageView(vkDevice, image.vkImageView, NULL);
    vkDestroyImage(vkDevice, image.vkImage, NULL);
    vkFreeMemory(vkDevice, image.vkDeviceMemory, NULL);
}

void createImagePtr(GraphicsContext *pGraphicsContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, Image **ppImage)
{
    Image image;
    createImage(pGraphicsContext->vkDevice, pGraphicsContext->vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, &image);
    *ppImage = tknMalloc(sizeof(Image));
    **ppImage = image;
}
void destroyImagePtr(GraphicsContext *pGraphicsContext, Image *pImage)
{
    destroyImage(pGraphicsContext->vkDevice, *pImage);
    tknFree(pImage);
}

void createDynamicAttachmentPtr(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment **ppAttachment)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pGraphicsContext->swapchainExtent.width * scaler),
        .height = (uint32_t)(pGraphicsContext->swapchainExtent.height * scaler),
        .depth = 1,
    };
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    Image image;
    createImage(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, &image);
    DynamicAttachmentContent dynamicAttachmentContent = {
        .image = image,
        .vkFormat = vkFormat,
        .scaler = scaler,
        .vkImageUsageFlags = vkImageUsageFlags,
        .vkMemoryPropertyFlags = vkMemoryPropertyFlags,
        .vkImageAspectFlags = vkImageAspectFlags,
    };
    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_DYNAMIC,
        .attachmentContent.dynamicAttachmentContent = dynamicAttachmentContent,
    };
    *ppAttachment = pAttachment;
}
void destroyDynamicAttachmentPtr(GraphicsContext *pGraphicsContext, Attachment *pAttachment)
{
    DynamicAttachmentContent dynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
    destroyImage(pGraphicsContext->vkDevice, dynamicAttachmentContent.image);
    tknFree(pAttachment);
}
void resizeDynamicAttachmentPtr(GraphicsContext *pGraphicsContext, Attachment *pAttachment)
{
    DynamicAttachmentContent dynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
    destroyImage(pGraphicsContext->vkDevice, dynamicAttachmentContent.image);

    // Create a new image and image view with the new scaler
    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pGraphicsContext->swapchainExtent.width * pAttachment->attachmentContent.dynamicAttachmentContent.scaler),
        .height = (uint32_t)(pGraphicsContext->swapchainExtent.height * pAttachment->attachmentContent.dynamicAttachmentContent.scaler),
        .depth = 1,
    };
    createImage(pGraphicsContext->vkDevice, pGraphicsContext->vkPhysicalDevice, vkExtent3D, dynamicAttachmentContent.vkFormat, VK_IMAGE_TILING_OPTIMAL, dynamicAttachmentContent.vkImageUsageFlags, dynamicAttachmentContent.vkMemoryPropertyFlags, dynamicAttachmentContent.vkImageAspectFlags, &dynamicAttachmentContent.image);
}

void createFixedAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment **ppAttachment)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    FixedAttachmentContent fixedAttachmentContent = {
        .image = {0},
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
    createImage(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, &pAttachment->attachmentContent.fixedAttachmentContent.image);
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
    destroyImage(vkDevice, fixedAttachmentContent.image);
    tknFree(pAttachment);
}

void getSwapchainAttachments(GraphicsContext *pGraphicsContext, Attachment **pAttachments)
{
    *pAttachments = pGraphicsContext->swapchainAttachmentPtr;
}