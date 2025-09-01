require("vulkan")
local srp = {}
local deferredRenderPass = require("deferredRenderPass")

function srp.setup(pGfxContext, assetsPath)
    local depthVkFormat = gfx.getSupportedFormat(pGfxContext, {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
    }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    srp.pColorAttachment = gfx.createDynamicAttachmentPtr(pGfxContext, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    srp.pDepthAttachment = gfx.createDynamicAttachmentPtr(pGfxContext, depthVkFormat,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, 1)
    srp.pAlbedoAttachment = gfx.createDynamicAttachmentPtr(pGfxContext, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    srp.pNormalAttachment = gfx.createDynamicAttachmentPtr(pGfxContext, VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    srp.pSwapchainAttachment = gfx.getSwapchainAttachmentPtr(pGfxContext)

    srp.pMeshVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext,
        {
            { name = "position", size = 64, },
            { name = "color",    size = 64, },
            { name = "normal",   size = 64, },
        }
    )

    srp.pInstanceVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext,
        {
            instanceAttributeDescription = {
                { name = "model", size = 4, },
            }
        }
    );
    srp.pDeferredRenderPass = deferredRenderPass.createRenderPassPtr(pGfxContext, {
        srp.pColorAttachment,
        srp.pDepthAttachment,
        srp.pAlbedoAttachment,
        srp.pNormalAttachment,
        srp.pSwapchainAttachment
    }, assetsPath, srp.pMeshVertexInputLayout,  srp.pInstanceVertexInputLayout)
end

function srp.tearDown(pGfxContext)
    srp.pDeferredRenderPass = nil
    deferredRenderPass.destroyRenderPassPtr(pGfxContext, srp.pDeferredRenderPass)
    gfx.destroyVertexInputLayoutPtr(srp.pInstanceVertexInputLayout)
    gfx.destroyVertexInputLayoutPtr(srp.pMeshVertexInputLayout)
    gfx.destroyDynamicAttachmentPtr(pGfxContext, srp.pNormalAttachment)
    gfx.destroyDynamicAttachmentPtr(pGfxContext, srp.pAlbedoAttachment)
    gfx.destroyDynamicAttachmentPtr(pGfxContext, srp.pDepthAttachment)
    gfx.destroyDynamicAttachmentPtr(pGfxContext, srp.pColorAttachment)
end

return srp
