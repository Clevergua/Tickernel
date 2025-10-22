#include "gfxCore.h"

// Helper function to transition image layout
static void transitionImageLayout(GfxContext *pGfxContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(pGfxContext);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        tknError("Unsupported layout transition!");
        return;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier);

    endSingleTimeCommands(pGfxContext, commandBuffer);
}

// Helper function to copy buffer to image
static void copyBufferToImage(GfxContext *pGfxContext, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(pGfxContext);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = (VkOffset3D){0, 0, 0};
    region.imageExtent = (VkExtent3D){width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(pGfxContext, commandBuffer);
}

// Helper function similar to createBufferWithData for staging operations
static bool createStagingBufferWithData(GfxContext *pGfxContext, void *data, VkDeviceSize size, VkBuffer *pStagingBuffer, VkDeviceMemory *pStagingBufferMemory)
{
    if (size == 0)
    {
        *pStagingBuffer = VK_NULL_HANDLE;
        *pStagingBufferMemory = VK_NULL_HANDLE;
        return true;
    }

    // Create staging buffer
    createVkBuffer(pGfxContext, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   pStagingBuffer, pStagingBufferMemory);

    // Copy data to staging buffer
    void *mappedData;
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkMapMemory(vkDevice, *pStagingBufferMemory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, (size_t)size);
    vkUnmapMemory(vkDevice, *pStagingBufferMemory);

    return true;
}

Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, void *data, VkDeviceSize dataSize)
{
    Image *pImage = tknMalloc(sizeof(Image));
    VkImage vkImage;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
    
    // Create the Vulkan image
    createVkImage(pGfxContext, vkExtent3D, vkFormat, vkImageTiling, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, &vkImage, &vkDeviceMemory, &vkImageView);
    
    Image image = {
        .vkImage = vkImage,
        .vkDeviceMemory = vkDeviceMemory,
        .vkImageView = vkImageView,
        .bindingPtrHashSet = tknCreateHashSet(sizeof(Binding *)),
    };
    *pImage = image;
    
    // Handle image layout initialization based on usage
    if (data != NULL && dataSize > 0)
    {
        // Data provided - upload it immediately
        uint32_t width = vkExtent3D.width;
        uint32_t height = vkExtent3D.height;
        VkDeviceSize size = dataSize;
        
        // Create staging buffer with data
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        if (createStagingBufferWithData(pGfxContext, data, size, &stagingBuffer, &stagingBufferMemory))
        {
            // Transition image layout for transfer
            transitionImageLayout(pGfxContext, pImage->vkImage, vkFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            
            // Copy buffer to image
            copyBufferToImage(pGfxContext, stagingBuffer, pImage->vkImage, width, height);
            
            // Transition image layout for shader access
            transitionImageLayout(pGfxContext, pImage->vkImage, vkFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            
            // Clean up staging buffer
            destroyVkBuffer(pGfxContext, stagingBuffer, stagingBufferMemory);
        }
        else
        {
            tknError("Failed to create staging buffer during image creation with data");
        }
    }
    // Note: Empty images (data == NULL) remain in VK_IMAGE_LAYOUT_UNDEFINED state
    // Layout transitions will be handled when the image is actually used
    return pImage;
}
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage)
{
    clearBindingPtrHashSet(pGfxContext, pImage->bindingPtrHashSet);
    tknDestroyHashSet(pImage->bindingPtrHashSet);
    destroyVkImage(pGfxContext, pImage->vkImage, pImage->vkDeviceMemory, pImage->vkImageView);
    tknFree(pImage);
}

