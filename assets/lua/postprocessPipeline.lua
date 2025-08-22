local postprocessPipeline = {}
function postprocessPipeline.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, assetsPath)
    local postprocessPipelineSpvPaths = {
        assetsPath .. "/shaders/spv/postprocess.vert.spv",
        assetsPath .. "/shaders/spv/postprocess.frag.spv",
    }

    local vkPipelineInputAssemblyStateCreateInfo = {
        topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        primitiveRestartEnable = false,
    }

    local vkPipelineViewportStateCreateInfo = {
        pViewports = {
            {
                x = 0.0,
                y = 0.0,
                width = 0.0,
                height = 0.0,
                minDepth = 0.0,
                maxDepth = 1.0,
            },
        },
        pScissors = {
            {
                offset = { 0, 0 },
                extent = { 0, 0 },
            },
        },
    }

    local vkPipelineRasterizationStateCreateInfo = {
        depthClampEnable = false,
        rasterizerDiscardEnable = false,
        polygonMode = VK_POLYGON_MODE_FILL,
        cullMode = VK_CULL_MODE_FRONT_BIT,
        frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        depthBiasEnable = false,
        depthBiasConstantFactor = 0.0,
        depthBiasClamp = 0.0,
        depthBiasSlopeFactor = 0.0,
        lineWidth = 1.0,
    }

    local vkPipelineMultisampleStateCreateInfo = {
        rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        sampleShadingEnable = false,
        minSampleShading = 0,
        pSampleMask = nil,
        alphaToCoverageEnable = false,
        alphaToOneEnable = false,
    }
    local vkPipelineDepthStencilStateCreateInfo = {
        depthTestEnable = false,
        depthWriteEnable = false,
        depthCompareOp = VK_COMPARE_OP_ALWAYS,
        depthBoundsTestEnable = false,
        stencilTestEnable = false,
        front = {
            failOp = VK_STENCIL_OP_KEEP,
            passOp = VK_STENCIL_OP_KEEP,
            depthFailOp = VK_STENCIL_OP_KEEP,
            compareOp = VK_COMPARE_OP_ALWAYS,
            compareMask = 0xFF,
            writeMask = 0xFF,
            reference = 0,
        },
        back = {
            failOp = VK_STENCIL_OP_KEEP,
            passOp = VK_STENCIL_OP_KEEP,
            depthFailOp = VK_STENCIL_OP_KEEP,
            compareOp = VK_COMPARE_OP_ALWAYS,
            compareMask = 0xFF,
            writeMask = 0xFF,
            reference = 0,
        },
        minDepthBounds = 0.0,
        maxDepthBounds = 1.0,
    }
    local vkPipelineColorBlendStateCreateInfo = {
        logicOpEnable = false,
        logicOp = VK_LOGIC_OP_COPY,
        pAttachments = {
            {
                blendEnable = false,
                srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                colorBlendOp = VK_BLEND_OP_ADD,
                srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                alphaBlendOp = VK_BLEND_OP_ADD,
                colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
            }
        },
        blendConstants = { 0.0, 0.0, 0.0, 0.0 },
    }
    local vkPipelineDynamicStateCreateInfo = {
        pDynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        },
    }

    return gfx.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, postprocessPipelineSpvPaths,
        nil, vkPipelineInputAssemblyStateCreateInfo, vkPipelineViewportStateCreateInfo,
        vkPipelineRasterizationStateCreateInfo, vkPipelineMultisampleStateCreateInfo,
        vkPipelineDepthStencilStateCreateInfo, vkPipelineColorBlendStateCreateInfo, vkPipelineDynamicStateCreateInfo)
end

function postprocessPipeline.destroyPipelinePtr(pGfxContext, pRenderPass)
    gfx.destroyPipelinePtr(pGfxContext, pRenderPass)
end

return postprocessPipeline
