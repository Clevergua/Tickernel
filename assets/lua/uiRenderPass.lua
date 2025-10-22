local gfx = require("gfx")
local uiPipeline = require("uiPipeline")
local uiRenderPass = {}

function uiRenderPass.setup(pGfxContext, pSwapchainAttachment, assetsPath, pUIVertexInputLayout)
    local swapchainAttachmentDescription = {
        samples = VK_SAMPLE_COUNT_1_BIT,
        loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    local vkAttachmentDescriptions = {swapchainAttachmentDescription};

    local vkClearValues = {{0.0, 0.0, 0.0, 1.0}};

    local uiSubpassDescription = {
        pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        pInputAttachments = {},
        pColorAttachments = {{
            attachment = 0,
            layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        }},
        pResolveAttachments = {},
        pDepthStencilAttachment = nil,
        pPreserveAttachments = {},
    }

    local vkSubpassDescriptions = {uiSubpassDescription}

    local spvPathsArray = {{}}

    local vkSubpassDependencies = {{
        srcSubpass = VK_SUBPASS_EXTERNAL,
        dstSubpass = 0,
        srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
        dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    }, {
        srcSubpass = 0,
        dstSubpass = VK_SUBPASS_EXTERNAL,
        srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
        dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    }}

    uiRenderPass.pRenderPass = gfx.createRenderPassPtr(pGfxContext, vkAttachmentDescriptions, {pSwapchainAttachment}, vkClearValues, vkSubpassDescriptions, spvPathsArray, vkSubpassDependencies, 0)
    uiRenderPass.pPipeline = uiPipeline.createPipelinePtr(pGfxContext, uiRenderPass.pRenderPass, 0, assetsPath, pUIVertexInputLayout)
    print(pUIVertexInputLayout)
end

function uiRenderPass.teardown(pGfxContext)
    uiPipeline.destroyPipelinePtr(pGfxContext, uiRenderPass.pPipeline)
    gfx.destroyRenderPassPtr(pGfxContext, uiRenderPass.pRenderPass)
    uiRenderPass.pRenderPass = nil
    uiRenderPass.pPipeline = nil
end

return uiRenderPass
