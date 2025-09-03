local geometryPipeline = require("geometryPipeline")
local lightingPipeline = require("lightingPipeline")
local postProcessPipeline = require("postProcessPipeline")
local deferredRenderPass = {}

function deferredRenderPass.setup(pGfxContext, pAttachments, assetsPath, pMeshVertexInputLayout,
                                  pInstanceVertexInputLayout, renderPassIndex)
    local colorAttachmentDescription = {
        samples = VK_SAMPLE_COUNT_1_BIT,
        loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    local depthAttachmentDescription = {
        samples = VK_SAMPLE_COUNT_1_BIT,
        loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    local albedoAttachmentDescription = {
        samples = VK_SAMPLE_COUNT_1_BIT,
        loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    local normalAttachmentDescription = {
        samples = VK_SAMPLE_COUNT_1_BIT,
        loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    local swapchainAttachmentDescription = {
        samples = VK_SAMPLE_COUNT_1_BIT,
        loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    local vkAttachmentDescriptions = {
        colorAttachmentDescription,
        depthAttachmentDescription,
        albedoAttachmentDescription,
        normalAttachmentDescription,
        swapchainAttachmentDescription,
    };

    local vkClearValues = {
        { 0.0,         0.0,        0.0, 1.0 },
        { depth = 1.0, stencil = 0 },
        { 0.0,         0.0,        0.0, 1.0 },
        { 0.0,         0.0,        0.0, 1.0 },
        { 0.0,         0.0,        0.0, 1.0 },
    };

    local geometrySubpassDescription = {
        pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        inputAttachmentCount = 0,
        pInputAttachments = {},
        colorAttachmentCount = 2,
        pColorAttachments = {
            { attachment = 2, layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
            { attachment = 3, layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
        },
        pResolveAttachments = {},
        pDepthStencilAttachment = { attachment = 1, layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
        preserveAttachmentCount = 0,
        pPreserveAttachments = {},
    }

    local ligthtingSubpassDescription = {
        pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        inputAttachmentCount = 3,
        pInputAttachments = {
            { attachment = 1, layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL },
            { attachment = 2, layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
            { attachment = 3, layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
        },
        colorAttachmentCount = 1,
        pColorAttachments = {
            { attachment = 0, layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
        },
        pResolveAttachments = {},
        pDepthStencilAttachment = nil,
        preserveAttachmentCount = 0,
        pPreserveAttachments = {},
    }

    local postProcessSubpassDescription = {
        pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        inputAttachmentCount = 1,
        pInputAttachments = {
            { attachment = 0, layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
        },
        colorAttachmentCount = 1,
        pColorAttachments = {
            { attachment = 4, layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
        },
        pResolveAttachments = {},
        pDepthStencilAttachment = nil,
        preserveAttachmentCount = 0,
        pPreserveAttachments = {},
    }

    local vkSubpassDescriptions = {
        geometrySubpassDescription,
        ligthtingSubpassDescription,
        postProcessSubpassDescription,
    }

    local spvPathsArray = {
        {},
        { assetsPath .. "/shaders/lighting.subpass.frag.spv" },
        { assetsPath .. "/shaders/postProcess.subpass.frag.spv" },
    }

    local vkSubpassDependencies = {
        {
            srcSubpass = VK_SUBPASS_EXTERNAL,
            dstSubpass = 0,
            srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        },
        {
            srcSubpass = 0,
            dstSubpass = 1,
            srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
            dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        },
        {
            srcSubpass = 1,
            dstSubpass = 2,
            srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
            dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        },
        {
            srcSubpass = 2,
            dstSubpass = VK_SUBPASS_EXTERNAL,
            srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        }
    }
    local pRenderPass = gfx.createRenderPassPtr(pGfxContext, vkAttachmentDescriptions, pAttachments, vkClearValues,
        vkSubpassDescriptions,
        spvPathsArray, vkSubpassDependencies, renderPassIndex)
    local pipelineIndex = 0
    local pGeometryPipeline = geometryPipeline.createPipelinePtr(pGfxContext, pRenderPass, pipelineIndex, assetsPath,
        pMeshVertexInputLayout, pInstanceVertexInputLayout)
    pipelineIndex = pipelineIndex + 1
    local pLightingPipeline = lightingPipeline.createPipelinePtr(pGfxContext, pRenderPass, pipelineIndex, assetsPath)
    pipelineIndex = pipelineIndex + 1
    local pPostProcessPipeline = postProcessPipeline.createPipelinePtr(pGfxContext, pRenderPass, pipelineIndex,
        assetsPath)

    deferredRenderPass.pGeometryMaterial = gfx.createPipelineMaterialPtr(pGfxContext, pGeometryPipeline)
    deferredRenderPass.pLightingMaterial = gfx.createPipelineMaterialPtr(pGfxContext, pLightingPipeline)
    deferredRenderPass.pPostProcessMaterial = gfx.createPipelineMaterialPtr(pGfxContext, pPostProcessPipeline)
    local instances = {
        {
            {
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0,
            },
        },
    }
    gfx.createInstancePtr(pGfxContext, pInstanceVertexInputLayout, instances)
    local vertices = {
        {
            {
                -0.5, -0.5, 0.0
            },
            {
                0xFF0000FF
            },
            {
                0x0
            }
        },
    }
    gfx.createMeshPtr(pGfxContext, pMeshVertexInputLayout, vertices, nil)
    deferredRenderPass.pRenderPass = pRenderPass
    deferredRenderPass.pGeometryPipeline = pGeometryPipeline
    deferredRenderPass.pLightingPipeline = pLightingPipeline
    deferredRenderPass.pPostProcessPipeline = pPostProcessPipeline
end

function deferredRenderPass.teardown(pGfxContext)
    deferredRenderPass.pPostProcessMaterial = nil
    postProcessPipeline.destroyPipelinePtr(pGfxContext, deferredRenderPass.pGeometryPipeline)
    deferredRenderPass.pLightingMaterial = nil
    lightingPipeline.destroyPipelinePtr(pGfxContext, deferredRenderPass.pLightingPipeline)
    deferredRenderPass.pGeometryMaterial = nil
    geometryPipeline.destroyPipelinePtr(pGfxContext, deferredRenderPass.pGeometryPipeline)

    gfx.destroyPipelineMaterialPtr(pGfxContext, deferredRenderPass.pPostProcessMaterial)
    gfx.destroyPipelineMaterialPtr(pGfxContext, deferredRenderPass.pLightingMaterial)
    gfx.destroyPipelineMaterialPtr(pGfxContext, deferredRenderPass.pGeometryMaterial)


    gfx.destroyRenderPassPtr(pGfxContext, deferredRenderPass.pRenderPass)

    deferredRenderPass.pRenderPass = nil
    deferredRenderPass.pGeometryPipeline = nil
    deferredRenderPass.pLightingPipeline = nil
    deferredRenderPass.pPostProcessPipeline = nil
end

return deferredRenderPass
