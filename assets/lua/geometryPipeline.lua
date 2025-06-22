local geometryPipeline = {}
local engine = require("engine")

geometryPipeline.createPipeline = function(pRenderPass, subpassIndex, pipelineIndex, meshLayout)
    local stages = {
        {
            stage = VK_SHADER_STAGE_VERTEX_BIT,
            shaderPath = "assets/shaders/geometry.vert",
            pName = "main",
        },
        {
            stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            shaderPath = "assets/shaders/geometry.frag",
            pName = "main",
        }
    }

    local inputAssemblyState = {
        topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
        primitiveRestartEnable = false,
    }

    local viewportState = {
        pViewports = {
            {
                x = 0.0,
                y = 0.0,
                width = 0,
                height = 0,
                minDepth = 0.0,
                maxDepth = 1.0,
            }
        },
        pScissors = {
            {
                offset = { x = 0, y = 0 },
                extent = { width = 0, height = 0 },
            }
        },
    }

    local rasterizationState = {
        depthClampEnable = false,
        rasterizerDiscardEnable = false,
        polygonMode = VK_POLYGON_MODE_POINT,
        cullMode = VK_CULL_MODE_NONE,
        frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        depthBiasEnable = false,
        depthBiasConstantFactor = 0.0,
        depthBiasClamp = 0.0,
        depthBiasSlopeFactor = 0.0,
        lineWidth = 1.0,
    }

    local multisampleState = {
        rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        sampleShadingEnable = false,
        minSampleShading = 0,
        sampleMask = 0,
        alphaToCoverageEnable = false,
        alphaToOneEnable = false,
    }

    local depthStencilState = {
        depthTestEnable = true,
        depthWriteEnable = true,
        depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
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
    }

    local colorBlendState = {
        logicOpEnable = false,
        logicOp = VK_LOGIC_OP_COPY,
        pAttachments = {
            {
                blendEnable = false,
                colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            }
        },
        blendConstants = { 0.0, 0.0, 0.0, 0.0 },
    }
    local dynamicState = {
        pDynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        },
    }

    local vkDescriptorSetLayoutCreateInfo = {
        bindings =
        {
            {
                binding = 0,
                descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                descriptorCount = 1,
                stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                pImmutableSamplers = {},
            }
        }
    }

    local vkDescriptorPoolSizes = {
        {
            type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            descriptorCount = 1,
        }
    }

    local pPipeline = engine.createPipeline(stages, meshLayout, inputAssemblyState, viewportState,
        rasterizationState, multisampleState, depthStencilState, colorBlendState, dynamicState,
        vkDescriptorSetLayoutCreateInfo, pRenderPass, subpassIndex, vkDescriptorPoolSizes, pipelineIndex)
    return pPipeline
end

geometryPipeline.destroyPipeline = function(pRenderPass, subpassIndex, pPipeline)
    engine.destroyPipeline(pRenderPass, subpassIndex, pPipeline)
end


return geometryPipeline
