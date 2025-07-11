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

static void createImage2D(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage *pVkImage, VkDeviceMemory *pVkDeviceMemory)
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
    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_DYNAMIC,
        .attachmentContent = (AttachmentContent)dynamicAttachmentContent,
    };

    pAttachment->attachmentType = ATTACHMENT_TYPE_DYNAMIC;
    DynamicAttachmentContent *pDynamicAttachmentContent = &pAttachment->attachmentContent.dynamicAttachmentContent;
    pDynamicAttachmentContent->vkFormat = vkFormat;
    pDynamicAttachmentContent->vkImageUsageFlags = vkImageUsageFlags;
    pDynamicAttachmentContent->vkMemoryPropertyFlags = vkMemoryPropertyFlags;
    pDynamicAttachmentContent->vkImageAspectFlags = vkImageAspectFlags;
    VkExtent3D vkExtent3D =
        {
            .width = (uint32_t)(pGraphicsContext->swapchainExtent.width * scaler),
            .height = (uint32_t)(pGraphicsContext->swapchainExtent.height * scaler),
            .depth = 1,
        };
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    // pGraphicsImage->vkFormat = vkFormat;

    createImage2D(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, &pAttachment->attachmentContent.dynamicAttachmentContent.vkImage, &pAttachment->attachmentContent.dynamicAttachmentContent.vkDeviceMemory);
    createImageView(vkDevice, pAttachment->attachmentContent.dynamicAttachmentContent.vkImage, vkFormat, vkImageAspectFlags, &pAttachment->attachmentContent.dynamicAttachmentContent.vkImageView);

    *ppAttachment = pAttachment;
}
void destroyDynamicAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment)
{
    tickernelRemoveFromDynamicArray(&pGraphicsContext->attachmentPtrDynamicArray, &pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage);
    destroyGraphicsImage(pGraphicsContext, pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage);
    tickernelDestroyDynamicArray(pAttachment->attachmentContent.dynamicAttachmentContent.subpassPtrDynamicArray);
    tickernelFree(pAttachment);
}
void resizeDynamicAttachment()
{
}

void createFixedAttachment()
{
}
void destroyFixedAttachment()
{
}

// void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **pAttachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass)
// {

// }

// void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass)
// {

// }
