#include "gfxCore.h"

Instance *createInstancePtr(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout, uint32_t instanceCount, void *instances)
{
    Instance *pInstance = tknMalloc(sizeof(Instance));
    TknHashSet drawCallPtrHashSet = tknCreateHashSet(sizeof(DrawCall *));
    // Initialize common fields
    *pInstance = (Instance){
        .pVertexInputLayout = pVertexInputLayout,
        .instanceVkBuffer = VK_NULL_HANDLE,
        .instanceVkDeviceMemory = VK_NULL_HANDLE,
        .instanceMappedBuffer = NULL,
        .instanceCount = instanceCount,
        .maxInstanceCount = instanceCount,
        .drawCallPtrHashSet = drawCallPtrHashSet,
    };
    
    if (instanceCount > 0)
    {
        VkDeviceSize bufferSize = instanceCount * pVertexInputLayout->stride;
        createVkBuffer(pGfxContext, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pInstance->instanceVkBuffer, &pInstance->instanceVkDeviceMemory);
        vkMapMemory(pGfxContext->vkDevice, pInstance->instanceVkDeviceMemory, 0, bufferSize, 0, &pInstance->instanceMappedBuffer);
        memcpy(pInstance->instanceMappedBuffer, instances, bufferSize);
    }
    else
    {
        // Resources already initialized to NULL/0 above
    }
    
    return pInstance;
}
void destroyInstancePtr(GfxContext *pGfxContext, Instance *pInstance)
{
    tknAssert(0 == pInstance->drawCallPtrHashSet.count, "Instance still has draw calls attached!");
    if (pInstance->instanceCount > 0)
    {
        destroyVkBuffer(pGfxContext, pInstance->instanceVkBuffer, pInstance->instanceVkDeviceMemory);
    }
    else
    {
        // Nothing to clean up
    }
    tknDestroyHashSet(pInstance->drawCallPtrHashSet);
    tknFree(pInstance);
}
void updateInstancePtr(GfxContext *pGfxContext, Instance *pInstance, void *newData, uint32_t instanceCount)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    VkDeviceSize newBufferSize = pInstance->pVertexInputLayout->stride * instanceCount;
    
    if (0 == pInstance->maxInstanceCount)
    {
        if (instanceCount > 0)
        {
            pInstance->maxInstanceCount = instanceCount;
            pInstance->instanceCount = instanceCount;
            createVkBuffer(pGfxContext, newBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pInstance->instanceVkBuffer, &pInstance->instanceVkDeviceMemory);
            vkMapMemory(vkDevice, pInstance->instanceVkDeviceMemory, 0, newBufferSize, 0, &pInstance->instanceMappedBuffer);
            memcpy(pInstance->instanceMappedBuffer, newData, newBufferSize);
        }
        else
        {
            pInstance->instanceCount = 0;
        }
    }
    else
    {
        if (instanceCount == 0)
        {
            pInstance->instanceCount = 0;
        }
        else if (instanceCount <= pInstance->maxInstanceCount)
        {
            pInstance->instanceCount = instanceCount;
            memcpy(pInstance->instanceMappedBuffer, newData, newBufferSize);
        }
        else
        {
            destroyVkBuffer(pGfxContext, pInstance->instanceVkBuffer, pInstance->instanceVkDeviceMemory);
            pInstance->maxInstanceCount = instanceCount;
            pInstance->instanceCount = instanceCount;
            createVkBuffer(pGfxContext, newBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pInstance->instanceVkBuffer, &pInstance->instanceVkDeviceMemory);
            vkMapMemory(vkDevice, pInstance->instanceVkDeviceMemory, 0, newBufferSize, 0, &pInstance->instanceMappedBuffer);
            memcpy(pInstance->instanceMappedBuffer, newData, newBufferSize);
        }
    }
}
