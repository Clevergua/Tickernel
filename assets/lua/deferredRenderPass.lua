local deferredRenderPass = {}
local engine = require("engine")
local geometryPipeline = require("geometryPipeline")

function deferredRenderPass.createRenderPass(pAttachments)
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

    local spvPathArrays = {
        {},
        {
            "assets/shaders/lighting.subpass.frag.spv"
        },
        {
            "assets/shaders/postProcess.subpass.frag.spv"
        },
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

    return engine.createRenderPass(vkAttachmentDescriptions, pAttachments, vkSubpassDescriptions, spvPathArrays,
        vkSubpassDependencies,
        0)
end

function deferredRenderPass.destroyRenderPass(pRenderPass)
    engine.destroyRenderPass(pRenderPass)
end

return deferredRenderPass
