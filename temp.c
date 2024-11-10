// void CreateVertexBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkDeviceSize vertexBufferSize, void *vertices, VkBuffer *pVertexBuffer, VkDeviceMemory *pVertexBufferMemory)
// {
//     // VkBuffer stagingBuffer;
//     // VkDeviceMemory stagingBufferMemory;
//     // CreateBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

//     // void *pData;
//     // VkResult result = vkMapMemory(vkDevice, stagingBufferMemory, 0, vertexBufferSize, 0, &pData);
//     // TryThrowVulkanError(result);
//     // memcpy(pData, vertices, vertexBufferSize);
//     // vkUnmapMemory(vkDevice, stagingBufferMemory);

//     CreateBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pVertexBuffer, pVertexBufferMemory);
//     // CopyVkBuffer(graphicVkCommandPool, vkDevice, vkGraphicQueue, stagingBuffer, *pVertexBuffer, 0, vertexBufferSize);

//     // vkDestroyBuffer(vkDevice, stagingBuffer, NULL);
//     // vkFreeMemory(vkDevice, stagingBufferMemory, NULL);
// }
// void DestroyVertexBuffer(VkDevice vkDevice, VkBuffer vertexBuffer, VkDeviceMemory vertexBufferMemory)
// {
//     DestroyBuffer(vertexBuffer, vertexBufferMemory);
// }
// void UpdateVertexBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkDeviceSize vertexBufferOffset, VkDeviceSize vertexBufferSize, void *vertices, VkBuffer vertexBuffer)
// {
//     VkBuffer stagingBuffer;
//     VkDeviceMemory stagingBufferMemory;
//     CreateBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

//     void *pData;
//     VkResult result = vkMapMemory(vkDevice, stagingBufferMemory, 0, vertexBufferSize, 0, &pData);
//     TryThrowVulkanError(result);
//     memcpy(pData, vertices, vertexBufferSize);
//     vkUnmapMemory(vkDevice, stagingBufferMemory);

//     CopyVkBuffer(graphicVkCommandPool, vkDevice, vkGraphicQueue, stagingBuffer, *pVertexBuffer, 0, vertexBufferSize);

//     vkDestroyBuffer(vkDevice, stagingBuffer, NULL);
//     vkFreeMemory(vkDevice, stagingBufferMemory, NULL);
// }

// void CreateInstanceBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkDeviceSize instanceBufferSize, VkBuffer *pInstanceBuffer, VkDeviceMemory *pInstanceBufferMemory)
// {
//     CreateBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pInstanceBuffer, pInstanceBufferMemory);
// }
// void DestroyInstanceBuffer(VkDevice vkDevice, VkBuffer instanceBuffer, VkDeviceMemory instanceBufferMemory)
// {
//     DestroyBuffer(vkDevice, instanceBuffer, instanceBufferMemory);
// }
// void UpdateInstanceBuffer(VkDevice vkDevice, VkDeviceSize instanceBufferOffset, VkDeviceSize instanceBufferSize, void *instance, VkDeviceMemory instanceBufferMemory)
// {
//     void *data;
//     vkMapMemory(vkDevice, instanceBufferMemory, instanceBufferOffset, instanceBufferSize, 0, &data);
//     memcpy(data, instance, instanceBufferSize);
//     vkUnmapMemory(vkDevice, instanceBufferMemory);
// }
