local engine = require("engine")
local deferredRenderPass = require("deferredRenderPass")
local geometryPipeline = require("geometryPipeline")
-- local lightingPipeline = require("lightingPipeline")
-- local postProgressPipeline = require("postProgressPipeline")
local graphic = {}


function graphic.setUp()
    local depthVkFormat = engine.findSupportedFormat({
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    graphic.pColorAttachment = engine.createDynamicAttachment(VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    graphic.pDepthAttachment = engine.createDynamicAttachment(depthVkFormat,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, 1)
    graphic.pAlbedoAttachment = engine.createDynamicAttachment(VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    graphic.pNormalAttachment = engine.createDynamicAttachment(VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    graphic.pSwapchainAttachment = engine.getSwapchainAttachment()

    graphic.pDeferredRenderPass = deferredRenderPass.createRenderPass({
        graphic.pColorAttachment,
        graphic.pDepthAttachment,
        graphic.pAlbedoAttachment,
        graphic.pNormalAttachment,
        graphic.pSwapchainAttachment
    })


    graphic.pGeometryPipeline = geometryPipeline.createPipeline(graphic.pDeferredRenderPass, 0, 0)
    -- graphic.pLightingPipeline = lightingPipeline.createPipeline(graphic.pDeferredRenderPass, 1, 0)
    -- graphic.pPostProgressPipeline = postProgressPipeline.createPipeline(graphic.pDeferredRenderPass, 2, 0)
    
    engine.createMaterial("assets/materials/geometry.material", graphic.pGeometryPipeline, 0)
end

function graphic.tearDown()
    -- postProgressPipeline.destroyPipeline(graphic.pDeferredRenderPass, 2, graphic.pPostProgressPipeline)
    -- lightingPipeline.destroyPipeline(graphic.pDeferredRenderPass, 1, graphic.graphiclightingPipeline)
    geometryPipeline.destroyPipeline(graphic.pDeferredRenderPass, 0, graphic.pGeometryPipeline)

    deferredRenderPass.destroyRenderPass(graphic.pDeferredRenderPass)

    engine.destroyDynamicAttachment(graphic.pNormalAttachment)
    engine.destroyDynamicAttachment(graphic.pAlbedoAttachment)
    engine.destroyDynamicAttachment(graphic.pDepthAttachment)
    engine.destroyDynamicAttachment(graphic.pColorAttachment)
end

return graphic
