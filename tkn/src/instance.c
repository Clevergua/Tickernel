#include "gfxCore.h"

Instance *createInstancePtr(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout, uint32_t instanceCount, void *instances, Mesh *pMesh)
{
    Instance *pInstance = tknMalloc(sizeof(Instance));

    if (instanceCount > 0)
    {
        VkBuffer instanceVkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory instanceVkDeviceMemory = VK_NULL_HANDLE;
        VkDeviceSize bufferSize = instanceCount * pVertexInputLayout->stride;
        createVkBuffer(pGfxContext, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &instanceVkBuffer, &instanceVkDeviceMemory);
        void *instanceMappedBuffer = NULL;
        vkMapMemory(pGfxContext->vkDevice, instanceVkDeviceMemory, 0, bufferSize, 0, &instanceMappedBuffer);
        memcpy(instanceMappedBuffer, instances, instanceCount * pVertexInputLayout->stride);
        *pInstance = (Instance){
            .pVertexInputLayout = pVertexInputLayout,
            .instanceVkBuffer = instanceVkBuffer,
            .instanceVkDeviceMemory = instanceVkDeviceMemory,
            .instanceMappedBuffer = instanceMappedBuffer,
            .instanceCount = instanceCount,
            .maxInstanceCount = instanceCount,
            .pMesh = pMesh,
            .materialPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
        };
    }
    else
    {
        *pInstance = (Instance){
            .pVertexInputLayout = pVertexInputLayout,
            .instanceVkBuffer = VK_NULL_HANDLE,
            .instanceVkDeviceMemory = VK_NULL_HANDLE,
            .instanceMappedBuffer = NULL,
            .instanceCount = instanceCount,
            .maxInstanceCount = instanceCount,
            .pMesh = pMesh,
            .materialPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE),
        };
    }
    tknAddToHashSet(&pMesh->instancePtrHashSet, pInstance);

    return pInstance;
}
void destroyInstancePtr(GfxContext *pGfxContext, Instance *pInstance)
{
    tknAssert(0 == pInstance->materialPtrHashSet.count, "Cannot destroy instance with materials attached!");
    tknDestroyHashSet(pInstance->materialPtrHashSet);
    tknRemoveFromHashSet(&pInstance->pMesh->instancePtrHashSet, pInstance);
    destroyVkBuffer(pGfxContext, pInstance->instanceVkBuffer, pInstance->instanceVkDeviceMemory);
    tknFree(pInstance);
}
void updateInstancePtr(GfxContext *pGfxContext, Instance *pInstance, void *newData, uint32_t instanceCount)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    if (0 == pInstance->maxInstanceCount)
    {
        if (0 == instanceCount)
        {
            pInstance->maxInstanceCount = instanceCount;
            pInstance->instanceCount = instanceCount;
            VkDeviceSize instanceBufferSize = pInstance->pVertexInputLayout->stride * pInstance->instanceCount;
            createVkBuffer(pGfxContext, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pInstance->instanceVkBuffer, &pInstance->instanceVkDeviceMemory);
            pInstance->instanceMappedBuffer = NULL;
            vkMapMemory(vkDevice, pInstance->instanceVkDeviceMemory, 0, instanceBufferSize, 0, &pInstance->instanceMappedBuffer);
            memcpy(pInstance->instanceMappedBuffer, newData, instanceBufferSize);
        }
        else
        {
            pInstance->instanceCount = 0;
            destroyVkBuffer(pGfxContext, pInstance->instanceVkBuffer, pInstance->instanceVkDeviceMemory);
            pInstance->instanceMappedBuffer = NULL;
        }
    }
    else
    {
        if (instanceCount <= pInstance->maxInstanceCount)
        {
            pInstance->instanceCount = instanceCount;
            VkDeviceSize bufferSize = pInstance->pVertexInputLayout->stride * pInstance->instanceCount;
            memcpy(pInstance->instanceMappedBuffer, newData, bufferSize);
        }
        else
        {
            destroyVkBuffer(pGfxContext, pInstance->instanceVkBuffer, pInstance->instanceVkDeviceMemory);
            pInstance->maxInstanceCount = instanceCount;
            pInstance->instanceCount = instanceCount;
            VkDeviceSize instanceBufferSize = pInstance->pVertexInputLayout->stride * pInstance->instanceCount;
            createVkBuffer(pGfxContext, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pInstance->instanceVkBuffer, &pInstance->instanceVkDeviceMemory);
            vkMapMemory(vkDevice, pInstance->instanceVkDeviceMemory, 0, instanceBufferSize, 0, &pInstance->instanceMappedBuffer);
            memcpy(pInstance->instanceMappedBuffer, newData, instanceBufferSize);
        }
    }
}
void addInstanceToMaterial(GfxContext *pGfxContext, Instance *pInstance, Material *pMaterial)
{
    if (pMaterial->pPipeline->pInstanceVertexInputLayout == pInstance->pVertexInputLayout && pMaterial->pPipeline->pMeshVertexInputLayout == pInstance->pMesh->pVertexInputLayout)
    {
        tknAddToDynamicArray(&pMaterial->instancePtrDynamicArray, &pInstance);
        tknAddToHashSet(&pInstance->materialPtrHashSet, pMaterial);
    }
    else
    {
        printf("Instance does not match pipeline layouts\n");
    }
}
void removeInstanceFromMaterial(GfxContext *pGfxContext, Instance *pInstance, Material *pMaterial)
{
    if (tknContainsInHashSet(&pInstance->materialPtrHashSet, pMaterial))
    {
        tknRemoveFromDynamicArray(&pMaterial->instancePtrDynamicArray, pInstance);
        tknRemoveFromHashSet(&pInstance->materialPtrHashSet, pMaterial);
    }
    else
    {
        printf("Instance is not part of the material\n");
    }
}
