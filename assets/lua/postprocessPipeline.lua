require("vulkan")
require("gfx")
local postprocessPipeline = {}
function postprocessPipeline.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, assetsPath)
    local postprocessPipelineSpvPaths = {
        assetsPath .. "/shaders/postprocess.vert.spv",
        assetsPath .. "/shaders/postprocess.frag.spv",
    }

    local vkPipelineInputAssemblyStateCreateInfo = {
        topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        primitiveRestartEnable = false,
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

    local vkPipelineDepthStencilStateCreateInfo = {
        depthTestEnable = false,
        depthWriteEnable = false,
        depthCompareOp = VK_COMPARE_OP_ALWAYS,
        depthBoundsTestEnable = false,
        stencilTestEnable = false,
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
                colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT,
            }
        },
        blendConstants = { 0.0, 0.0, 0.0, 0.0 },
    }
    return gfx.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, postprocessPipelineSpvPaths,
        nil, nil, vkPipelineInputAssemblyStateCreateInfo,
        gfx.defaultVkPipelineViewportStateCreateInfo,
        vkPipelineRasterizationStateCreateInfo, gfx.defaultVkPipelineMultisampleStateCreateInfo,
        vkPipelineDepthStencilStateCreateInfo, vkPipelineColorBlendStateCreateInfo, gfx.defaultVkPipelineDynamicStateCreateInfo)
end

function postprocessPipeline.destroyPipelinePtr(pGfxContext, pRenderPass)
    gfx.destroyPipelinePtr(pGfxContext, pRenderPass)
end

return postprocessPipeline
