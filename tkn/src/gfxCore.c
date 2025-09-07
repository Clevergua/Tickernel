#include "gfxCore.h"

void assertVkResult(VkResult vkResult)
{
    tknAssert(vkResult == VK_SUCCESS, "Vulkan error: %d", vkResult);
}

SpvReflectShaderModule createSpvReflectShaderModule(const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        tknError("Failed to open file: %s\n", filePath);
    }
    else
    {
        // File opened successfully
    }
    fseek(file, 0, SEEK_END);
    size_t shaderSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (shaderSize % 4 != 0)
    {
        fclose(file);
        tknError("Invalid SPIR-V file size: %s\n", filePath);
    }
    else
    {
        // Valid SPIR-V file size
    }
    void *shaderCode = tknMalloc(shaderSize);
    size_t bytesRead = fread(shaderCode, 1, shaderSize, file);

    fclose(file);

    if (bytesRead != shaderSize)
    {
        tknError("Failed to read entire file: %s\n", filePath);
    }
    else
    {
        // File read successfully
    }
    SpvReflectShaderModule spvReflectShaderModule;
    SpvReflectResult spvReflectResult = spvReflectCreateShaderModule(shaderSize, shaderCode, &spvReflectShaderModule);
    tknAssert(spvReflectResult == SPV_REFLECT_RESULT_SUCCESS, "Failed to reflect shader module: %s", filePath);
    tknFree(shaderCode);

    return spvReflectShaderModule;
}
void destroySpvReflectShaderModule(SpvReflectShaderModule *pSpvReflectShaderModule)
{
    spvReflectDestroyShaderModule(pSpvReflectShaderModule);
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
        else
        {
            // Memory type doesn't match requirements
        }
    }
    tknError("Failed to get suitable memory type!");
    return UINT32_MAX;
}


void clearBindingPtrHashSet(GfxContext *pGfxContext, TknHashSet bindingPtrHashSet)
{
    for (uint32_t i = 0; i < bindingPtrHashSet.capacity; i++)
    {
        TknListNode *node = bindingPtrHashSet.nodePtrs[i];
        while (node)
        {
            Binding *pBinding = *(Binding **)node->data;
            InputBinding inputBinding = {
                .vkDescriptorType = pBinding->vkDescriptorType,
                .inputBindingUnion = {0},
                .binding = pBinding->binding,
            };
            updateMaterialPtr(pGfxContext, pBinding->pMaterial, 1, &inputBinding);
            node = node->nextNodePtr;
        }
    }
}

void createVkImage(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, VkImage *pVkImage, VkDeviceMemory *pVkDeviceMemory, VkImageView *pVkImageView)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGfxContext->vkPhysicalDevice;
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
    assertVkResult(vkCreateImage(vkDevice, &imageCreateInfo, NULL, pVkImage));
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, *pVkImage, &memoryRequirements);
    uint32_t memoryTypeIndex = getMemoryTypeIndex(vkPhysicalDevice, memoryRequirements.memoryTypeBits, vkMemoryPropertyFlags);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    assertVkResult(vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pVkDeviceMemory));
    assertVkResult(vkBindImageMemory(vkDevice, *pVkImage, *pVkDeviceMemory, 0));

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
        .image = *pVkImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = vkFormat,
        .components = components,
        .subresourceRange = subresourceRange,
    };
    assertVkResult(vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, pVkImageView));
}
void destroyVkImage(GfxContext *pGfxContext, VkImage vkImage, VkDeviceMemory vkDeviceMemory, VkImageView vkImageView)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkDestroyImageView(vkDevice, vkImageView, NULL);
    vkDestroyImage(vkDevice, vkImage, NULL);
    vkFreeMemory(vkDevice, vkDeviceMemory, NULL);
}

void createVkBuffer(GfxContext *pGfxContext, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer *pVkBuffer, VkDeviceMemory *pVkDeviceMemory)
{
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
    assertVkResult(vkCreateBuffer(vkDevice, &bufferCreateInfo, NULL, pVkBuffer));
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vkDevice, *pVkBuffer, &memoryRequirements);
    uint32_t memoryTypeIndex = getMemoryTypeIndex(vkPhysicalDevice, memoryRequirements.memoryTypeBits, memoryPropertyFlags);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    assertVkResult(vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pVkDeviceMemory));
    assertVkResult(vkBindBufferMemory(vkDevice, *pVkBuffer, *pVkDeviceMemory, 0));
}
void destroyVkBuffer(GfxContext *pGfxContext, VkBuffer vkBuffer, VkDeviceMemory vkDeviceMemory)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkDestroyBuffer(vkDevice, vkBuffer, NULL);
    vkFreeMemory(vkDevice, vkDeviceMemory, NULL);
}
VertexInputLayout *createVertexInputLayoutPtr(GfxContext *pGfxContext, uint32_t attributeCount, const char **names, uint32_t *sizes)
{
    VertexInputLayout *pVertexInputLayout = tknMalloc(sizeof(VertexInputLayout));
    const char **namesCopy = tknMalloc(sizeof(char *) * attributeCount);
    // Deep copy the strings, not just the pointers
    for (uint32_t i = 0; i < attributeCount; i++)
    {
        size_t nameLen = strlen(names[i]) + 1;
        char *nameCopy = tknMalloc(nameLen);
        memcpy(nameCopy, names[i], nameLen);
        namesCopy[i] = nameCopy;
    }
    uint32_t *sizesCopy = tknMalloc(sizeof(uint32_t) * attributeCount);
    memcpy(sizesCopy, sizes, sizeof(uint32_t) * attributeCount);
    uint32_t *offsets = tknMalloc(sizeof(uint32_t) * attributeCount);
    uint32_t stride = 0;
    for (uint32_t i = 0; i < attributeCount; i++)
    {
        offsets[i] = stride;
        stride += sizes[i];
    }
    *pVertexInputLayout = (VertexInputLayout){
        .attributeCount = attributeCount,
        .names = namesCopy,
        .sizes = sizesCopy,
        .offsets = offsets,
        .stride = stride,
        .referencePtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
    };
    tknAddToHashSet(&pGfxContext->vertexInputLayoutPtrHashSet, &pVertexInputLayout);
    return pVertexInputLayout;
}
void destroyVertexInputLayoutPtr(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout)
{
    tknAssert(0 == pVertexInputLayout->referencePtrHashSet.count, "Cannot destroy vertex input layout with meshes | instance attached!");
    tknRemoveFromHashSet(&pGfxContext->vertexInputLayoutPtrHashSet, &pVertexInputLayout);
    tknDestroyHashSet(pVertexInputLayout->referencePtrHashSet);
    
    // Free the deep-copied strings
    for (uint32_t i = 0; i < pVertexInputLayout->attributeCount; i++)
    {
        tknFree((void*)pVertexInputLayout->names[i]);
    }
    tknFree(pVertexInputLayout->names);
    tknFree(pVertexInputLayout->sizes);
    tknFree(pVertexInputLayout->offsets);
    tknFree(pVertexInputLayout);
}

DescriptorSet *createDescriptorSetPtr(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set)
{
    DescriptorSet *pDescriptorSet = tknMalloc(sizeof(DescriptorSet));

    TknHashSet materialPtrHashSet = tknCreateHashSet(sizeof(Material *));
    VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
    TknDynamicArray vkDescriptorPoolSizeDynamicArray = tknCreateDynamicArray(sizeof(VkDescriptorPoolSize), TKN_DEFAULT_COLLECTION_SIZE);
    uint32_t bindingCount = 0;
    VkDescriptorType *vkDescriptorTypes = NULL;

    for (uint32_t spvReflectShaderModuleIndex = 0; spvReflectShaderModuleIndex < spvReflectShaderModuleCount; spvReflectShaderModuleIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[spvReflectShaderModuleIndex];
        for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
            if (set == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    if (pSpvReflectDescriptorBinding->binding < bindingCount)
                    {
                        // Skip, already counted
                    }
                    else
                    {
                        bindingCount = pSpvReflectDescriptorBinding->binding + 1;
                    }
                }
                // Skip other sets.
                break;
            }
            else
            {
                //  Skip
                printf("Skipping descriptor set %d because of target set %d\n ", spvReflectDescriptorSet.set, set);
            }
        }
    }

    vkDescriptorTypes = tknMalloc(sizeof(VkDescriptorType) * bindingCount);
    VkDescriptorSetLayoutBinding *vkDescriptorSetLayoutBindings = tknMalloc(sizeof(VkDescriptorSetLayoutBinding) * bindingCount);
    for (uint32_t binding = 0; binding < bindingCount; binding++)
    {
        vkDescriptorTypes[binding] = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        vkDescriptorSetLayoutBindings[binding] = (VkDescriptorSetLayoutBinding){
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM,
            .descriptorCount = 0,
            .stageFlags = 0,
            .pImmutableSamplers = NULL,
        };
    }
    for (uint32_t moduleIndex = 0; moduleIndex < spvReflectShaderModuleCount; moduleIndex++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[moduleIndex];
        for (uint32_t setIndex = 0; setIndex < spvReflectShaderModule.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[setIndex];
            if (set == spvReflectDescriptorSet.set)
            {
                for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
                {
                    SpvReflectDescriptorBinding *pSpvReflectDescriptorBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                    uint32_t binding = pSpvReflectDescriptorBinding->binding;
                    if (VK_DESCRIPTOR_TYPE_MAX_ENUM == vkDescriptorSetLayoutBindings[binding].descriptorType)
                    {
                        VkDescriptorType vkDescriptorType = (VkDescriptorType)pSpvReflectDescriptorBinding->descriptor_type;

                        VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = {
                            .binding = binding,
                            .descriptorType = vkDescriptorType,
                            .descriptorCount = pSpvReflectDescriptorBinding->count,
                            .stageFlags = (VkShaderStageFlags)spvReflectShaderModule.shader_stage,
                            .pImmutableSamplers = NULL,
                        };
                        vkDescriptorSetLayoutBindings[binding] = vkDescriptorSetLayoutBinding;
                        vkDescriptorTypes[binding] = vkDescriptorType;

                        uint32_t poolSizeIndex;
                        for (poolSizeIndex = 0; poolSizeIndex < vkDescriptorPoolSizeDynamicArray.count; poolSizeIndex++)
                        {
                            VkDescriptorPoolSize *pVkDescriptorPoolSize = tknGetFromDynamicArray(&vkDescriptorPoolSizeDynamicArray, poolSizeIndex);
                            if (vkDescriptorType == pVkDescriptorPoolSize->type)
                            {
                                pVkDescriptorPoolSize->descriptorCount += vkDescriptorSetLayoutBinding.descriptorCount;
                                break;
                            }
                            else
                            {
                                // Skip
                            }
                        }
                        if (poolSizeIndex < vkDescriptorPoolSizeDynamicArray.count)
                        {
                            // Pool size already exists, skip adding
                        }
                        else
                        {
                            VkDescriptorPoolSize vkDescriptorPoolSize = {
                                .type = vkDescriptorSetLayoutBinding.descriptorType,
                                .descriptorCount = vkDescriptorSetLayoutBinding.descriptorCount,
                            };
                            tknAddToDynamicArray(&vkDescriptorPoolSizeDynamicArray, &vkDescriptorPoolSize);
                        }
                    }
                    else
                    {
                        tknAssert(vkDescriptorSetLayoutBindings[binding].descriptorType == (VkDescriptorType)pSpvReflectDescriptorBinding->descriptor_type, "Incompatible descriptor binding");
                        vkDescriptorSetLayoutBindings[binding].stageFlags |= (VkShaderStageFlags)spvReflectShaderModule.shader_stage;
                        vkDescriptorSetLayoutBindings[binding].descriptorCount = pSpvReflectDescriptorBinding->count > vkDescriptorSetLayoutBindings[binding].descriptorCount ? pSpvReflectDescriptorBinding->count : vkDescriptorSetLayoutBindings[binding].descriptorCount;
                    }
                }
                // Skip other sets.
                break;
            }
            else
            {
                // Skip
            }
        }
    }

    VkDevice vkDevice = pGfxContext->vkDevice;
    VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = bindingCount,
        .pBindings = vkDescriptorSetLayoutBindings,
    };
    assertVkResult(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout));
    tknFree(vkDescriptorSetLayoutBindings);

    *pDescriptorSet = (DescriptorSet){
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPoolSizeDynamicArray = vkDescriptorPoolSizeDynamicArray,
        .descriptorCount = bindingCount,
        .vkDescriptorTypes = vkDescriptorTypes,
        .materialPtrHashSet = materialPtrHashSet,
    };
    return pDescriptorSet;
}
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet)
{
    // Safely destroy all materials by repeatedly taking the first one
    while (pDescriptorSet->materialPtrHashSet.count > 0)
    {
        Material *pMaterial = NULL;
        for (uint32_t nodeIndex = 0; nodeIndex < pDescriptorSet->materialPtrHashSet.capacity; nodeIndex++)
        {
            TknListNode *pNode = pDescriptorSet->materialPtrHashSet.nodePtrs[nodeIndex];
            if (pNode)
            {
                pMaterial = *(Material **)pNode->data;
                break;
            }
            else
            {
                // No node at this index
            }
        }
        if (pMaterial)
        {
            destroyMaterialPtr(pGfxContext, pMaterial);
        }
        else
        {
            break; // Safety check to avoid infinite loop
        }
    }
    tknDestroyHashSet(pDescriptorSet->materialPtrHashSet);
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkDestroyDescriptorSetLayout(vkDevice, pDescriptorSet->vkDescriptorSetLayout, NULL);
    tknDestroyDynamicArray(pDescriptorSet->vkDescriptorPoolSizeDynamicArray);
    tknFree(pDescriptorSet->vkDescriptorTypes);
    tknFree(pDescriptorSet);
}

VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (uint32_t i = 0; i < candidateCount; i++)
    {
        VkFormat format = candidates[i];
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(pGfxContext->vkPhysicalDevice, format, &props);
        if (VK_IMAGE_TILING_LINEAR == tiling)
        {
            if ((props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else
            {
                // Linear tiling features don't match
            }
        }
        else if (VK_IMAGE_TILING_OPTIMAL == tiling)
        {
            if ((props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
            else
            {
                // Optimal tiling features don't match
            }
        }
        else
        {
            // Unknown tiling mode
        }
    }
    fprintf(stderr, "Error: No supported format found for the given requirements\n");
    return VK_FORMAT_MAX_ENUM;
}
