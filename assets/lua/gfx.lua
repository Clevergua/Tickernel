-- Document not code!
-- This file provides type hints and documentation for the gfx module
-- Actual implementations are provided by C bindings
-- Initialize gfx table if not already loaded by C bindings
_G.gfx = _G.gfx or {}

-- Type constants (will be overridden by C bindings if available)
TYPE_UINT8 = TYPE_UINT8 or 0
TYPE_UINT16 = TYPE_UINT16 or 1
TYPE_UINT32 = TYPE_UINT32 or 2
TYPE_UINT64 = TYPE_UINT64 or 3
TYPE_INT8 = TYPE_INT8 or 4
TYPE_INT16 = TYPE_INT16 or 5
TYPE_INT32 = TYPE_INT32 or 6
TYPE_INT64 = TYPE_INT64 or 7
TYPE_FLOAT = TYPE_FLOAT or 8
TYPE_DOUBLE = TYPE_DOUBLE or 9

-- Function declarations for IDE support (only used if C binding not available)
if not gfx.getSupportedFormat then
    function gfx.getSupportedFormat(pGfxContext, candidates, tiling, features)
        error("gfx.getSupportedFormat: C binding not loaded")
    end
end

if not gfx.createDynamicAttachmentPtr then
    function gfx.createDynamicAttachmentPtr(pGfxContext, vkFormat, vkImageUsageFlags, vkImageAspectFlags, scaler)
        error("gfx.createDynamicAttachmentPtr: C binding not loaded")
    end
end

if not gfx.destroyDynamicAttachmentPtr then
    function gfx.destroyDynamicAttachmentPtr(pGfxContext, pAttachment)
        error("gfx.destroyDynamicAttachmentPtr: C binding not loaded")
    end
end

if not gfx.getSwapchainAttachmentPtr then
    function gfx.getSwapchainAttachmentPtr(pGfxContext)
        error("gfx.getSwapchainAttachmentPtr: C binding not loaded")
    end
end

if not gfx.createVertexInputLayoutPtr then
    function gfx.createVertexInputLayoutPtr(pGfxContext, format)
        error("gfx.createVertexInputLayoutPtr: C binding not loaded")
    end
end

if not gfx.destroyVertexInputLayoutPtr then
    function gfx.destroyVertexInputLayoutPtr(pGfxContext, pLayout)
        error("gfx.destroyVertexInputLayoutPtr: C binding not loaded")
    end
end

if not gfx.createRenderPassPtr then
    function gfx.createRenderPassPtr(pGfxContext, vkAttachmentDescriptions, inputAttachmentPtrs, vkClearValues, vkSubpassDescriptions, spvPathsArray, vkSubpassDependencies, renderPassIndex)
        error("gfx.createRenderPassPtr: C binding not loaded")
    end
end

if not gfx.destroyRenderPassPtr then
    function gfx.destroyRenderPassPtr(pGfxContext, pRenderPass)
        error("gfx.destroyRenderPassPtr: C binding not loaded")
    end
end

if not gfx.createPipelinePtr then
    function gfx.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, spvPaths, pMeshVertexInputLayout, pInstanceVertexInputLayout, vkPipelineInputAssemblyStateCreateInfo, vkPipelineViewportStateCreateInfo, vkPipelineRasterizationStateCreateInfo, vkPipelineMultisampleStateCreateInfo, vkPipelineDepthStencilStateCreateInfo, vkPipelineColorBlendStateCreateInfo, vkPipelineDynamicStateCreateInfo)
        error("gfx.createPipelinePtr: C binding not loaded")
    end
end

if not gfx.destroyPipelinePtr then
    function gfx.destroyPipelinePtr(pGfxContext, pPipeline)
        error("gfx.destroyPipelinePtr: C binding not loaded")
    end
end

if not gfx.createDrawCallPtr then
    function gfx.createDrawCallPtr(pGfxContext, pMaterial, pMesh, pInstance)
        error("gfx.createDrawCallPtr: C binding not loaded")
    end
end

if not gfx.destroyDrawCallPtr then
    function gfx.destroyDrawCallPtr(pGfxContext, pDrawCall)
        error("gfx.destroyDrawCallPtr: C binding not loaded")
    end
end

if not gfx.insertDrawCallPtr then
    function gfx.insertDrawCallPtr(pDrawCall, index)
        error("gfx.insertDrawCallPtr: C binding not loaded")
    end
end

if not gfx.removeDrawCallPtr then
    function gfx.removeDrawCallPtr(pDrawCall)
        error("gfx.removeDrawCallPtr: C binding not loaded")
    end
end

if not gfx.removeDrawCallAtIndex then
    function gfx.removeDrawCallAtIndex(pPipeline, index)
        error("gfx.removeDrawCallAtIndex: C binding not loaded")
    end
end

if not gfx.getDrawCallAtIndex then
    function gfx.getDrawCallAtIndex(pPipeline, index)
        error("gfx.getDrawCallAtIndex: C binding not loaded")
    end
end

if not gfx.getDrawCallCount then
    function gfx.getDrawCallCount(pPipeline)
        error("gfx.getDrawCallCount: C binding not loaded")
    end
end

if not gfx.createUniformBufferPtr then
    function gfx.createUniformBufferPtr(pGfxContext, format, buffer)
        error("gfx.createUniformBufferPtr: C binding not loaded")
    end
end

if not gfx.destroyUniformBufferPtr then
    function gfx.destroyUniformBufferPtr(pGfxContext, pUniformBuffer)
        error("gfx.destroyUniformBufferPtr: C binding not loaded")
    end
end

if not gfx.updateUniformBufferPtr then
    function gfx.updateUniformBufferPtr(pGfxContext, pUniformBuffer, format, buffer, size)
        error("gfx.updateUniformBufferPtr: C binding not loaded")
    end
end

if not gfx.createInstancePtr then
    function gfx.createInstancePtr(pGfxContext, pVertexInputLayout, format, instances)
        error("gfx.createInstancePtr: C binding not loaded")
    end
end

if not gfx.destroyInstancePtr then
    function gfx.destroyInstancePtr(pGfxContext, pInstance)
        error("gfx.destroyInstancePtr: C binding not loaded")
    end
end

if not gfx.createMeshPtrWithData then
    function gfx.createMeshPtrWithData(pGfxContext, pMeshVertexInputLayout, format, vertices, indexType, indices)
        error("gfx.createMeshPtrWithData: C binding not loaded")
    end
end

if not gfx.updateMeshPtr then
    function gfx.updateMeshPtr(pGfxContext, pMesh, format, vertices, indexType, indices)
        error("gfx.updateMeshPtr: C binding not loaded")
    end
end

if not gfx.destroyMeshPtr then
    function gfx.destroyMeshPtr(pGfxContext, pMesh)
        error("gfx.destroyMeshPtr: C binding not loaded")
    end
end

if not gfx.getGlobalMaterialPtr then
    function gfx.getGlobalMaterialPtr(pGfxContext)
        error("gfx.getGlobalMaterialPtr: C binding not loaded")
    end
end

if not gfx.getSubpassMaterialPtr then
    function gfx.getSubpassMaterialPtr(pGfxContext, pRenderPass, subpassIndex)
        error("gfx.getSubpassMaterialPtr: C binding not loaded")
    end
end

if not gfx.createPipelineMaterialPtr then
    function gfx.createPipelineMaterialPtr(pGfxContext, pPipeline)
        error("gfx.createPipelineMaterialPtr: C binding not loaded")
    end
end

if not gfx.destroyPipelineMaterialPtr then
    function gfx.destroyPipelineMaterialPtr(pGfxContext, pMaterial)
        error("gfx.destroyPipelineMaterialPtr: C binding not loaded")
    end
end

if not gfx.updateMaterialPtr then
    function gfx.updateMaterialPtr(pGfxContext, pMaterial, inputBindings)
        error("gfx.updateMaterialPtr: C binding not loaded")
    end
end

gfx.defaultVkPipelineViewportStateCreateInfo = {
    pViewports = {{
        x = 0.0,
        y = 0.0,
        width = 0.0,
        height = 0.0,
        minDepth = 0.0,
        maxDepth = 1.0,
    }},
    pScissors = {{
        offset = {
            x = 0,
            y = 0,
        },
        extent = {
            width = 0,
            height = 0,
        },
    }},
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
    pDynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
}

gfx.defaultVkPipelineRasterizationStateCreateInfo = {
    depthClampEnable = false,
    rasterizerDiscardEnable = false,
    polygonMode = VK_POLYGON_MODE_FILL,
    cullMode = VK_CULL_MODE_NONE,
    frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    depthBiasEnable = false,
    depthBiasConstantFactor = 0.0,
    depthBiasClamp = 0.0,
    depthBiasSlopeFactor = 0.0,
    lineWidth = 1.0,
}
