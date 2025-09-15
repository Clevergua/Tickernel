#include "gfxCore.h"
// #include "rply.h"

static uint32_t getPlyPropertySize(const char *propertyType)
{
    if (strcmp(propertyType, "float") == 0 || strcmp(propertyType, "int") == 0 || strcmp(propertyType, "uint") == 0)
    {
        return 4;
    }
    else if (strcmp(propertyType, "double") == 0)
    {
        return 8;
    }
    else if (strcmp(propertyType, "short") == 0 || strcmp(propertyType, "ushort") == 0)
    {
        return 2;
    }
    else if (strcmp(propertyType, "char") == 0 || strcmp(propertyType, "uchar") == 0)
    {
        return 1;
    }
    else
    {
        return 0; // Invalid type
    }
}

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

static bool readBinaryData(FILE *file, void **data, size_t dataSize, const char *dataType)
{
    if (dataSize == 0)
    {
        *data = NULL;
        return true;
    }
    
    *data = tknMalloc(dataSize);
    size_t bytesRead = fread(*data, 1, dataSize, file);
    if (bytesRead != dataSize)
    {
        tknWarning("Failed to read %s data from PLY file: expected %zu bytes, got %zu",
                   dataType, dataSize, bytesRead);
        tknFree(*data);
        *data = NULL;
        return false;
    }
    return true;
}

static bool createBufferWithData(GfxContext *pGfxContext, void *data, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer *pBuffer, VkDeviceMemory *pDeviceMemory)
{
    if (size == 0)
    {
        *pBuffer = VK_NULL_HANDLE;
        *pDeviceMemory = VK_NULL_HANDLE;
        return true;
    }
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    // Create staging buffer
    createVkBuffer(pGfxContext, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                   &stagingBuffer, &stagingBufferMemory);
    
    // Copy data to staging buffer
    void *mappedData;
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkMapMemory(vkDevice, stagingBufferMemory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, (size_t)size);
    vkUnmapMemory(vkDevice, stagingBufferMemory);
    
    // Create device local buffer
    createVkBuffer(pGfxContext, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, 
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pBuffer, pDeviceMemory);
    
    // Copy from staging to device local buffer
    copyVkBuffer(pGfxContext, stagingBuffer, *pBuffer, size);
    
    // Clean up staging buffer
    destroyVkBuffer(pGfxContext, stagingBuffer, stagingBufferMemory);
    
    return true;
}

Mesh *createMeshPtrWithData(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout, void *vertices, uint32_t vertexCount, VkIndexType vkIndexType, void *indices, uint32_t indexCount)
{
    Mesh *pMesh = tknMalloc(sizeof(Mesh));
    VkBuffer vertexVkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexVkDeviceMemory = VK_NULL_HANDLE;
    VkBuffer indexVkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexVkDeviceMemory = VK_NULL_HANDLE;

    // Create vertex buffer
    VkDeviceSize vertexSize = vertexCount * pVertexInputLayout->stride;
    createBufferWithData(pGfxContext, vertices, vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                        &vertexVkBuffer, &vertexVkDeviceMemory);

    // Create index buffer if needed
    if (indexCount > 0)
    {
        size_t indexSize = (vkIndexType == VK_INDEX_TYPE_UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);
        VkDeviceSize indexBufferSize = indexCount * indexSize;
        createBufferWithData(pGfxContext, indices, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                            &indexVkBuffer, &indexVkDeviceMemory);
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
            // Parse property type from line and get size
            char *propertyType = NULL;
            uint32_t propertySize = 0;
            
            // Extract property type from line (format: "property <type> <name>")
            char *typeStart = strstr(line, " ");
            if (typeStart)
            {
                typeStart++; // Skip first space
                char *typeEnd = strstr(typeStart, " ");
                if (typeEnd)
                {
                    size_t typeLen = typeEnd - typeStart;
                    propertyType = tknMalloc(typeLen + 1);
                    strncpy(propertyType, typeStart, typeLen);
                    propertyType[typeLen] = '\0';
                    
                    propertySize = getPlyPropertySize(propertyType);
                    tknFree(propertyType);
                }
            }
            
            if (propertySize == 0)
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

    // Read vertex data
    size_t vertexDataSize = vertexCount * pMeshVertexInputLayout->stride;
    if (!readBinaryData(file, &vertices, vertexDataSize, "vertex"))
    {
        fclose(file);
        return NULL;
    }

    // Read index data
    if (indexCount > 0)
    {
        size_t indexSize = (vkIndexType == VK_INDEX_TYPE_UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);
        size_t indexDataSize = indexCount * indexSize;
        if (!readBinaryData(file, &indices, indexDataSize, "index"))
        {
            tknFree(vertices);
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

void saveMeshPtrToPlyFile(uint32_t vertexPropertyCount, const char **vertexPropertyNames, const char **vertexPropertyTypes, VertexInputLayout *pMeshVertexInputLayout, void *vertices, uint32_t vertexCount, VkIndexType vkIndexType, void *indices, uint32_t indexCount, const char *plyFilePath)
{
    // Validate that the provided property names and types match the VertexInputLayout
    uint32_t propertyIndex = 0;
    for (uint32_t i = 0; i < pMeshVertexInputLayout->attributeCount; i++)
    {
        uint32_t expectedAttributeSize = pMeshVertexInputLayout->sizes[i];
        uint32_t accumulatedPropertySize = 0;
        
        // Accumulate property sizes until we match the expected attribute size
        while (accumulatedPropertySize < expectedAttributeSize)
        {
            if (propertyIndex >= vertexPropertyCount)
            {
                tknWarning("Not enough properties provided: need more properties for attribute %u", i);
                return;
            }
            
            const char *propertyType = vertexPropertyTypes[propertyIndex];
            uint32_t propertySize = getPlyPropertySize(propertyType);
            
            if (propertySize == 0)
            {
                tknWarning("Unsupported property type: %s", propertyType);
                return;
            }
            
            accumulatedPropertySize += propertySize;
            propertyIndex++;
            
            // Check if we've exceeded the expected size
            if (accumulatedPropertySize > expectedAttributeSize)
            {
                tknWarning("Property sizes exceed expected size for attribute %u: got %u bytes, expected %u bytes",
                           i, accumulatedPropertySize, expectedAttributeSize);
                return;
            }
        }
        
        // Check if we have exact match
        if (accumulatedPropertySize != expectedAttributeSize)
        {
            tknWarning("Property sizes don't match expected size for attribute %u: got %u bytes, expected %u bytes",
                       i, accumulatedPropertySize, expectedAttributeSize);
            return;
        }
    }
    
    // Check if we have used all provided properties
    if (propertyIndex != vertexPropertyCount)
    {
        tknWarning("Property count mismatch: used %u properties, but provided %u properties",
                   propertyIndex, vertexPropertyCount);
        return;
    }
    
    FILE *file = fopen(plyFilePath, "wb");
    if (!file)
    {
        tknWarning("Cannot create PLY file: %s", plyFilePath);
        return;
    }

    // Write PLY header
    fprintf(file, "ply\n");
    fprintf(file, "format binary_little_endian 1.0\n");
    
    // Write vertex element definition
    fprintf(file, "element vertex %u\n", vertexCount);
    
    // Write vertex properties
    for (uint32_t i = 0; i < vertexPropertyCount; i++)
    {
        fprintf(file, "property %s %s\n", vertexPropertyTypes[i], vertexPropertyNames[i]);
    }
    
    // Write index element definition if indices exist
    if (indexCount > 0)
    {
        fprintf(file, "element index %u\n", indexCount);
        if (vkIndexType == VK_INDEX_TYPE_UINT16)
        {
            fprintf(file, "property ushort vertex_index\n");
        }
        else // VK_INDEX_TYPE_UINT32
        {
            fprintf(file, "property uint vertex_index\n");
        }
    }
    
    // End header
    fprintf(file, "end_header\n");
    
    // Write binary vertex data
    if (vertexCount > 0)
    {
        size_t vertexDataSize = vertexCount * pMeshVertexInputLayout->stride;
        size_t bytesWritten = fwrite(vertices, 1, vertexDataSize, file);
        if (bytesWritten != vertexDataSize)
        {
            tknWarning("Failed to write vertex data to PLY file: expected %zu bytes, wrote %zu",
                       vertexDataSize, bytesWritten);
            fclose(file);
            return;
        }
    }
    
    // Write binary index data
    if (indexCount > 0)
    {
        size_t indexSize = (vkIndexType == VK_INDEX_TYPE_UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);
        size_t indexDataSize = indexCount * indexSize;
        size_t bytesWritten = fwrite(indices, 1, indexDataSize, file);
        if (bytesWritten != indexDataSize)
        {
            tknWarning("Failed to write index data to PLY file: expected %zu bytes, wrote %zu",
                       indexDataSize, bytesWritten);
            fclose(file);
            return;
        }
    }
    
    fclose(file);
}
