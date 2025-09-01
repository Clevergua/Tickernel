-- Document not code!
_G.gfx = {}

-- VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

-- GfxContext *createGfxContextPtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, uint32_t spvPathCount, const char **spvPaths);
-- void waitGfxContextPtr(GfxContext *pGfxContext);
-- void updateGfxContextPtr(GfxContext *pGfxContext, VkExtent2D swapchainExtent);
-- void destroyGfxContextPtr(GfxContext *pGfxContext);

-- Attachment *createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkImageAspectFlags vkImageAspectFlags, float scaler);
-- void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
-- Attachment *createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height);
-- void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
-- Attachment *getSwapchainAttachmentPtr(GfxContext *pGfxContext);

-- VertexInputLayout *createVertexInputLayoutPtr(uint32_t attributeCount, const char **names, uint32_t *sizes);
-- void destroyVertexInputLayoutPtr(VertexInputLayout *pVertexInputLayout);

-- RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, VkClearValue *vkClearValues, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t *spvPathCounts, const char ***spvPathsArray, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies);
-- void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);
-- void insertRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t index);
-- void removeRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);

-- Pipeline *createPipelinePtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex, uint32_t spvPathCount, const char **spvPaths, VertexInputLayout *pMeshVertexInputLayout, VertexInputLayout *pInstanceVertexInputLayout, VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo, VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo, VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo, VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo, VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo, VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo);
-- void destroyPipelinePtr(GfxContext *pGfxContext, Pipeline *pPipeline);

-- DrawCall *addDrawCall(GfxContext *pGfxContext, Pipeline *pPipeline, Material *pMaterial, Mesh *pMesh, Instance *pInstance);
-- void removeDrawCall(GfxContext *pGfxContext, DrawCall *pDrawCall);
-- void clearDrawCalls(GfxContext *pGfxContext, Pipeline *pPipeline);

-- Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags);
-- void destroyImagePtr(GfxContext *pGfxContext, Image *pImage);

-- UniformBuffer *createUniformBufferPtr(GfxContext *pGfxContext, VkDeviceSize vkDeviceSize);
-- void destroyUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer);
-- void updateUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer, const void *data, VkDeviceSize vkDeviceSize);

-- Mesh *createMeshPtr(GfxContext *pGfxContext, VertexInputLayout *pMeshVertexInputLayout, void *vertices, uint32_t vertexCount, VkIndexType vkIndexType, void *indices, uint32_t indexCount);
-- void destroyMeshPtr(GfxContext *pGfxContext, Mesh *pMesh);

-- Instance *createInstancePtr(GfxContext *pGfxContext, VertexInputLayout *pVertexInputLayout, uint32_t instanceCount, void *instances);
-- void destroyInstancePtr(GfxContext *pGfxContext, Instance *pInstance);

-- Material *getGlobalMaterialPtr(GfxContext *pGfxContext);
-- Material *getSubpassMaterialPtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex);
-- Material *createPipelineMaterialPtr(GfxContext *pGfxContext, Pipeline *pPipeline);
-- void destroyPipelineMaterialPtr(GfxContext *pGfxContext, Material *pMaterial);

function gfx.getSupportedFormat(pGfxContext, candidateCount, candidates, tiling, features)
    return VK_FORMAT_MAX_ENUM
end

function gfx.createDynamicAttachmentPtr(pGfxContext, vkFormat, vkImageUsageFlags,
                                        vkImageAspectFlags, scaler)
    local pAttachment
    return pAttachment
end

function gfx.destroyDynamicAttachmentPtr(pGfxContext, pAttachment)
end

function gfx.createVertexInputLayoutPtr(meshLayout)
    local pMeshLayout
    return pMeshLayout
end

function gfx.destroyVertexInputLayoutPtr(pMeshLayout)
end

function gfx.createRenderPassPtr(pGfxContext, vkAttachmentDescriptions, inputAttachmentPtrs, vkClearValues,
                                 vkSubpassDescriptions,
                                 spvPathsArray, vkSubpassDependencies)
    local pRenderPass
    return pRenderPass
end

function gfx.destroyRenderPassPtr(pGfxContext, pRenderPass)
end

function gfx.insertRenderPassPtr(pGfxContext, pRenderPass, index)
end

function gfx.removeRenderPassPtr(pGfxContext, pRenderPass)
end

function gfx.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, spvPaths,
                               vertexAttributeDescriptions, instanceAttributeDescriptions,
                               vkPipelineInputAssemblyStateCreateInfo, vkPipelineViewportStateCreateInfo,
                               vkPipelineRasterizationStateCreateInfo, vkPipelineMultisampleStateCreateInfo,
                               vkPipelineDepthStencilStateCreateInfo, vkPipelineColorBlendStateCreateInfo,
                               vkPipelineDynamicStateCreateInfo)
    local pPipeline
    return pPipeline
end

function gfx.destroyPipelinePtr(pGfxContext, pPipeline)
end

function gfx.addDrawCall(pGfxContext, pPipeline, pMaterial, pMesh, pInstance)
    local pDrawCall
    return pDrawCall
end
function gfx.removeDrawCall(pGfxContext, pDrawCall)
end
function gfx.clearDrawCalls(pGfxContext, pPipeline)

end

-- Image *createImagePtr(GfxContext *pGfxContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags);
-- void destroyImagePtr(GfxContext *pGfxContext, Image *pImage);

-- UniformBuffer *createUniformBufferPtr(GfxContext *pGfxContext, VkDeviceSize vkDeviceSize);
-- void destroyUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer);
-- void updateUniformBufferPtr(GfxContext *pGfxContext, UniformBuffer *pUniformBuffer, const void *data, VkDeviceSize vkDeviceSize);

function gfx.createMeshPtr(pGfxContext, pMeshVertexInputLayout, vertices, indices)
    local pMesh
    return pMesh
end

function gfx.destroyMeshPtr(pGfxContext, pMesh)
end

function gfx.createInstancePtr(pGfxContext, pVertexInputLayout, instances)
    local pInstance
    return pInstance
end
function gfx.destroyInstancePtr(pGfxContext, pInstance)

end

function  gfx.getGlobalMaterialPtr(pGfxContext)
    local pMaterial
    return pMaterial
end
function gfx.getSubpassMaterialPtr(pGfxContext, pRenderPass, subpassIndex)
    local pMaterial
    return pMaterial
end
function gfx.createPipelineMaterialPtr(pGfxContext, pPipeline)

end
function gfx.destroyPipelineMaterialPtr(pGfxContext, pMaterial)

end


gfx.defaultVkPipelineViewportStateCreateInfo = {
    pViewports = {
        {
            x = 0.0,
            y = 0.0,
            width = 0.0,
            height = 0.0,
            minDepth = 0.0,
            maxDepth = 1.0,
        }
    },
    pScissors = {
        {
            offset = { x = 0, y = 0, },
            extent = { width = 0, height = 0, },
        }
    },
}
