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
    fseek(file, 0, SEEK_END);
    size_t shaderSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (shaderSize % 4 != 0)
    {
        fclose(file);
        tknError("Invalid SPIR-V file size: %s\n", filePath);
    }
    void *shaderCode = tknMalloc(shaderSize);
    size_t bytesRead = fread(shaderCode, 1, shaderSize, file);

    fclose(file);

    if (bytesRead != shaderSize)
    {
        tknError("Failed to read entire file: %s\n", filePath);
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

size_t getSizeOfVkFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_UNDEFINED:
        return 0;
    case VK_FORMAT_R16_UINT:
    case VK_FORMAT_R16_SINT:
    case VK_FORMAT_R16_SFLOAT:
        return 2;
    case VK_FORMAT_R16G16_UINT:
    case VK_FORMAT_R16G16_SINT:
    case VK_FORMAT_R16G16_SFLOAT:
    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32_SINT:
    case VK_FORMAT_R32_SFLOAT:
        return 4;
    case VK_FORMAT_R16G16B16_UINT:
    case VK_FORMAT_R16G16B16_SINT:
    case VK_FORMAT_R16G16B16_SFLOAT:
        return 6;
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
    case VK_FORMAT_R32G32B32_UINT:
    case VK_FORMAT_R32G32B32_SINT:
    case VK_FORMAT_R32G32B32_SFLOAT:
        return 12;
    case VK_FORMAT_R32G32B32A32_UINT:
    case VK_FORMAT_R32G32B32A32_SINT:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
    case VK_FORMAT_R64G64_UINT:
    case VK_FORMAT_R64G64_SINT:
    case VK_FORMAT_R64G64_SFLOAT:
        return 16;
    case VK_FORMAT_R64G64B64_UINT:
    case VK_FORMAT_R64G64B64_SINT:
    case VK_FORMAT_R64G64B64_SFLOAT:
        return 24;
    case VK_FORMAT_R64G64B64A64_UINT:
    case VK_FORMAT_R64G64B64A64_SINT:
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return 32;
    default:
        tknError("getSizeOfVkFormat: unsupported VkFormat %d", format);
        return 0;
    }
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

static void createVkImage(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, VkImage *pVkImage, VkDeviceMemory *pVkDeviceMemory, VkImageView *pVkImageView)
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
static void destroyVkImage(GfxContext *pGfxContext, VkImage vkImage, VkDeviceMemory vkDeviceMemory, VkImageView vkImageView)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkDestroyImageView(vkDevice, vkImageView, NULL);
    vkDestroyImage(vkDevice, vkImage, NULL);
    vkFreeMemory(vkDevice, vkDeviceMemory, NULL);
}
static void clearBindingPtrHashSet(GfxContext *pGfxContext, TknHashSet bindingPtrHashSet)
{
    for (uint32_t i = 0; i < bindingPtrHashSet.capacity; i++)
    {
        TknListNode *node = bindingPtrHashSet.nodePtrs[i];
        while (node)
        {
            Binding *pBinding = (Binding *)node->pointer;
            Binding binding = *pBinding;
            binding.bindingUnion = getNullBindingUnion(pBinding->vkDescriptorType);
            updateBindings(pGfxContext, 1, &binding);
            node = node->nextNodePtr;
        }
    }
}
static void copyBuffer(GfxContext *pGfxContext, VkBuffer srcVkBuffer, VkBuffer dstVkBuffer, VkDeviceSize size)
{
    VkDevice vkDevice = pGfxContext->vkDevice;

    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = pGfxContext->gfxVkCommandPool,
        .commandBufferCount = 1};

    VkCommandBuffer vkCommandBuffer;
    assertVkResult(vkAllocateCommandBuffers(vkDevice, &vkCommandBufferAllocateInfo, &vkCommandBuffer));

    VkCommandBufferBeginInfo vkCommandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    assertVkResult(vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo));

    VkBufferCopy vkBufferCopy = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size};
    vkCmdCopyBuffer(vkCommandBuffer, srcVkBuffer, dstVkBuffer, 1, &vkBufferCopy);
    assertVkResult(vkEndCommandBuffer(vkCommandBuffer));

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkCommandBuffer};

    assertVkResult(vkQueueSubmit(pGfxContext->vkGfxQueue, 1, &submitInfo, VK_NULL_HANDLE));
    assertVkResult(vkQueueWaitIdle(pGfxContext->vkGfxQueue));

    vkFreeCommandBuffers(vkDevice, pGfxContext->gfxVkCommandPool, 1, &vkCommandBuffer);
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

Attachment *createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkImageAspectFlags vkImageAspectFlags, float scaler)
{
    SwapchainAttachment *pswapchainAttachment = &pGfxContext->pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pswapchainAttachment->swapchainExtent.width * scaler),
        .height = (uint32_t)(pswapchainAttachment->swapchainExtent.height * scaler),
        .depth = 1,
    };

    VkImage vkImage;
    VkDeviceMemory vkDeviceMemory;
    VkImageView vkImageView;
    createVkImage(pGfxContext, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkImageAspectFlags, &vkImage, &vkDeviceMemory, &vkImageView);
    DynamicAttachment dynamicAttachment = {
        .vkImage = vkImage,
        .vkDeviceMemory = vkDeviceMemory,
        .vkImageView = vkImageView,
        .vkImageUsageFlags = vkImageUsageFlags,
        .vkImageAspectFlags = vkImageAspectFlags,
        .scaler = scaler,
        .bindingPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
    };
    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_DYNAMIC,
        .attachmentUnion.dynamicAttachment = dynamicAttachment,
        .vkFormat = vkFormat,
        .renderPassPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
    };
    tknAddToDynamicArray(&pGfxContext->dynamicAttachmentPtrDynamicArray, &pAttachment);
    return pAttachment;
}
void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment)
{
    tknAssert(ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType, "Attachment type mismatch!");
    tknAssert(0 == pAttachment->renderPassPtrHashSet.count, "Cannot destroy dynamic attachment with render passes attached!");
    tknRemoveFromDynamicArray(&pGfxContext->dynamicAttachmentPtrDynamicArray, &pAttachment);
    tknDestroyHashSet(pAttachment->attachmentUnion.dynamicAttachment.bindingPtrHashSet);
    tknDestroyHashSet(pAttachment->renderPassPtrHashSet);
    DynamicAttachment dynamicAttachment = pAttachment->attachmentUnion.dynamicAttachment;
    destroyVkImage(pGfxContext, dynamicAttachment.vkImage, dynamicAttachment.vkDeviceMemory, dynamicAttachment.vkImageView);
    tknFree(pAttachment);
}
void resizeDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment)
{
    tknAssert(ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType, "Attachment type mismatch!");
    DynamicAttachment dynamicAttachment = pAttachment->attachmentUnion.dynamicAttachment;
    SwapchainAttachment *pswapchainAttachment = &pGfxContext->pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    VkExtent3D vkExtent3D = {
        .width = (uint32_t)(pswapchainAttachment->swapchainExtent.width * dynamicAttachment.scaler),
        .height = (uint32_t)(pswapchainAttachment->swapchainExtent.height * dynamicAttachment.scaler),
        .depth = 1,
    };
    destroyVkImage(pGfxContext, dynamicAttachment.vkImage, dynamicAttachment.vkDeviceMemory, dynamicAttachment.vkImageView);
    createVkImage(pGfxContext, vkExtent3D, pAttachment->vkFormat, VK_IMAGE_TILING_OPTIMAL, dynamicAttachment.vkImageUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, dynamicAttachment.vkImageAspectFlags, &dynamicAttachment.vkImage, &dynamicAttachment.vkDeviceMemory, &dynamicAttachment.vkImageView);

    for (uint32_t i = 0; i < pAttachment->attachmentUnion.dynamicAttachment.bindingPtrHashSet.capacity; i++)
    {
        TknListNode *node = pAttachment->attachmentUnion.dynamicAttachment.bindingPtrHashSet.nodePtrs[i];
        while (node)
        {
            Binding *pBinding = (Binding *)node->pointer;
            updateInputAttachmentBindings(pGfxContext, 1, pBinding);
            node = node->nextNodePtr;
        }
    }
}
Attachment *createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height)
{
    Attachment *pAttachment = tknMalloc(sizeof(Attachment));
    VkExtent3D vkExtent3D = {
        .width = width,
        .height = height,
        .depth = 1,
    };

    VkImage vkImage;
    VkDeviceMemory vkDeviceMemory;
    VkImageView vkImageView;
    createVkImage(pGfxContext, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkImageAspectFlags, &vkImage, &vkDeviceMemory, &vkImageView);

    FixedAttachment fixedAttachment = {
        .vkImage = vkImage,
        .vkDeviceMemory = vkDeviceMemory,
        .vkImageView = vkImageView,
        .width = width,
        .height = height,
        .bindingPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
    };

    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_FIXED,
        .attachmentUnion.fixedAttachment = fixedAttachment,
        .vkFormat = vkFormat,
        .renderPassPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
    };
    return pAttachment;
}
void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment)
{
    tknAssert(ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType, "Attachment type mismatch!");
    tknAssert(0 == pAttachment->renderPassPtrHashSet.count, "Cannot destroy fixed attachment with render passes attached!");
    tknDestroyHashSet(pAttachment->renderPassPtrHashSet);
    FixedAttachment fixedAttachment = pAttachment->attachmentUnion.fixedAttachment;
    destroyVkImage(pGfxContext, fixedAttachment.vkImage, fixedAttachment.vkDeviceMemory, fixedAttachment.vkImageView);
    tknDestroyHashSet(fixedAttachment.bindingPtrHashSet);
    tknFree(pAttachment);
}
Attachment *getSwapchainAttachmentPtr(GfxContext *pGfxContext)
{
    return pGfxContext->pSwapchainAttachment;
}

Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags)
{
    Image *pImage = tknMalloc(sizeof(Image));

    VkImage vkImage;
    VkImageView vkImageView;
    VkDeviceMemory vkDeviceMemory;
    createVkImage(pGfxContext, vkExtent3D, vkFormat, vkImageTiling, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, &vkImage, &vkDeviceMemory, &vkImageView);
    Image image = {
        .vkImage = vkImage,
        .vkDeviceMemory = vkDeviceMemory,
        .vkImageView = vkImageView,
        .bindingPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
    };
    *pImage = image;

    return pImage;
}
void destroyImagePtr(GfxContext *pGfxContext, Image *pImage)
{
    clearBindingPtrHashSet(pGfxContext, pImage->bindingPtrHashSet);
    tknDestroyHashSet(pImage->bindingPtrHashSet);
    destroyVkImage(pGfxContext, pImage->vkImage, pImage->vkDeviceMemory, pImage->vkImageView);
    tknFree(pImage);
}

UniformBuffer *createUniformBufferPtr(GfxContext *pGfxContext, VkDeviceSize vkDeviceSize)
{
    UniformBuffer *pUniformBuffer = tknMalloc(sizeof(UniformBuffer));

    VkBuffer vkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
    void *mapped = NULL;
    TknHashSet bindingPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE);
    VkDeviceSize size = vkDeviceSize;

    createVkBuffer(pGfxContext, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vkBuffer, &vkDeviceMemory);
    VkDevice vkDevice = pGfxContext->vkDevice;
    assertVkResult(vkMapMemory(vkDevice, vkDeviceMemory, 0, size, 0, &mapped));
    *pUniformBuffer = (UniformBuffer){
        .vkBuffer = vkBuffer,
        .vkDeviceMemory = vkDeviceMemory,
        .mapped = mapped,
        .bindingPtrHashSet = bindingPtrHashSet,
        .size = size,
    };

    return pUniformBuffer;
}
void destroyUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer)
{
    clearBindingPtrHashSet(pGfxContext, pUniformBuffer->bindingPtrHashSet);

    tknDestroyHashSet(pUniformBuffer->bindingPtrHashSet);
    destroyVkBuffer(pGfxContext, pUniformBuffer->vkBuffer, pUniformBuffer->vkDeviceMemory);
    tknFree(pUniformBuffer);
}
void updateUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer, const void *data, VkDeviceSize vkDeviceSize)
{
    tknAssert(pUniformBuffer->vkDeviceMemory != VK_NULL_HANDLE, "Mapped buffer memory is not allocated!");
    tknAssert(vkDeviceSize <= pUniformBuffer->size, "Data size exceeds mapped buffer size!");
    memcpy(pUniformBuffer->mapped, data, vkDeviceSize);
}

Material *createMaterialPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet)
{
    Material *pMaterial = tknMalloc(sizeof(Material));
    VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
    uint32_t descriptorCount = pDescriptorSet->descriptorCount;
    Binding *bindings = tknMalloc(sizeof(Binding) * descriptorCount);
    VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;

    for (uint32_t descriptorIndex = 0; descriptorIndex < descriptorCount; descriptorIndex++)
    {
        VkDescriptorType vkDescriptorType = pDescriptorSet->vkDescriptorTypes[descriptorIndex];
        bindings[descriptorIndex] = (Binding){
            .vkDescriptorType = vkDescriptorType,
            .bindingUnion = getNullBindingUnion(vkDescriptorType),
            .pMaterial = pMaterial,
            .binding = descriptorIndex,
        };
    }
    VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = pDescriptorSet->vkDescriptorPoolSizeDynamicArray.count,
        .pPoolSizes = pDescriptorSet->vkDescriptorPoolSizeDynamicArray.array,
        .maxSets = 1,
    };
    VkDevice vkDevice = pGfxContext->vkDevice;
    assertVkResult(vkCreateDescriptorPool(vkDevice, &vkDescriptorPoolCreateInfo, NULL, &vkDescriptorPool));

    VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pDescriptorSet->vkDescriptorSetLayout,
    };
    assertVkResult(vkAllocateDescriptorSets(vkDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet));
    *pMaterial = (Material){
        .vkDescriptorSet = vkDescriptorSet,
        .bindingCount = descriptorCount,
        .bindings = bindings,
        .vkDescriptorPool = vkDescriptorPool,
        .pDescriptorSet = pDescriptorSet,
    };
    tknAddToDynamicArray(&pDescriptorSet->materialPtrDynamicArray, &pMaterial);
    return pMaterial;
}

VertexInputLayout *createVertexInputLayoutPtr(uint32_t attributeCount, const char **names, uint32_t *sizes)
{
    VertexInputLayout *pVertexInputLayout = tknMalloc(sizeof(VertexInputLayout));
    const char **namesCopy = tknMalloc(sizeof(char *) * attributeCount);
    memcpy(namesCopy, names, sizeof(char *) * attributeCount);
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
    return pVertexInputLayout;
}
void destroyVertexInputLayoutPtr(VertexInputLayout *pVertexInputLayout)
{
    tknAssert(pVertexInputLayout->referencePtrHashSet.count == 0, "Cannot destroy vertex input layout with meshes | instance attached!");
    tknDestroyHashSet(pVertexInputLayout->referencePtrHashSet);
    tknFree(pVertexInputLayout->names);
    tknFree(pVertexInputLayout->sizes);
    tknFree(pVertexInputLayout->offsets);
    tknFree(pVertexInputLayout);
}

DescriptorSet *createDescriptorSetPtr(GfxContext *pGfxContext, uint32_t spvReflectShaderModuleCount, SpvReflectShaderModule *spvReflectShaderModules, uint32_t set)
{
    DescriptorSet *pDescriptorSet = tknMalloc(sizeof(DescriptorSet));
    TknDynamicArray materialPtrDynamicArray = tknCreateDynamicArray(sizeof(Material *), TKN_MIN_COLLECTION_SIZE);
    VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
    TknDynamicArray vkDescriptorPoolSizeDynamicArray = tknCreateDynamicArray(sizeof(VkDescriptorPoolSize), TKN_DEFAULT_COLLECTION_SIZE);
    uint32_t descriptorCount = 0;
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
                    if (pSpvReflectDescriptorBinding->binding < descriptorCount)
                    {
                        // Skip, already counted
                    }
                    else
                    {
                        descriptorCount = pSpvReflectDescriptorBinding->binding + 1;
                    }
                }
                // Skip other sets.
                break;
            }
            else
            {
                // Skip
                tknError("Skipping descriptor set %d because of target set %d\n ", spvReflectDescriptorSet.set, set);
            }
        }
    }

    vkDescriptorTypes = tknMalloc(sizeof(VkDescriptorType) * descriptorCount);
    VkDescriptorSetLayoutBinding *vkDescriptorSetLayoutBindings = tknMalloc(sizeof(VkDescriptorSetLayoutBinding) * descriptorCount);
    for (uint32_t binding = 0; binding < descriptorCount; binding++)
    {
        vkDescriptorTypes[binding] = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        vkDescriptorSetLayoutBindings[binding] = (VkDescriptorSetLayoutBinding){
            .binding = binding,
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
        .bindingCount = descriptorCount,
        .pBindings = vkDescriptorSetLayoutBindings,
    };
    assertVkResult(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout));
    tknFree(vkDescriptorSetLayoutBindings);

    *pDescriptorSet = (DescriptorSet){
        .materialPtrDynamicArray = materialPtrDynamicArray,
        .vkDescriptorSetLayout = vkDescriptorSetLayout,
        .vkDescriptorPoolSizeDynamicArray = vkDescriptorPoolSizeDynamicArray,
        .descriptorCount = descriptorCount,
        .vkDescriptorTypes = vkDescriptorTypes,
    };
    return pDescriptorSet;
}
void destroyDescriptorSetPtr(GfxContext *pGfxContext, DescriptorSet *pDescriptorSet)
{
    for (uint32_t i = 0; i < pDescriptorSet->materialPtrDynamicArray.count; i++)
    {
        Material *pMaterial = *(Material **)tknGetFromDynamicArray(&pDescriptorSet->materialPtrDynamicArray, i);
        destroyMaterialPtr(pGfxContext, pMaterial);
    }
    tknDestroyDynamicArray(pDescriptorSet->materialPtrDynamicArray);
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
        }
        else if (VK_IMAGE_TILING_OPTIMAL == tiling)
        {
            if ((props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
    }
    fprintf(stderr, "Error: No supported format found for the given requirements\n");
    return VK_FORMAT_MAX_ENUM;
}

void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    uint32_t width = UINT32_MAX;
    uint32_t height = UINT32_MAX;
    uint32_t attachmentCount = pRenderPass->attachmentCount;
    Attachment **attachmentPtrs = pRenderPass->attachmentPtrs;
    SwapchainAttachment *pSwapchainUnion = &pGfxContext->pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    uint32_t swapchainWidth = pSwapchainUnion->swapchainExtent.width;
    uint32_t swapchainHeight = pSwapchainUnion->swapchainExtent.height;
    for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
    {
        Attachment *pAttachment = attachmentPtrs[attachmentIndex];
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            if (UINT32_MAX == width && UINT32_MAX == height)
            {
                width = swapchainWidth;
                height = swapchainHeight;
            }
            else
            {
                tknAssert(width == swapchainWidth && height == swapchainHeight, "Swapchain attachment size mismatch!");
            }
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            DynamicAttachment dynamicUnion = pAttachment->attachmentUnion.dynamicAttachment;
            uint32_t dynamicWidth = swapchainWidth * dynamicUnion.scaler;
            uint32_t dynamicHeight = swapchainHeight * dynamicUnion.scaler;
            if (UINT32_MAX == width && UINT32_MAX == height)
            {
                width = dynamicWidth;
                height = dynamicHeight;
            }
            else
            {
                tknAssert(width == dynamicWidth && height == dynamicHeight, "Dynamic attachment size mismatch!");
            }
        }
        else
        {
            FixedAttachment fixedUnion = pAttachment->attachmentUnion.fixedAttachment;
            if (UINT32_MAX == width && UINT32_MAX == height)
            {
                width = fixedUnion.width;
                height = fixedUnion.height;
            }
            else
            {
                tknAssert(width == fixedUnion.width && height == fixedUnion.height, "Fixed attachment size mismatch!");
            }
        }
    }

    tknAssert(UINT32_MAX != width && UINT32_MAX != height, "No valid attachment found to determine framebuffer size");
    pRenderPass->renderArea = (VkRect2D){
        .offset = {0, 0},
        .extent = {width, height},
    };
    Attachment *pSwapchainAttachment = getSwapchainAttachmentPtr(pGfxContext);
    if (tknContainsInHashSet(&pSwapchainAttachment->renderPassPtrHashSet, pRenderPass))
    {
        uint32_t swapchainImageCount = pSwapchainUnion->swapchainImageCount;
        pRenderPass->vkFramebufferCount = swapchainImageCount;
        pRenderPass->vkFramebuffers = tknMalloc(sizeof(VkFramebuffer) * swapchainImageCount);
        VkImageView *attachmentVkImageViews = tknMalloc(sizeof(VkImageView) * attachmentCount);
        for (uint32_t swapchainIndex = 0; swapchainIndex < swapchainImageCount; swapchainIndex++)
        {
            for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
            {
                Attachment *pAttachment = attachmentPtrs[attachmentIndex];
                if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
                {
                    attachmentVkImageViews[attachmentIndex] = pSwapchainUnion->swapchainImageViews[swapchainIndex];
                }
                else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
                {
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentUnion.dynamicAttachment.vkImageView;
                }
                else
                {
                    attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentUnion.fixedAttachment.vkImageView;
                }
            }
            VkFramebufferCreateInfo vkFramebufferCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .renderPass = pRenderPass->vkRenderPass,
                .attachmentCount = attachmentCount,
                .pAttachments = attachmentVkImageViews,
                .width = width,
                .height = height,
                .layers = 1,
            };
            assertVkResult(vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pRenderPass->vkFramebuffers[swapchainIndex]));
        }
        tknFree(attachmentVkImageViews);
    }
    else
    {
        pRenderPass->vkFramebufferCount = 1;
        pRenderPass->vkFramebuffers = tknMalloc(sizeof(VkFramebuffer));
        VkImageView *attachmentVkImageViews = tknMalloc(sizeof(VkImageView) * attachmentCount);
        for (uint32_t attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
        {
            Attachment *pAttachment = attachmentPtrs[attachmentIndex];
            if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
            {
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentUnion.dynamicAttachment.vkImageView;
            }
            else
            {
                // ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType
                attachmentVkImageViews[attachmentIndex] = pAttachment->attachmentUnion.fixedAttachment.vkImageView;
            }
        }
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pRenderPass->vkRenderPass,
            .attachmentCount = attachmentCount,
            .pAttachments = attachmentVkImageViews,
            .width = width,
            .height = height,
            .layers = 1,
        };

        assertVkResult(vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pRenderPass->vkFramebuffers[0]));
        tknFree(attachmentVkImageViews);
    }
}
void cleanupFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pRenderPass->vkFramebuffers[i], NULL);
    }
    tknFree(pRenderPass->vkFramebuffers);
}
void repopulateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass)
{
    cleanupFramebuffers(pGfxContext, pRenderPass);
    populateFramebuffers(pGfxContext, pRenderPass);
}
