require("vulkan")
local srp = {}
local deferredRenderPass = require("deferredRenderPass")
-- local geometryPipeline = require("geometryPipeline")
-- local lightingPipeline = require("lightingPipeline")
-- local postProgressPipeline = require("postProgressPipeline")

function srp.setUp(pGfxContext, assetsPath)
    local depthVkFormat = gfx.getSupportedFormat(pGfxContext, {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
    }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    srp.pColorAttachment = gfx.createDynamicAttachmentPtr(pGfxContext, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    srp.pDepthAttachment = gfx.createDynamicAttachmentPtr(pGfxContext, depthVkFormat,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, 1)
    srp.pAlbedoAttachment = gfx.createDynamicAttachmentPtr(pGfxContext, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    srp.pNormalAttachment = gfx.createDynamicAttachmentPtr(pGfxContext, VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    srp.pSwapchainAttachment = gfx.getSwapchainAttachmentPtr(pGfxContext)

    srp.pDeferredRenderPass = deferredRenderPass.createRenderPassPtr(pGfxContext, {
        srp.pColorAttachment,
        srp.pDepthAttachment,
        srp.pAlbedoAttachment,
        srp.pNormalAttachment,
        srp.pSwapchainAttachment
    }, assetsPath)
    -- local meshLayout = {
    --     vertexLayouts = {
    --         {
    --             name = "inputPosition",
    --             size = 12,
    --         },
    --         {
    --             name = "inputColor",
    --             size = 16
    --         },
    --         {
    --             name = "inputNormal",
    --             size = 12,
    --         }
    --     },
    --     instanceLayouts = {
    --         {
    --             name = "inputModel",
    --             size = 64,
    --         }
    --     }
    -- }
    -- gfx.pGeometryPipeline = geometryPipeline.createPipeline(gfx.pDeferredRenderPass, 0, 0)

    -- -- gfx.pLightingPipeline = lightingPipeline.createPipeline(gfx.pDeferredRenderPass, 1, 0)
    -- -- gfx.pPostProgressPipeline = postProgressPipeline.createPipeline(gfx.pDeferredRenderPass, 2, 0)

    -- local resources = {

    -- }
    -- gfx.createMaterial(gfx.pGeometryPipeline, resources)
end

function srp.tearDown(pGfxContext)
    -- postProgressPipeline.destroyPipeline(gfx.pDeferredRenderPass, 2, gfx.pPostProgressPipeline)
    -- lightingPipeline.destroyPipeline(gfx.pDeferredRenderPass, 1, gfx.gfxlightingPipeline)
    -- geometryPipeline.destroyPipeline(gfx.pDeferredRenderPass, 0, gfx.pGeometryPipeline)
    deferredRenderPass.destroyRenderPass(gfx.pDeferredRenderPass)

    gfx.destroyDynamicAttachmentPtr(pGfxContext, srp.pNormalAttachment)
    gfx.destroyDynamicAttachmentPtr(pGfxContext, srp.pAlbedoAttachment)
    gfx.destroyDynamicAttachmentPtr(pGfxContext, srp.pDepthAttachment)
    gfx.destroyDynamicAttachmentPtr(pGfxContext, srp.pColorAttachment)
end

return srp
