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

// Helper function to get bytes per pixel for different Vulkan formats
static uint32_t getBytesPerPixel(VkFormat format)
{
    switch (format)
    {
        // 8-bit formats (1 byte per pixel)
        case VK_FORMAT_R8_UNORM:
        case VK_FORMAT_R8_SNORM:
        case VK_FORMAT_R8_UINT:
        case VK_FORMAT_R8_SINT:
        case VK_FORMAT_R8_SRGB:
            return 1;

        // 16-bit formats (2 bytes per pixel)
        case VK_FORMAT_R8G8_UNORM:
        case VK_FORMAT_R8G8_SNORM:
        case VK_FORMAT_R8G8_UINT:
        case VK_FORMAT_R8G8_SINT:
        case VK_FORMAT_R8G8_SRGB:
        case VK_FORMAT_R16_UNORM:
        case VK_FORMAT_R16_SNORM:
        case VK_FORMAT_R16_UINT:
        case VK_FORMAT_R16_SINT:
        case VK_FORMAT_R16_SFLOAT:
            return 2;

        // 24-bit formats (3 bytes per pixel)
        case VK_FORMAT_R8G8B8_UNORM:
        case VK_FORMAT_R8G8B8_SNORM:
        case VK_FORMAT_R8G8B8_UINT:
        case VK_FORMAT_R8G8B8_SINT:
        case VK_FORMAT_R8G8B8_SRGB:
        case VK_FORMAT_B8G8R8_UNORM:
        case VK_FORMAT_B8G8R8_SNORM:
        case VK_FORMAT_B8G8R8_UINT:
        case VK_FORMAT_B8G8R8_SINT:
        case VK_FORMAT_B8G8R8_SRGB:
            return 3;

        // 32-bit formats (4 bytes per pixel) - Most common
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SNORM:
        case VK_FORMAT_R8G8B8A8_UINT:
        case VK_FORMAT_R8G8B8A8_SINT:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_B8G8R8A8_UNORM:
        case VK_FORMAT_B8G8R8A8_SNORM:
        case VK_FORMAT_B8G8R8A8_UINT:
        case VK_FORMAT_B8G8R8A8_SINT:
        case VK_FORMAT_B8G8R8A8_SRGB:
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        case VK_FORMAT_R16G16_UNORM:
        case VK_FORMAT_R16G16_SNORM:
        case VK_FORMAT_R16G16_UINT:
        case VK_FORMAT_R16G16_SINT:
        case VK_FORMAT_R16G16_SFLOAT:
        case VK_FORMAT_R32_UINT:
        case VK_FORMAT_R32_SINT:
        case VK_FORMAT_R32_SFLOAT:
            return 4;

        // 48-bit formats (6 bytes per pixel)
        case VK_FORMAT_R16G16B16_UNORM:
        case VK_FORMAT_R16G16B16_SNORM:
        case VK_FORMAT_R16G16B16_UINT:
        case VK_FORMAT_R16G16B16_SINT:
        case VK_FORMAT_R16G16B16_SFLOAT:
            return 6;

        // 64-bit formats (8 bytes per pixel)
        case VK_FORMAT_R16G16B16A16_UNORM:
        case VK_FORMAT_R16G16B16A16_SNORM:
        case VK_FORMAT_R16G16B16A16_UINT:
        case VK_FORMAT_R16G16B16A16_SINT:
        case VK_FORMAT_R16G16B16A16_SFLOAT:
        case VK_FORMAT_R32G32_UINT:
        case VK_FORMAT_R32G32_SINT:
        case VK_FORMAT_R32G32_SFLOAT:
        case VK_FORMAT_R64_UINT:
        case VK_FORMAT_R64_SINT:
        case VK_FORMAT_R64_SFLOAT:
            return 8;

        // 96-bit formats (12 bytes per pixel)
        case VK_FORMAT_R32G32B32_UINT:
        case VK_FORMAT_R32G32B32_SINT:
        case VK_FORMAT_R32G32B32_SFLOAT:
            return 12;

        // 128-bit formats (16 bytes per pixel)
        case VK_FORMAT_R32G32B32A32_UINT:
        case VK_FORMAT_R32G32B32A32_SINT:
        case VK_FORMAT_R32G32B32A32_SFLOAT:
        case VK_FORMAT_R64G64_UINT:
        case VK_FORMAT_R64G64_SINT:
        case VK_FORMAT_R64G64_SFLOAT:
            return 16;

        // Depth/Stencil formats
        case VK_FORMAT_D16_UNORM:
            return 2;
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return 4;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return 8;

        // Compressed formats - these require special handling
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        case VK_FORMAT_BC4_UNORM_BLOCK:
        case VK_FORMAT_BC4_SNORM_BLOCK:
            tknError("Compressed format %d requires special block-based calculation", format);
            return 0;

        default:
            tknError("Unsupported or unknown format: %d", format);
            return 4; // Default to 4 bytes (RGBA8) as fallback
    }
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

Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, void *data)
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
    if (data != NULL)
    {
        // Data provided - upload it immediately
        uint32_t width = vkExtent3D.width;
        uint32_t height = vkExtent3D.height;
        uint32_t bytesPerPixel = getBytesPerPixel(vkFormat);
        VkDeviceSize size = (VkDeviceSize)width * height * bytesPerPixel;
        
        if (size > 0)
        {
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
        else
        {
            tknError("Invalid size calculation during image creation: width=%u, height=%u, bytesPerPixel=%u", width, height, bytesPerPixel);
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

