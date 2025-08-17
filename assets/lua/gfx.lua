-- Document not code!
_G.gfx = {}

-- VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
function gfx.getSupportedFormat(pGfxContext, candidateCount, candidates, tiling, features)
    return VK_FORMAT_MAX_ENUM
end

-- RenderPass *createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t spvPathCount, const char **spvPaths, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex);
function gfx.createRenderPassPtr(pGfxContext, vkAttachmentDescriptions, pAttachments, vkClearValues,
                                 vkSubpassDescriptions,
                                 spvPathsArray, vkSubpassDependencies)
    local pRenderPass
    return pRenderPass
end

-- void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);
function gfx.destroyRenderPassPtr(pGfxContext, pRenderPass)
end

-- Attachment *createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler);
function gfx.createDynamicAttachmentPtr(pGfxContext, vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags,
                                        vkImageAspectFlags, scaler)
    local pAttachment
    return pAttachment
end

-- void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
function gfx.destroyDynamicAttachmentPtr(pGfxContext, pAttachment)
end

-- Attachment *createFixedAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height);
-- void destroyFixedAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
