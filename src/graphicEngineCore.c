#include <graphicEngineCore.h>

static void CreateImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage *pVkImage, VkDeviceMemory *pVkDeviceMemory)
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
    TryThrowVulkanError(result);
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, *pVkImage, &memoryRequirements);
    uint32_t memoryTypeIndex;
    FindMemoryType(vkPhysicalDevice, memoryRequirements.memoryTypeBits, vkMemoryPropertyFlags, &memoryTypeIndex);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };

    result = vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pVkDeviceMemory);
    TryThrowVulkanError(result);
    result = vkBindImageMemory(vkDevice, *pVkImage, *pVkDeviceMemory, 0);
    TryThrowVulkanError(result);
}

static void FindSupportedFormat(VkPhysicalDevice vkPhysicalDevice, VkFormat *candidates, uint32_t candidatesCount, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat *vkFormat)
{
    VkResult result = VK_SUCCESS;

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

static void CopyVkBuffer(VkCommandPool graphicVkCommandPool, VkDevice vkDevice, VkQueue vkGraphicQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
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
    TryThrowVulkanError(result);

    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = NULL,
        };
    result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    TryThrowVulkanError(result);

    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
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

void CreateBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags msemoryPropertyFlags, VkBuffer *pBuffer, VkDeviceMemory *pDeviceMemory)
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
    TryThrowVulkanError(result);
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vkDevice, *pBuffer, &memoryRequirements);
    uint32_t memoryTypeIndex;
    FindMemoryType(vkPhysicalDevice, memoryRequirements.memoryTypeBits, msemoryPropertyFlags, &memoryTypeIndex);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    result = vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pDeviceMemory);
    TryThrowVulkanError(result);
    result = vkBindBufferMemory(vkDevice, *pBuffer, *pDeviceMemory, 0);
    TryThrowVulkanError(result);
}
void DestroyBuffer(VkDevice vkDevice, VkBuffer vkBuffer, VkDeviceMemory deviceMemory)
{
    vkFreeMemory(vkDevice, deviceMemory, NULL);
    vkDestroyBuffer(vkDevice, vkBuffer, NULL);
}

void CreateVertexBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkDeviceSize vertexBufferSize, void *vertices, VkBuffer *pVertexBuffer, VkDeviceMemory *pVertexBufferMemory)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void *pData;
    VkResult result = vkMapMemory(vkDevice, stagingBufferMemory, 0, vertexBufferSize, 0, &pData);
    TryThrowVulkanError(result);
    memcpy(pData, vertices, vertexBufferSize);
    vkUnmapMemory(vkDevice, stagingBufferMemory);
    CreateBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pVertexBuffer, pVertexBufferMemory);
    TryThrowVulkanError(result);
    CopyVkBuffer(graphicVkCommandPool, vkDevice, vkGraphicQueue, stagingBuffer, *pVertexBuffer, vertexBufferSize);

    vkDestroyBuffer(vkDevice, stagingBuffer, NULL);
    vkFreeMemory(vkDevice, stagingBufferMemory, NULL);
}
void DestroyVertexBuffer(VkDevice vkDevice, VkBuffer vertexBuffer, VkDeviceMemory vertexBufferMemory)
{
    vkDestroyBuffer(vkDevice, vertexBuffer, NULL);
    vkFreeMemory(vkDevice, vertexBufferMemory, NULL);
}

void FindMemoryType(VkPhysicalDevice vkPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex)
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

void FindDepthFormat(VkPhysicalDevice vkPhysicalDevice, VkFormat *pDepthFormat)
{
    uint32_t candidatesCount = 3;
    VkFormat *candidates = (VkFormat[]){VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    FindSupportedFormat(vkPhysicalDevice, candidates, candidatesCount, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, pDepthFormat);
}

void CreateImageView(VkDevice vkDevice, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView)
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
    TryThrowVulkanError(result);
}

void CreateGraphicImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage)
{
    pGraphicImage->vkFormat = vkFormat;
    CreateImage(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, &pGraphicImage->vkImage, &pGraphicImage->vkDeviceMemory);
    CreateImageView(vkDevice, pGraphicImage->vkImage, pGraphicImage->vkFormat, vkImageAspectFlags, &pGraphicImage->vkImageView);
}

void DestroyGraphicImage(VkDevice vkDevice, GraphicImage graphicImage)
{
    vkDestroyImageView(vkDevice, graphicImage.vkImageView, NULL);
    vkDestroyImage(vkDevice, graphicImage.vkImage, NULL);
    vkFreeMemory(vkDevice, graphicImage.vkDeviceMemory, NULL);
}

void CreateVkShaderModule(VkDevice vkDevice, const char *filePath, VkShaderModule *pVkShaderModule)
{
    FILE *pFile = fopen(filePath, "rb");
    if (NULL == pFile)
    {
        printf("Failed to read file with path: %s\n", filePath);
    }
    else
    {
        fseek(pFile, 0, SEEK_END);
        size_t fileLength = ftell(pFile);
        rewind(pFile);

        uint32_t *pCode = TickernelMalloc(fileLength);
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
            VkShaderModule shaderModule;
            vkCreateShaderModule(vkDevice, &shaderModuleCreateInfo, NULL, pVkShaderModule);
            TickernelFree(pCode);
        }
        else
        {
            printf("Failed to read file codeSize:%zu fileLength:%zu\n", codeSize, fileLength);
        }
    }
}

void DestroyVkShaderModule(VkDevice vkDevice, VkShaderModule vkShaderModule)
{
    vkDestroyShaderModule(vkDevice, vkShaderModule, NULL);
}

// void AddModelToSubpass(VkDevice vkDevice, Subpass *pSubpass, uint32_t *pIndex)
// {
//     TickernelAddToCollection(pSubpass->models, ,pIndex)
//     if (NULL != pSubpass->removedIndexLinkedList.pHead)
//     {
//         uint32_t modelIndex = *(uint32_t *)pSubpass->removedIndexLinkedList.pHead->pData;
//         TickernelRemoveFromLinkedList(&pSubpass->removedIndexLinkedList);
//         *pIndex = modelIndex;
//         return;
//     }
//     else if (pSubpass->modelCount < pSubpass->vkDescriptorPoolCount * pSubpass->modelCountPerDescriptorPool)
//     {
//         *pIndex = pSubpass->modelCount;
//         pSubpass->modelCount++;
//         return;
//     }
//     else
//     {
//         uint32_t newVkDescriptorPoolCount = pSubpass->vkDescriptorPoolCount + 1;
//         VkDescriptorPool *newVkDescriptorPools = TickernelMalloc(sizeof(VkDescriptorPool) * newVkDescriptorPoolCount);
//         memcpy(newVkDescriptorPools, pSubpass->vkDescriptorPools, sizeof(VkDescriptorPool) * pSubpass->vkDescriptorPoolCount);
//         TickernelFree(pSubpass->vkDescriptorPools);
//         VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
//             .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
//             .pNext = NULL,
//             .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
//             .maxSets = pSubpass->modelCountPerDescriptorPool,
//             .poolSizeCount = pSubpass->vkDescriptorPoolSizeCount,
//             .pPoolSizes = pSubpass->vkDescriptorPoolSizes,
//         };
//         VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &newVkDescriptorPools[pSubpass->vkDescriptorPoolCount]);
//         TryThrowVulkanError(result);
//         pSubpass->vkDescriptorPoolCount = newVkDescriptorPoolCount;
//         pSubpass->vkDescriptorPools = newVkDescriptorPools;

//         SubpassModel *newModels = TickernelMalloc(sizeof(SubpassModel) * pSubpass->modelCountPerDescriptorPool * newVkDescriptorPoolCount);
//         memcpy(newModels, pSubpass->models, sizeof(SubpassModel) * pSubpass->modelCount);
//         TickernelFree(pSubpass->models);
//         pSubpass->models = newModels;

//         *pIndex = pSubpass->modelCount;
//         pSubpass->modelCount++;
//         return;
//     }
// }
// void RemoveModelFromSubpass(uint32_t index, Subpass *pSubpass)
// {
//     if (index == (pSubpass->modelCount - 1))
//     {
//         pSubpass->modelCount--;
//     }
//     else
//     {
//         TickernelAddToLinkedList(&pSubpass->removedIndexLinkedList, &index);
//     }
// }