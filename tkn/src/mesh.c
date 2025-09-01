#include "gfxCore.h"
static void copyVkBuffer(GfxContext *pGfxContext, VkBuffer srcVkBuffer, VkBuffer dstVkBuffer, VkDeviceSize size)
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

Mesh *createMeshPtr(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout, void *vertices, uint32_t vertexCount, VkIndexType vkIndexType, void *indices, uint32_t indexCount)
{
    Mesh *pMesh = tknMalloc(sizeof(Mesh));
    VkBuffer vertexVkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexVkDeviceMemory = VK_NULL_HANDLE;

    VkBuffer indexVkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexVkDeviceMemory = VK_NULL_HANDLE;

    VkBuffer vertexStagingBuffer;
    VkDeviceMemory vertexStagingBufferMemory;
    VkDeviceSize vertexSize = pVertexInputLayout->stride;
    createVkBuffer(pGfxContext, vertexCount * vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertexStagingBuffer, &vertexStagingBufferMemory);
    void *data;
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkMapMemory(vkDevice, vertexStagingBufferMemory, 0, vertexCount * vertexSize, 0, &data);
    memcpy(data, vertices, (size_t)vertexCount * vertexSize);
    vkUnmapMemory(vkDevice, vertexStagingBufferMemory);
    createVkBuffer(pGfxContext, vertexCount * vertexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexVkBuffer, &vertexVkDeviceMemory);
    copyVkBuffer(pGfxContext, vertexStagingBuffer, vertexVkBuffer, vertexCount * vertexSize);
    destroyVkBuffer(pGfxContext, vertexStagingBuffer, vertexStagingBufferMemory);

    if (indexCount > 0)
    {
        VkBuffer indexStagingBuffer;
        VkDeviceMemory indexStagingBufferMemory;
        size_t indexSize = (vkIndexType == VK_INDEX_TYPE_UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);
        VkDeviceSize indexBufferSize = indexCount * indexSize;
        createVkBuffer(pGfxContext, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &indexStagingBuffer, &indexStagingBufferMemory);
        void *data;
        VkDevice vkDevice = pGfxContext->vkDevice;
        vkMapMemory(vkDevice, indexStagingBufferMemory, 0, indexBufferSize, 0, &data);
        memcpy(data, indices, indexBufferSize);
        vkUnmapMemory(vkDevice, indexStagingBufferMemory);
        createVkBuffer(pGfxContext, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexVkBuffer, &indexVkDeviceMemory);
        copyVkBuffer(pGfxContext, indexStagingBuffer, indexVkBuffer, indexBufferSize);
        destroyVkBuffer(pGfxContext, indexStagingBuffer, indexStagingBufferMemory);
    }
    else
    {
        // Keep NULL
    }
    TknHashSet drawCallPtrHashSet = tknCreateHashSet(TKN_DEFAULT_COLLECTION_SIZE);
    *pMesh = (Mesh){
        .vertexVkBuffer = vertexVkBuffer,
        .vertexVkDeviceMemory = vertexVkDeviceMemory,
        .vertexCount = vertexCount,
        .indexVkBuffer = indexVkBuffer,
        .indexVkDeviceMemory = indexVkDeviceMemory,
        .indexCount = indexCount,
        .pVertexInputLayout = pVertexInputLayout,
        .vkIndexType = vkIndexType,
        .drawCallPtrHashSet = drawCallPtrHashSet,
    };
    tknAddToHashSet(&pVertexInputLayout->referencePtrHashSet, pMesh);
    return pMesh;
}
void destroyMeshPtr(GfxContext *pGfxContext, Mesh *pMesh)
{
    tknAssert(0 == pMesh->drawCallPtrHashSet.count, "Mesh still has draw calls attached!");
    tknDestroyHashSet(pMesh->drawCallPtrHashSet);
    tknRemoveFromHashSet(&pMesh->pVertexInputLayout->referencePtrHashSet, pMesh);
    destroyVkBuffer(pGfxContext, pMesh->vertexVkBuffer, pMesh->vertexVkDeviceMemory);
    destroyVkBuffer(pGfxContext, pMesh->indexVkBuffer, pMesh->indexVkDeviceMemory);
    tknFree(pMesh);
}