-- Document not code!
_G.gfx = {}

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

function gfx.getSwapchainAttachmentPtr(pGfxContext)
    local pAttachment
    return pAttachment
end

function gfx.createVertexInputLayoutPtr(pGfxContext, layout)
    local pLayout
    return pLayout
end

function gfx.destroyVertexInputLayoutPtr(pGfxContext, pLayout)
end

function gfx.createRenderPassPtr(pGfxContext, vkAttachmentDescriptions, inputAttachmentPtrs, vkClearValues,
                                 vkSubpassDescriptions,
                                 spvPathsArray, vkSubpassDependencies, renderPassIndex)
    local pRenderPass
    return pRenderPass
end

function gfx.destroyRenderPassPtr(pGfxContext, pRenderPass)
end

function gfx.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, spvPaths,
                               pMeshVertexInputLayout, pInstanceVertexInputLayout,
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

function gfx.getGlobalMaterialPtr(pGfxContext)
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
gfx.defaultVkPipelineMultisampleStateCreateInfo = {
    rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    sampleShadingEnable = false,
    minSampleShading = 0,
    pSampleMask = nil,
    alphaToCoverageEnable = false,
    alphaToOneEnable = false,
}

gfx.defaultVkPipelineDynamicStateCreateInfo = {
    pDynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    },
}
