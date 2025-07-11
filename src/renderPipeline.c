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

void getSwapchainAttachment(GraphicsContext *pGraphicsContext, uint32_t swapchainIndex, Attachment **ppAttachment)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    SwapchainAttachmentContent swapchainAttachmentContent = {
        .swapchainIndex = swapchainIndex,
    };
    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_SWAPCHAIN,
        .attachmentContent.swapchainAttachmentContent = swapchainAttachmentContent,
    };
    *ppAttachment = pAttachment;
}

// void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **pAttachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass)
// {

// }

// void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass)
// {

// }
