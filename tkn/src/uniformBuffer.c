#include "gfxCore.h"

UniformBuffer *createUniformBufferPtr(GfxContext *pGfxContext, const void *data, VkDeviceSize size)
{
    UniformBuffer *pUniformBuffer = tknMalloc(sizeof(UniformBuffer));
    VkBuffer vkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
    void *mapped = NULL;
    TknHashSet bindingPtrHashSet = tknCreateHashSet(sizeof(Binding *));

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

    memcpy(pUniformBuffer->mapped, data, size);
    return pUniformBuffer;
}
void destroyUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer)
{
    clearBindingPtrHashSet(pGfxContext, pUniformBuffer->bindingPtrHashSet);
    tknDestroyHashSet(pUniformBuffer->bindingPtrHashSet);
    if (pUniformBuffer->mapped != NULL)
    {
        vkUnmapMemory(pGfxContext->vkDevice, pUniformBuffer->vkDeviceMemory);
    }
    destroyVkBuffer(pGfxContext, pUniformBuffer->vkBuffer, pUniformBuffer->vkDeviceMemory);
    pUniformBuffer->vkBuffer = VK_NULL_HANDLE;
    pUniformBuffer->vkDeviceMemory = VK_NULL_HANDLE;
    tknFree(pUniformBuffer);
}
void updateUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer, const void *data, VkDeviceSize size)
{
    tknAssert(pUniformBuffer->mapped != NULL, "Uniform buffer is not mapped!");
    tknAssert(size <= pUniformBuffer->size, "Data size exceeds mapped buffer size!");
    memcpy(pUniformBuffer->mapped, data, size);
}
