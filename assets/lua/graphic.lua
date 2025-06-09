local engine = require("engine")

local graphic = {}

function graphic.createDeferredRenderPass(pAttachments)
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

    engine.createRenderPass(vkAttachmentDescriptions, pAttachments)
end

function graphic.destroyDeferredRenderPass()

end

function graphic.setUp()
    graphic.pColorAttachment = engine.createDynamicAttachment(VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    graphic.depthAttachment = engine.createDynamicAttachment(VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1)
end

function graphic.tearDown()
    engine.destroyDynamicAttachment(graphic.pColorAttachment)
end

return graphic
