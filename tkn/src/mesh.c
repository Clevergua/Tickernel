#include "gfxCore.h"
// #include "rply.h"
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

Mesh *createMeshPtrWithData(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout, void *vertices, uint32_t vertexCount, VkIndexType vkIndexType, void *indices, uint32_t indexCount)
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
    TknHashSet drawCallPtrHashSet = tknCreateHashSet(sizeof(DrawCall *));
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
    tknAddToHashSet(&pVertexInputLayout->referencePtrHashSet, &pMesh);
    return pMesh;
}

Mesh *createMeshPtrWithPlyFile(GfxContext *pGfxContext, VertexInputLayout *pMeshVertexInputLayout, VkIndexType vkIndexType, const char *plyFilePath)
{
    FILE *file = fopen(plyFilePath, "rb");
    if (!file)
    {
        tknWarning("Cannot open PLY file: %s", plyFilePath);
        return NULL;
    }

    char line[256];
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    
    // First pass: get element counts and validate format
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = 0;

        if (strcmp(line, "ply") == 0)
        {
            continue;
        }
        else if (strncmp(line, "format ", 7) == 0)
        {
            if (!strstr(line, "binary_little_endian"))
            {
                tknWarning("PLY file is not binary little endian format: %s", plyFilePath);
                fclose(file);
                return NULL;
            }
        }
        else if (strncmp(line, "element ", 8) == 0)
        {
            if (strstr(line, "vertex "))
            {
                sscanf(line, "element vertex %u", &vertexCount);
            }
            else if (strstr(line, "index "))
            {
                sscanf(line, "element index %u", &indexCount);
            }
        }
        else if (strcmp(line, "end_header") == 0)
        {
            break;
        }
    }

    // Second pass: detailed property validation
    fseek(file, 0, SEEK_SET);
    
    uint32_t currentAttributeIndex = 0;
    uint32_t currentAttributeBytesMatched = 0;
    bool inVertexElement = false;
    bool inIndexElement = false;
    uint32_t indexPropertyBytes = 0;
    
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = 0;

        if (strncmp(line, "element ", 8) == 0)
        {
            inVertexElement = false;
            inIndexElement = false;
            
            if (strstr(line, "vertex "))
            {
                inVertexElement = true;
                currentAttributeIndex = 0;
                currentAttributeBytesMatched = 0;
            }
            else if (strstr(line, "index "))
            {
                inIndexElement = true;
                indexPropertyBytes = 0;
            }
        }
        else if (strncmp(line, "property ", 9) == 0)
        {
            uint32_t propertySize = 0;
            
            // Parse property type and get size
            if (strstr(line, " float ") || strstr(line, " int ") || strstr(line, " uint "))
            {
                propertySize = 4;
            }
            else if (strstr(line, " double "))
            {
                propertySize = 8;
            }
            else if (strstr(line, " short ") || strstr(line, " ushort "))
            {
                propertySize = 2;
            }
            else if (strstr(line, " char ") || strstr(line, " uchar "))
            {
                propertySize = 1;
            }
            else
            {
                tknWarning("Unsupported PLY property type in line: %s", line);
                fclose(file);
                return NULL;
            }
            
            if (inVertexElement)
            {
                // Check if we have more attributes to match
                if (currentAttributeIndex >= pMeshVertexInputLayout->attributeCount)
                {
                    tknWarning("PLY has more properties than vertex layout attributes");
                    fclose(file);
                    return NULL;
                }

                // Add this property size to current attribute
                currentAttributeBytesMatched += propertySize;

                // Check if current attribute is complete
                uint32_t expectedAttributeSize = pMeshVertexInputLayout->sizes[currentAttributeIndex];
                if (currentAttributeBytesMatched == expectedAttributeSize)
                {
                    // Attribute complete, move to next
                    currentAttributeIndex++;
                    currentAttributeBytesMatched = 0;
                }
                else if (currentAttributeBytesMatched > expectedAttributeSize)
                {
                    tknWarning("PLY properties exceed expected size for attribute %u: got %u bytes, expected %u bytes",
                               currentAttributeIndex, currentAttributeBytesMatched, expectedAttributeSize);
                    fclose(file);
                    return NULL;
                }
                else
                {
                    // Still matching current attribute
                }
            }
            else if (inIndexElement)
            {
                indexPropertyBytes += propertySize;
            }
        }
        else if (strcmp(line, "end_header") == 0)
        {
            break;
        }
    }

    // Check if all vertex attributes were matched completely
    if (currentAttributeIndex != pMeshVertexInputLayout->attributeCount || currentAttributeBytesMatched != 0)
    {
        tknWarning("PLY properties don't match vertex layout: completed %u/%u attributes, %u bytes remaining",
                   currentAttributeIndex, pMeshVertexInputLayout->attributeCount, currentAttributeBytesMatched);
        fclose(file);
        return NULL;
    }

    // Validate index layout
    if (indexCount > 0)
    {
        uint32_t expectedIndexSize = (vkIndexType == VK_INDEX_TYPE_UINT16) ? 2 : 4;
        if (indexPropertyBytes != expectedIndexSize)
        {
            tknWarning("PLY index properties size (%u bytes) doesn't match expected index size (%u bytes)",
                       indexPropertyBytes, expectedIndexSize);
            fclose(file);
            return NULL;
        }
    }

    // Read binary data
    void *vertices = NULL;
    void *indices = NULL;

    if (vertexCount > 0)
    {
        size_t vertexDataSize = vertexCount * pMeshVertexInputLayout->stride;
        vertices = tknMalloc(vertexDataSize);

        size_t bytesRead = fread(vertices, 1, vertexDataSize, file);
        if (bytesRead != vertexDataSize)
        {
            tknWarning("Failed to read vertex data from PLY file: expected %zu bytes, got %zu",
                       vertexDataSize, bytesRead);
            tknFree(vertices);
            fclose(file);
            return NULL;
        }
    }

    if (indexCount > 0)
    {
        size_t indexSize = (vkIndexType == VK_INDEX_TYPE_UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);
        size_t indexDataSize = indexCount * indexSize;
        indices = tknMalloc(indexDataSize);

        size_t bytesRead = fread(indices, 1, indexDataSize, file);
        if (bytesRead != indexDataSize)
        {
            tknWarning("Failed to read index data from PLY file: expected %zu bytes, got %zu",
                       indexDataSize, bytesRead);
            tknFree(vertices);
            tknFree(indices);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);

    Mesh *pMesh = createMeshPtrWithData(pGfxContext, pMeshVertexInputLayout, vertices, vertexCount, vkIndexType, indices, indexCount);

    // Clean up temporary data
    if (vertices)
    {
        tknFree(vertices);
    }
    if (indices)
    {
        tknFree(indices);
    }

    return pMesh;
}
void destroyMeshPtr(GfxContext *pGfxContext, Mesh *pMesh)
{
    tknAssert(0 == pMesh->drawCallPtrHashSet.count, "Mesh still has draw calls attached!");
    tknDestroyHashSet(pMesh->drawCallPtrHashSet);
    tknRemoveFromHashSet(&pMesh->pVertexInputLayout->referencePtrHashSet, &pMesh);
    if (pMesh->vertexVkBuffer != VK_NULL_HANDLE && pMesh->vertexVkDeviceMemory != VK_NULL_HANDLE)
    {
        destroyVkBuffer(pGfxContext, pMesh->vertexVkBuffer, pMesh->vertexVkDeviceMemory);
    }
    if (pMesh->indexVkBuffer != VK_NULL_HANDLE && pMesh->indexVkDeviceMemory != VK_NULL_HANDLE)
    {
        destroyVkBuffer(pGfxContext, pMesh->indexVkBuffer, pMesh->indexVkDeviceMemory);
    }
    tknFree(pMesh);
}
