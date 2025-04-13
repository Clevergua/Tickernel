#include "graphicCore.h"

static void createImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage *pVkImage, VkDeviceMemory *pVkDeviceMemory)
{
    VkResult result = VK_SUCCESS;

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
    result = vkCreateImage(vkDevice, &imageCreateInfo, NULL, pVkImage);
    tryThrowVulkanError(result);
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, *pVkImage, &memoryRequirements);
    uint32_t memoryTypeIndex;
    findMemoryType(vkPhysicalDevice, memoryRequirements.memoryTypeBits, vkMemoryPropertyFlags, &memoryTypeIndex);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };

    result = vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pVkDeviceMemory);
    tryThrowVulkanError(result);
    result = vkBindImageMemory(vkDevice, *pVkImage, *pVkDeviceMemory, 0);
    tryThrowVulkanError(result);
}

static void findSupportedFormat(VkPhysicalDevice vkPhysicalDevice, VkFormat *candidates, uint32_t candidatesCount, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat *vkFormat)
{

    for (uint32_t i = 0; i < candidatesCount; i++)
    {
        VkFormat format = candidates[i];
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, format, &properties);
        if ((properties.optimalTilingFeatures & features) == features)
        {
            if (VK_IMAGE_TILING_LINEAR == tiling || VK_IMAGE_TILING_OPTIMAL == tiling)
            {
                *vkFormat = format;
                return;
            }
            else
            {
                // continue;
            }
        }
    }
    printf("Target format not found!");
}

void copyVkBuffer(VkCommandPool graphicVkCommandPool, VkDevice vkDevice, VkQueue vkGraphicQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize offset, VkDeviceSize size)
{
    VkCommandBuffer vkCommandBuffer;
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = graphicVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VkResult result = vkAllocateCommandBuffers(vkDevice, &vkCommandBufferAllocateInfo, &vkCommandBuffer);
    tryThrowVulkanError(result);

    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = NULL,
        };
    result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    tryThrowVulkanError(result);

    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = offset,
        .size = size,
    };
    vkCmdCopyBuffer(vkCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(vkCommandBuffer);
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .pWaitDstStageMask = NULL,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkCommandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = NULL,
    };
    vkQueueSubmit(vkGraphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkGraphicQueue);
    vkFreeCommandBuffers(vkDevice, graphicVkCommandPool, 1, &vkCommandBuffer);
}

void createBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags msemoryPropertyFlags, VkBuffer *pBuffer, VkDeviceMemory *pDeviceMemory)
{
    VkResult result = VK_SUCCESS;
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
    result = vkCreateBuffer(vkDevice, &bufferCreateInfo, NULL, pBuffer);
    tryThrowVulkanError(result);
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vkDevice, *pBuffer, &memoryRequirements);
    uint32_t memoryTypeIndex;
    findMemoryType(vkPhysicalDevice, memoryRequirements.memoryTypeBits, msemoryPropertyFlags, &memoryTypeIndex);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    result = vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pDeviceMemory);
    tryThrowVulkanError(result);
    result = vkBindBufferMemory(vkDevice, *pBuffer, *pDeviceMemory, 0);
    tryThrowVulkanError(result);
}
void destroyBuffer(VkDevice vkDevice, VkBuffer vkBuffer, VkDeviceMemory deviceMemory)
{
    vkFreeMemory(vkDevice, deviceMemory, NULL);
    vkDestroyBuffer(vkDevice, vkBuffer, NULL);
}

void updateBufferWithStagingBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer vkBuffer)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(vkDevice, vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void *pData;
    VkResult result = vkMapMemory(vkDevice, stagingBufferMemory, offset, bufferSize, 0, &pData);
    tryThrowVulkanError(result);
    memcpy(pData, bufferData, bufferSize);
    vkUnmapMemory(vkDevice, stagingBufferMemory);

    copyVkBuffer(graphicVkCommandPool, vkDevice, vkGraphicQueue, stagingBuffer, vkBuffer, 0, bufferSize);
    destroyBuffer(vkDevice, stagingBuffer, stagingBufferMemory);
}
void updateBuffer(VkDevice vkDevice, VkDeviceMemory bufferMemory, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData)
{
    void *data;
    vkMapMemory(vkDevice, bufferMemory, offset, bufferSize, 0, &data);
    memcpy(data, bufferData, bufferSize);
    vkUnmapMemory(vkDevice, bufferMemory);
}

void findMemoryType(VkPhysicalDevice vkPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex)
{
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
    printf("Failed to find suitable memory type!");
}

void findDepthFormat(VkPhysicalDevice vkPhysicalDevice, VkFormat *pDepthFormat)
{
    uint32_t candidatesCount = 3;
    VkFormat *candidates = (VkFormat[]){VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    findSupportedFormat(vkPhysicalDevice, candidates, candidatesCount, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, pDepthFormat);
}

void createImageView(VkDevice vkDevice, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView)
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
    VkResult result = vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, pImageView);
    tryThrowVulkanError(result);
}

void createGraphicImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage)
{
    pGraphicImage->vkFormat = vkFormat;
    createImage(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, &pGraphicImage->vkImage, &pGraphicImage->vkDeviceMemory);
    createImageView(vkDevice, pGraphicImage->vkImage, pGraphicImage->vkFormat, vkImageAspectFlags, &pGraphicImage->vkImageView);
}

void destroyGraphicImage(VkDevice vkDevice, GraphicImage graphicImage)
{
    vkDestroyImageView(vkDevice, graphicImage.vkImageView, NULL);
    vkDestroyImage(vkDevice, graphicImage.vkImage, NULL);
    vkFreeMemory(vkDevice, graphicImage.vkDeviceMemory, NULL);
}

void createVkShaderModule(VkDevice vkDevice, const char *filePath, VkShaderModule *pVkShaderModule)
{
    FILE *pFile = fopen(filePath, "rb");
    if (NULL == pFile)
    {
        tickernelError("Failed to read file with path: %s\n", filePath);
    }
    else
    {
        fseek(pFile, 0, SEEK_END);
        size_t fileLength = ftell(pFile);
        rewind(pFile);

        uint32_t *pCode = tickernelMalloc(fileLength);
        size_t codeSize = fread(pCode, 1, fileLength, pFile);

        fclose(pFile);
        if (codeSize == fileLength)
        {
            printf("Succeeded to read file!\n");
            VkShaderModuleCreateInfo shaderModuleCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .codeSize = codeSize,
                .pCode = pCode,
            };
            VkResult result = vkCreateShaderModule(vkDevice, &shaderModuleCreateInfo, NULL, pVkShaderModule);
            tryThrowVulkanError(result);
            tickernelFree(pCode);
        }
        else
        {
            printf("Failed to read file codeSize:%zu fileLength:%zu\n", codeSize, fileLength);
        }
    }
}

void destroyVkShaderModule(VkDevice vkDevice, VkShaderModule vkShaderModule)
{
    vkDestroyShaderModule(vkDevice, vkShaderModule, NULL);
}

static void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1};
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1}};

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        printf("Unsupported layout transition!\n");
        return;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier);

    vkEndCommandBuffer(commandBuffer);
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer};
    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

static void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1};
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {width, height, 1}};

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer};
    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void createASTCGraphicImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, const char *fileName, VkCommandPool commandPool, VkQueue graphicQueue, GraphicImage *pGraphicImage)
{
    FILE *file = fopen(fileName, "rb");
    if (!file)
    {
        tickernelError("Failed to open ASTC file: %s\n", fileName);
    }

    ASTCHeader header;
    if (fread(&header, sizeof(ASTCHeader), 1, file) != 1)
    {
        fclose(file);
        tickernelError("Invalid ASTC file header: %s\n", fileName);
    }

    if (memcmp(header.magic, "ASTC", 4) != 0)
    {
        fclose(file);
        tickernelError("Not an ASTC file: %s\n", fileName);
    }

    uint32_t width = (header.width[0] << 16) | (header.width[1] << 8) | header.width[2];
    uint32_t height = (header.height[0] << 16) | (header.height[1] << 8) | header.height[2];
    uint8_t blockWidth = header.blockDimX;
    uint8_t blockHeight = header.blockDimY;

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file) - sizeof(ASTCHeader);
    fseek(file, sizeof(ASTCHeader), SEEK_SET);

    uint8_t *astcData = tickernelMalloc(fileSize);
    if (fread(astcData, 1, fileSize, file) != fileSize)
    {
        tickernelFree(astcData);
        fclose(file);
        tickernelError("Failed to read ASTC data: %s\n", fileName);
    }
    fclose(file);

    VkFormat astcFormat;
    switch ((blockWidth << 8) | blockHeight)
    {
    case 0x0404:
        astcFormat = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        break;
    case 0x0504:
        astcFormat = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        break;
    case 0x0505:
        astcFormat = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        break;
    case 0x0605:
        astcFormat = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        break;
    case 0x0606:
        astcFormat = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        break;
    case 0x0805:
        astcFormat = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        break;
    case 0x0806:
        astcFormat = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        break;
    case 0x0808:
        astcFormat = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        break;
    case 0x0A05:
        astcFormat = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        break;
    case 0x0A06:
        astcFormat = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        break;
    case 0x0A08:
        astcFormat = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        break;
    case 0x0A0A:
        astcFormat = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        break;
    case 0x0C08:
        astcFormat = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        break;
    case 0x0C0A:
        astcFormat = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        break;
    default:
        tickernelFree(astcData);
        tickernelError("Unsupported ASTC block size: %dx%d\n", blockWidth, blockHeight);
    }

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, astcFormat, &props);
    if (!(props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
    {
        tickernelFree(astcData);
        tickernelError("Device does not support ASTC format: %x\n", astcFormat);
    }

    VkExtent3D imageExtent = {width, height, 1};
    createGraphicImage(
        vkDevice, vkPhysicalDevice,
        imageExtent,
        astcFormat,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        pGraphicImage);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    createBuffer(
        vkDevice, vkPhysicalDevice,
        fileSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &stagingBuffer, &stagingMemory);

    updateBuffer(vkDevice, stagingMemory, 0, fileSize, astcData);
    tickernelFree(astcData);

    transitionImageLayout(
        vkDevice, commandPool, graphicQueue,
        pGraphicImage->vkImage,
        astcFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(
        vkDevice, commandPool, graphicQueue,
        stagingBuffer,
        pGraphicImage->vkImage,
        width, height);
    transitionImageLayout(
        vkDevice, commandPool, graphicQueue,
        pGraphicImage->vkImage,
        astcFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    destroyBuffer(vkDevice, stagingBuffer, stagingMemory);
}

void destroyASTCGraphicImage(VkDevice vkDevice, GraphicImage *pGraphicImage)
{
    destroyGraphicImage(vkDevice, *pGraphicImage);
}

void createSampler(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkSampler *pVkSampler)
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);
    VkSamplerCreateInfo samplerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0,
        .borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .minLod = 0.0f,
        .maxLod = 0.0f,
    };
    VkResult result = vkCreateSampler(vkDevice, &samplerCreateInfo, NULL, pVkSampler);
    tryThrowVulkanError(result);
}

void destroySampler(VkDevice vkDevice, VkSampler vkSampler)
{
    vkDestroySampler(vkDevice, vkSampler, NULL);
}