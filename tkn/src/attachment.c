#include "gfxCore.h"

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
        .bindingPtrHashSet = tknCreateHashSet(sizeof(Binding *)),
    };
    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_DYNAMIC,
        .attachmentUnion.dynamicAttachment = dynamicAttachment,
        .vkFormat = vkFormat,
        .renderPassPtrHashSet = tknCreateHashSet(sizeof(RenderPass *)),
    };
    tknAddToHashSet(&pGfxContext->dynamicAttachmentPtrHashSet, pAttachment);
    return pAttachment;
}
void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment)
{
    tknAssert(ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType, "Attachment type mismatch!");
    tknRemoveFromHashSet(&pGfxContext->dynamicAttachmentPtrHashSet, pAttachment);
    DynamicAttachment dynamicAttachment = pAttachment->attachmentUnion.dynamicAttachment;
    tknAssert(0 == dynamicAttachment.bindingPtrHashSet.count, "Cannot destroy dynamic attachment with bindings attached!");
    tknDestroyHashSet(dynamicAttachment.bindingPtrHashSet);
    tknAssert(0 == pAttachment->renderPassPtrHashSet.count, "Cannot destroy dynamic attachment with render passes attached!");
    tknDestroyHashSet(pAttachment->renderPassPtrHashSet);
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
            Binding *pBinding = *(Binding **)node->data;
            updateBindings(pGfxContext, 1, pBinding);
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
        .bindingPtrHashSet = tknCreateHashSet(sizeof(Binding *)),
    };

    *pAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_FIXED,
        .attachmentUnion.fixedAttachment = fixedAttachment,
        .vkFormat = vkFormat,
        .renderPassPtrHashSet = tknCreateHashSet(sizeof(RenderPass *)),
    };
    tknAddToHashSet(&pGfxContext->fixedAttachmentPtrHashSet, pAttachment);
    return pAttachment;
}
void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment)
{
    tknAssert(ATTACHMENT_TYPE_FIXED == pAttachment->attachmentType, "Attachment type mismatch!");
    tknRemoveFromHashSet(&pGfxContext->fixedAttachmentPtrHashSet, pAttachment);
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
