#include <graphicEngineCore.h>
static void CopyVkBuffer(GraphicEngine *pGraphicEngine, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer vkCommandBuffer;
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pGraphicEngine->graphicVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VkResult result = vkAllocateCommandBuffers(pGraphicEngine->vkDevice, &vkCommandBufferAllocateInfo, pGraphicEngine->graphicVkCommandBuffers);
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
    vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);

    VkBufferCopy copyRegion;
    copyRegion.size = size;
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
    vkQueueSubmit(pGraphicEngine->vkGraphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(pGraphicEngine->vkGraphicQueue);
    vkFreeCommandBuffers(pGraphicEngine->vkDevice, pGraphicEngine->graphicVkCommandPool, 1, &vkCommandBuffer);
}
static void CreateSubpassModel(GraphicEngine *pGraphicEngine, SubpassModelCreateInfo subpassModelCreateInfo, ModelGroup *pModelGroup, uint32_t modelIndex)
{
    SubpassModel *pSubpassModel = &pModelGroup->subpassModels[modelIndex];

    // Create vertex buffer
    pSubpassModel->vertexCount = subpassModelCreateInfo.vertexCount;
    VkDeviceSize vertexBufferSize = subpassModelCreateInfo.vertexSize * subpassModelCreateInfo.vertexCount;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(pGraphicEngine, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    void *pData;
    VkResult result = vkMapMemory(pGraphicEngine->vkDevice, stagingBufferMemory, 0, vertexBufferSize, 0, &pData);
    TryThrowVulkanError(result);
    memcpy(pData, subpassModelCreateInfo.vertices, vertexBufferSize);
    vkUnmapMemory(pGraphicEngine->vkDevice, stagingBufferMemory);
    CreateBuffer(pGraphicEngine, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pSubpassModel->vertexBuffer, &pSubpassModel->vertexBufferMemory);
    TryThrowVulkanError(result);
    CopyVkBuffer(pGraphicEngine, stagingBuffer, *&pSubpassModel->vertexBuffer, vertexBufferSize);
    DestroyBuffer(pGraphicEngine->vkDevice, stagingBuffer, stagingBufferMemory);

    // Create object uniform buffer
    CreateBuffer(pGraphicEngine, subpassModelCreateInfo.modelUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pSubpassModel->modelUniformBuffer, &pSubpassModel->modelUniformBufferMemory);
    vkMapMemory(pGraphicEngine->vkDevice, *&pSubpassModel->modelUniformBufferMemory, 0, subpassModelCreateInfo.modelUniformBufferSize, 0, pSubpassModel->modelUniformBufferMapped);
    
    // Create vkDescriptorSet
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = pModelGroup->vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &subpassModelCreateInfo.vkDescriptorSetLayout,
    };
    result = vkAllocateDescriptorSets(pGraphicEngine->vkDevice, &descriptorSetAllocateInfo, &pSubpassModel->vkDescriptorSet);

    VkDescriptorBufferInfo globalDescriptorBufferInfo = {
        .buffer = pGraphicEngine->globalUniformBuffer,
        .offset = 0,
        .range = sizeof(GlobalUniformBuffer),
    };
    VkDescriptorBufferInfo objectDescriptorBufferInfo = {
        .buffer = pSubpassModel->modelUniformBuffer,
        .offset = 0,
        .range = sizeof(GeometrySubpassModelUniformBuffer),
    };

    VkWriteDescriptorSet descriptorWrites[2] = {
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = pSubpassModel->vkDescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &globalDescriptorBufferInfo,
            .pTexelBufferView = NULL,
        },
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = pSubpassModel->vkDescriptorSet,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &objectDescriptorBufferInfo,
            .pTexelBufferView = NULL,
        },
    };
    vkUpdateDescriptorSets(pGraphicEngine->vkDevice, 2, subpassModelCreateInfo.vkWriteDescriptorSets, 0, NULL);
}
static void DestroySubpassModel(GraphicEngine *pGraphicEngine, ModelGroup modelGroup, uint32_t modelIndex)
{
    SubpassModel *pSubpassModel = &modelGroup.subpassModels[modelIndex];
    VkResult result = vkFreeDescriptorSets(pGraphicEngine->vkDevice, modelGroup.vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
    TryThrowVulkanError(result);
    DestroyBuffer(pGraphicEngine->vkDevice, pSubpassModel->modelUniformBuffer, pSubpassModel->modelUniformBufferMemory);
    DestroyBuffer(pGraphicEngine->vkDevice, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);
}
void CreateBuffer(GraphicEngine *pGraphicEngine, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags msemoryPropertyFlags, VkBuffer *pBuffer, VkDeviceMemory *pDeviceMemory)
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
    result = vkCreateBuffer(pGraphicEngine->vkDevice, &bufferCreateInfo, NULL, pBuffer);
    TryThrowVulkanError(result);
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(pGraphicEngine->vkDevice, *pBuffer, &memoryRequirements);
    uint32_t memoryTypeIndex;
    FindMemoryType(pGraphicEngine, memoryRequirements.memoryTypeBits, msemoryPropertyFlags, &memoryTypeIndex);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    result = vkAllocateMemory(pGraphicEngine->vkDevice, &memoryAllocateInfo, NULL, pDeviceMemory);
    TryThrowVulkanError(result);
    result = vkBindBufferMemory(pGraphicEngine->vkDevice, *pBuffer, *pDeviceMemory, 0);
    TryThrowVulkanError(result);
}

void DestroyBuffer(VkDevice vkDevice, VkBuffer vkBuffer, VkDeviceMemory deviceMemory)
{
    vkFreeMemory(vkDevice, deviceMemory, NULL);
    vkDestroyBuffer(vkDevice, vkBuffer, NULL);
}
static void CreateImage(GraphicEngine *pGraphicEngine, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage *pVkImage, VkDeviceMemory *pVkDeviceMemory)
{
    VkResult result = VK_SUCCESS;

    VkDevice vkDevice = pGraphicEngine->vkDevice;

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
    FindMemoryType(pGraphicEngine, memoryRequirements.memoryTypeBits, vkMemoryPropertyFlags, &memoryTypeIndex);

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

static void FindSupportedFormat(GraphicEngine *pGraphicEngine, VkFormat *candidates, uint32_t candidatesCount, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat *vkFormat)
{
    VkResult result = VK_SUCCESS;
    VkPhysicalDevice vkPhysicalDevice = pGraphicEngine->vkPhysicalDevice;

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

void FindMemoryType(GraphicEngine *pGraphicEngine, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex)
{
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(pGraphicEngine->vkPhysicalDevice, &physicalDeviceMemoryProperties);
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

void FindDepthFormat(GraphicEngine *pGraphicEngine, VkFormat *pDepthFormat)
{
    uint32_t candidatesCount = 3;
    VkFormat *candidates = (VkFormat[]){VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    FindSupportedFormat(pGraphicEngine, candidates, candidatesCount, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, pDepthFormat);
}

void CreateImageView(GraphicEngine *pGraphicEngine, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView)
{
    VkDevice vkDevice = pGraphicEngine->vkDevice;
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

void CreateGraphicImage(GraphicEngine *pGraphicEngine, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage)
{
    pGraphicImage->vkFormat = vkFormat;
    CreateImage(pGraphicEngine, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, &pGraphicImage->vkImage, &pGraphicImage->vkDeviceMemory);
    CreateImageView(pGraphicEngine, pGraphicImage->vkImage, pGraphicImage->vkFormat, vkImageAspectFlags, &pGraphicImage->vkImageView);
}

void DestroyGraphicImage(GraphicEngine *pGraphicEngine, GraphicImage graphicImage)
{
    VkDevice vkDevice = pGraphicEngine->vkDevice;
    vkDestroyImageView(vkDevice, graphicImage.vkImageView, NULL);
    vkDestroyImage(vkDevice, graphicImage.vkImage, NULL);
    vkFreeMemory(vkDevice, graphicImage.vkDeviceMemory, NULL);
}

void CreateVkShaderModule(GraphicEngine *pGraphicEngine, const char *filePath, VkShaderModule *pVkShaderModule)
{
    printf("Path: %s\n", filePath);
    FILE *pFile = fopen(filePath, "rb");
    fseek(pFile, 0, SEEK_END);
    size_t fileLength = ftell(pFile);
    rewind(pFile);

    uint32_t *pCode = calloc(fileLength, 1);
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
        vkCreateShaderModule(pGraphicEngine->vkDevice, &shaderModuleCreateInfo, NULL, pVkShaderModule);
        free(pCode);
    }
    else
    {
        printf("Failed to read file codeSize:%zu fileLength:%zu\n", codeSize, fileLength);
    }
}

void DestroyVkShaderModule(GraphicEngine *pGraphicEngine, VkShaderModule vkShaderModule)
{
    vkDestroyShaderModule(pGraphicEngine->vkDevice, vkShaderModule, NULL);
}

void CreateModelGroup(GraphicEngine *pGraphicEngine, Subpass *pSubpass, ModelGroup *pModelGroup)
{
    pModelGroup->modelCount = 0;
    pModelGroup->subpassModels = TickernelMalloc(sizeof(SubpassModel) * pSubpass->modelCountPerGroup);
    pModelGroup->pRemovedIndexLinkedList = NULL;

    VkDescriptorPoolSize poolSizes[MAX_VK_DESCRIPTOR_TPYE];
    for (uint32_t vkDescriptorType = 0; vkDescriptorType < MAX_VK_DESCRIPTOR_TPYE; vkDescriptorType++)
    {
        poolSizes[vkDescriptorType] = (VkDescriptorPoolSize){
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = pSubpass->modelCountPerGroup * pSubpass->vkDescriptorTypeToCount[vkDescriptorType],
        };
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = pSubpass->modelCountPerGroup,
        .poolSizeCount = 1,
        .pPoolSizes = poolSizes,
    };
    VkResult result = vkCreateDescriptorPool(pGraphicEngine->vkDevice, &descriptorPoolCreateInfo, NULL, &pModelGroup->vkDescriptorPool);
    TryThrowVulkanError(result);
}
static void DestroyModelGroup(GraphicEngine *pGraphicEngine, ModelGroup modelGroup)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t subpassIndex = 0;
    Subpass *pSubpass = &pDeferredRenderPass->subpasses[subpassIndex];

    for (uint32_t modelIndex = 0; modelIndex < modelGroup.modelCount; modelIndex++)
    {
        DestroySubpassModel(pGraphicEngine, modelGroup, modelIndex);
    }

    vkDestroyDescriptorPool(pGraphicEngine->vkDevice, modelGroup.vkDescriptorPool, NULL);
    TickernelFree(modelGroup.subpassModels);
}

void AddModelToSubpass(GraphicEngine *pGraphicEngine, SubpassModelCreateInfo subpassModelCreateInfo, Subpass *pSubpass, uint32_t *pGroupIndex, uint32_t *pModelIndex)
{
    for (uint32_t groupIndex = 0; groupIndex < pSubpass->modelGroupCount; groupIndex++)
    {
        ModelGroup *pModelGroup = &pSubpass->modelGroups[groupIndex];
        if (NULL != pModelGroup->pRemovedIndexLinkedList)
        {
            uint32_t modelIndex = pModelGroup->pRemovedIndexLinkedList->data;
            Uint32Node *pNode = pModelGroup->pRemovedIndexLinkedList;
            pModelGroup->pRemovedIndexLinkedList = pModelGroup->pRemovedIndexLinkedList->pNext;
            TickernelFree(pNode);
            CreateSubpassModel(pGraphicEngine, subpassModelCreateInfo, pModelGroup, modelIndex);
            *pGroupIndex = groupIndex;
            *pModelIndex = modelIndex;
            return;
        }
        else if (pModelGroup->modelCount < pSubpass->modelCountPerGroup)
        {
            uint32_t modelIndex = pModelGroup->modelCount;
            CreateSubpassModel(pGraphicEngine, subpassModelCreateInfo, pModelGroup, modelIndex);
            pModelGroup->modelCount++;
            *pGroupIndex = groupIndex;
            *pModelIndex = modelIndex;
            return;
        }
        else
        {
            // continue;
        }
    }

    // Create group & create model
    if (pSubpass->modelGroupCount < pSubpass->maxModelGroupCount)
    {
        uint32_t groupIndex = pSubpass->modelGroupCount;
        // create model group
        CreateModelGroup(pGraphicEngine, pSubpass, &pSubpass->modelGroups[groupIndex]);
        pSubpass->modelGroupCount++;
        // create model
        ModelGroup *pModelGroup = &pSubpass->modelGroups[groupIndex];
        uint32_t modelIndex = pModelGroup->modelCount;
        CreateSubpassModel(pGraphicEngine, subpassModelCreateInfo, pModelGroup, modelIndex);
        pModelGroup->modelCount++;

        *pGroupIndex = groupIndex;
        *pModelIndex = modelIndex;
        return;
    }
    else
    {
        uint32_t oldCount = pSubpass->maxModelGroupCount;
        ModelGroup *oldModelGroups = pSubpass->modelGroups;
        pSubpass->maxModelGroupCount = pSubpass->maxModelGroupCount * 2;
        pSubpass->modelGroups = TickernelMalloc(sizeof(ModelGroup) * pSubpass->maxModelGroupCount);
        memcpy(pSubpass->modelGroups, oldModelGroups, oldCount * sizeof(ModelGroup));
        TickernelFree(oldModelGroups);
        // create model group
        uint32_t groupIndex = pSubpass->modelGroupCount;
        CreateModelGroup(pGraphicEngine, pSubpass, &pSubpass->modelGroups[groupIndex]);
        pSubpass->modelGroupCount++;
        // create model
        ModelGroup *pModelGroup = &pSubpass->modelGroups[groupIndex];
        uint32_t modelIndex = pModelGroup->modelCount;
        CreateSubpassModel(pGraphicEngine, subpassModelCreateInfo, pModelGroup, modelIndex);
        pModelGroup->modelCount++;

        *pGroupIndex = groupIndex;
        *pModelIndex = modelIndex;
        return;
    }
}
void RemoveModelFromSubpass(GraphicEngine *pGraphicEngine, uint32_t groupIndex, uint32_t modelIndex, Subpass *pSubpass)
{
    ModelGroup *pModelGroup = &pSubpass->modelGroups[groupIndex];

    Uint32Node *newNode = TickernelMalloc(sizeof(Uint32Node));
    newNode->data = modelIndex;
    newNode->pNext = pModelGroup->pRemovedIndexLinkedList;
    pModelGroup->pRemovedIndexLinkedList = newNode;
    DestroySubpassModel(pGraphicEngine, *pModelGroup, modelIndex);
}