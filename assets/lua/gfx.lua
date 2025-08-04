require("vulkan")
local gfx = {}
-- VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
function gfx.getSupportedFormat(pGfxContext, candidateCount, candidates, tiling, features)
    local vkFormat = VK_FORMAT_MAX_ENUM
    return vkFormat
end

-- Attachment *createDynamicAttachmentPtr(GfxContext *pGfxContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler);
function gfx.createDynamicAttachmentPtr(pGfxContext, vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags,
                                        vkImageAspectFlags, scaler)
    local pAttachment = nil
    return pAttachment
end

-- void destroyDynamicAttachmentPtr(GfxContext *pGfxContext, Attachment *pAttachment);
function gfx.destroyDynamicAttachmentPtr(pGfxContext, pAttachment)

end

return gfx
