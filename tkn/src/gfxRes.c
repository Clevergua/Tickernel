#include "gfxRes.h"

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
static void createBuffer(GfxContext *pGfxContext, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer *pVkBuffer, VkDeviceMemory *pVkDeviceMemory)
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
static void destroyBuffer(GfxContext *pGfxContext, VkBuffer vkBuffer, VkDeviceMemory vkDeviceMemory)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkDestroyBuffer(vkDevice, vkBuffer, NULL);
    vkFreeMemory(vkDevice, vkDeviceMemory, NULL);
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
    VkDevice vkDevice = pGfxContext->vkDevice;
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
    for (uint32_t i = 0; i < pImage->bindingPtrHashSet.capacity; i++)
    {
        TknListNode *node = pImage->bindingPtrHashSet.nodePtrs[i];
        while (node)
        {
            Binding *pBinding = (Binding *)node->pointer;
            Binding binding = *pBinding;
            binding.bindingUnion = getNullBindingUnion(pBinding->vkDescriptorType);
            updateBindings(pGfxContext, 1, &binding);
            node = node->nextNodePtr;
        }
    }
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

    createBuffer(pGfxContext, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vkBuffer, &vkDeviceMemory);
    VkDevice vkDevice = pGfxContext->vkDevice;
    assertVkResult(vkMapMemory(vkDevice, vkDeviceMemory, 0, size, 0, &mapped));
    *pUniformBuffer = (UniformBuffer){
        .vkBuffer = vkBuffer,
        .vkDeviceMemory = vkDeviceMemory,
        .mapped = mapped,
        .bindingPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
        .size = size,
    };

    return pUniformBuffer;
}
void destroyUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer)
{
    for (uint32_t i = 0; i < pUniformBuffer->bindingPtrHashSet.capacity; i++)
    {
        TknListNode *node = pUniformBuffer->bindingPtrHashSet.nodePtrs[i];
        while (node)
        {
            Binding *pBinding = (Binding *)node->pointer;
            Binding binding = *pBinding;
            binding.bindingUnion = getNullBindingUnion(pBinding->vkDescriptorType);
            updateBindings(pGfxContext, 1, &binding);
            node = node->nextNodePtr;
        }
    }

    tknDestroyHashSet(pUniformBuffer->bindingPtrHashSet);
    destroyBuffer(pGfxContext, pUniformBuffer->vkBuffer, pUniformBuffer->vkDeviceMemory);
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
void destroyMaterialPtr(GfxContext *pGfxContext, Material *pMaterial)
{
    tknRemoveFromDynamicArray(&pMaterial->pDescriptorSet->materialPtrDynamicArray, &pMaterial);
    VkDevice vkDevice = pGfxContext->vkDevice;
    uint32_t descriptorCount = 0;
    Binding *bindings = tknMalloc(sizeof(Binding) * pMaterial->bindingCount);
    for (uint32_t binding = 0; binding < pMaterial->bindingCount; binding++)
    {
        VkDescriptorType descriptorType = pMaterial->bindings[binding].vkDescriptorType;
        if (descriptorType != VK_DESCRIPTOR_TYPE_MAX_ENUM && descriptorType != VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
        {
            bindings[descriptorCount] = (Binding){
                .vkDescriptorType = descriptorType,
                .bindingUnion = getNullBindingUnion(descriptorType),
                .pMaterial = pMaterial,
                .binding = binding,
            };
            descriptorCount++;
        }
        else
        {
            // Skip
        }
    }
    updateBindings(pGfxContext, descriptorCount, bindings);
    tknFree(pMaterial->bindings);
    vkFreeDescriptorSets(vkDevice, pMaterial->vkDescriptorPool, 1, &pMaterial->vkDescriptorSet);
    vkDestroyDescriptorPool(vkDevice, pMaterial->vkDescriptorPool, NULL);
    tknFree(pMaterial);
}
BindingUnion getNullBindingUnion(VkDescriptorType vkDescriptorType)
{
    BindingUnion bindingUnion = {0};
    // VK_DESCRIPTOR_TYPE_SAMPLER = 0,
    if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
    {
        bindingUnion.samplerBinding.pSampler = NULL;
    }
    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
    // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER == vkDescriptorType)
    {
        bindingUnion.uniformBufferBinding.pUniformBuffer = NULL;
    }
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
    // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
    else if (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == vkDescriptorType)
    {
        bindingUnion.inputAttachmentBinding.pAttachment = NULL;
        bindingUnion.inputAttachmentBinding.vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    else
    {
        tknError("Unsupported descriptor type: %d", vkDescriptorType);
    }
    return bindingUnion;
}
void updateInputAttachmentBindings(GfxContext *pGfxContext, uint32_t inputAttachmentBindingCount, Binding *inputAttachmentBindings)
{
    if (inputAttachmentBindingCount > 0)
    {
        VkWriteDescriptorSet *vkWriteDescriptorSets = tknMalloc(sizeof(VkWriteDescriptorSet) * inputAttachmentBindingCount);
        VkDescriptorImageInfo *vkDescriptorImageInfos = tknMalloc(sizeof(VkDescriptorImageInfo) * inputAttachmentBindingCount);
        for (uint32_t inputAttachmentBindingIndex = 0; inputAttachmentBindingIndex < inputAttachmentBindingCount; inputAttachmentBindingIndex++)
        {
            Binding descriptor = inputAttachmentBindings[inputAttachmentBindingIndex];
            tknAssert(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == descriptor.vkDescriptorType, "Input attachment descriptor type mismatch!");

            vkDescriptorImageInfos[inputAttachmentBindingIndex] = (VkDescriptorImageInfo){
                .sampler = VK_NULL_HANDLE,
                .imageView = VK_NULL_HANDLE,
                .imageLayout = descriptor.bindingUnion.inputAttachmentBinding.vkImageLayout,
            };
            vkWriteDescriptorSets[inputAttachmentBindingIndex] = (VkWriteDescriptorSet){
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptor.pMaterial->vkDescriptorSet,
                .dstBinding = descriptor.binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = descriptor.vkDescriptorType,
                .pImageInfo = &vkDescriptorImageInfos[inputAttachmentBindingIndex],
                .pBufferInfo = VK_NULL_HANDLE,
                .pTexelBufferView = VK_NULL_HANDLE,
            };
        }
        VkDevice vkDevice = pGfxContext->vkDevice;
        vkUpdateDescriptorSets(vkDevice, inputAttachmentBindingCount, vkWriteDescriptorSets, 0, NULL);
        tknFree(vkDescriptorImageInfos);
        tknFree(vkWriteDescriptorSets);
    }
    else
    {
        printf("No input attachments to update");
    }
}
void updateBindings(GfxContext *pGfxContext, uint32_t bindingCount, Binding *bindings)
{
    if (bindingCount > 0)
    {
        Material *pMaterial = bindings[0].pMaterial;
        tknAssert(NULL != pMaterial, "Material must not be NULL");
        uint32_t vkWriteDescriptorSetCount = 0;
        VkWriteDescriptorSet *vkWriteDescriptorSets = tknMalloc(sizeof(VkWriteDescriptorSet) * bindingCount);
        VkDescriptorImageInfo *vkDescriptorImageInfos = tknMalloc(sizeof(VkDescriptorImageInfo) * bindingCount);
        VkDescriptorBufferInfo *vkDescriptorBufferInfos = tknMalloc(sizeof(VkDescriptorBufferInfo) * bindingCount);
        for (uint32_t bindingIndex = 0; bindingIndex < bindingCount; bindingIndex++)
        {
            Binding newBinding = bindings[bindingIndex];
            tknAssert(newBinding.pMaterial == pMaterial, "All bindings must belong to the same descriptor set");
            uint32_t binding = newBinding.binding;
            tknAssert(binding < pMaterial->bindingCount, "Invalid binding index");
            VkDescriptorType vkDescriptorType = pMaterial->bindings[binding].vkDescriptorType;
            tknAssert(vkDescriptorType == newBinding.vkDescriptorType, "Incompatible descriptor type");
            // VK_DESCRIPTOR_TYPE_SAMPLER = 0,
            // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
            // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
            // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
            // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
            // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
            // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
            // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
            // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
            // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
            if (VK_DESCRIPTOR_TYPE_SAMPLER == vkDescriptorType)
            {
                Sampler *pNewSampler = newBinding.bindingUnion.samplerBinding.pSampler;
                Binding *pCurrentBinding = &pMaterial->bindings[binding];
                Sampler *pCurrentSampler = pCurrentBinding->bindingUnion.samplerBinding.pSampler;
                if (pNewSampler == pCurrentSampler)
                {
                    // No change, skip
                }
                else
                {
                    if (NULL == pCurrentSampler)
                    {
                        // Nothing
                    }
                    else
                    {
                        // Current sampler deref descriptor
                        tknRemoveFromHashSet(&pCurrentSampler->bindingPtrHashSet, pCurrentBinding);
                    }

                    pCurrentBinding->bindingUnion.samplerBinding.pSampler = pNewSampler;
                    if (NULL == pNewSampler)
                    {
                        vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                            .sampler = NULL,
                            .imageView = VK_NULL_HANDLE,
                            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        };
                    }
                    else
                    {
                        // New sampler ref descriptor
                        tknAddToHashSet(&pNewSampler->bindingPtrHashSet, pCurrentBinding);
                        vkDescriptorImageInfos[vkWriteDescriptorSetCount] = (VkDescriptorImageInfo){
                            .sampler = pNewSampler->vkSampler,
                            .imageView = VK_NULL_HANDLE,
                            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        };
                    }
                    VkWriteDescriptorSet vkWriteDescriptorSet = {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = pMaterial->vkDescriptorSet,
                        .dstBinding = binding,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = vkDescriptorType,
                        .pImageInfo = &vkDescriptorImageInfos[vkWriteDescriptorSetCount],
                        .pBufferInfo = NULL,
                        .pTexelBufferView = NULL,
                    };
                    vkWriteDescriptorSets[vkWriteDescriptorSetCount] = vkWriteDescriptorSet;
                    vkWriteDescriptorSetCount++;
                }
            }
            else if (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == vkDescriptorType)
            {
                tknError("Combined image sampler not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE == vkDescriptorType)
            {
                tknError("Sampled image not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_STORAGE_IMAGE == vkDescriptorType)
            {
                tknError("Storage image not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER == vkDescriptorType)
            {
                tknError("Uniform texel buffer not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER == vkDescriptorType)
            {
                tknError("Storage texel buffer not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER == vkDescriptorType)
            {
                UniformBuffer *pNewUniformBuffer = newBinding.bindingUnion.uniformBufferBinding.pUniformBuffer;
                Binding *pCurrentBinding = &pMaterial->bindings[binding];
                UniformBuffer *pCurrentUniformBuffer = pCurrentBinding->bindingUnion.uniformBufferBinding.pUniformBuffer;
                if (pNewUniformBuffer == pCurrentUniformBuffer)
                {
                    // No change, skip
                }
                else
                {
                    if (NULL == pCurrentUniformBuffer)
                    {
                        // Nothing
                    }
                    else
                    {
                        // Current uniform buffer deref descriptor
                        tknRemoveFromHashSet(&pCurrentUniformBuffer->bindingPtrHashSet, pCurrentBinding);
                    }

                    pCurrentBinding->bindingUnion.uniformBufferBinding.pUniformBuffer = pNewUniformBuffer;
                    if (NULL == pNewUniformBuffer)
                    {
                        vkDescriptorBufferInfos[vkWriteDescriptorSetCount] = (VkDescriptorBufferInfo){
                            .buffer = VK_NULL_HANDLE,
                            .offset = 0,
                            .range = VK_WHOLE_SIZE,
                        };
                    }
                    else
                    {
                        // New uniform buffer ref descriptor
                        tknAddToHashSet(&pNewUniformBuffer->bindingPtrHashSet, pCurrentBinding);
                        vkDescriptorBufferInfos[vkWriteDescriptorSetCount] = (VkDescriptorBufferInfo){
                            .buffer = pNewUniformBuffer->vkBuffer,
                            .offset = 0,
                            .range = pNewUniformBuffer->size,
                        };
                    }
                    VkWriteDescriptorSet vkWriteDescriptorSet = {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = pMaterial->vkDescriptorSet,
                        .dstBinding = binding,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = vkDescriptorType,
                        .pImageInfo = NULL,
                        .pBufferInfo = &vkDescriptorBufferInfos[vkWriteDescriptorSetCount],
                        .pTexelBufferView = NULL,
                    };
                    vkWriteDescriptorSets[vkWriteDescriptorSetCount] = vkWriteDescriptorSet;
                    vkWriteDescriptorSetCount++;
                }
            }
            else if (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER == vkDescriptorType)
            {
                tknError("Storage buffer not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC == vkDescriptorType)
            {
                tknError("Uniform buffer dynamic not yet implemented");
            }
            else if (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC == vkDescriptorType)
            {
                tknError("Storage buffer dynamic not yet implemented");
            }
            else
            {
                tknError("Unsupported descriptor type: %d", vkDescriptorType);
            }
        }

        if (vkWriteDescriptorSetCount > 0)
        {
            VkDevice vkDevice = pGfxContext->vkDevice;
            vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetCount, vkWriteDescriptorSets, 0, NULL);
        }
        tknFree(vkDescriptorBufferInfos);
        tknFree(vkDescriptorImageInfos);
        tknFree(vkWriteDescriptorSets);
    }
    else
    {
        printf("Warning: No bindings to update\n");
        return;
    }
}