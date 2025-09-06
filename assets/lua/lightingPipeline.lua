require("vulkan")
require("gfx")
local lightingPipeline = {}
function lightingPipeline.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, assetsPath)
    local lightingPipelineSpvPaths = {
        assetsPath .. "/shaders/opaqueLighting.vert.spv",
        assetsPath .. "/shaders/opaqueLighting.frag.spv",
    }

    local vkPipelineInputAssemblyStateCreateInfo = {
        topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        primitiveRestartEnable = false,
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

    return gfx.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, lightingPipelineSpvPaths,
        nil, nil, vkPipelineInputAssemblyStateCreateInfo,
        gfx.defaultVkPipelineViewportStateCreateInfo,
        gfx.defaultVkPipelineRasterizationStateCreateInfo, gfx.defaultVkPipelineMultisampleStateCreateInfo,
        vkPipelineDepthStencilStateCreateInfo, vkPipelineColorBlendStateCreateInfo, gfx.defaultVkPipelineDynamicStateCreateInfo)
end

function lightingPipeline.destroyPipelinePtr(pGfxContext, pRenderPass)
    gfx.destroyPipelinePtr(pGfxContext, pRenderPass)
end

return lightingPipeline
