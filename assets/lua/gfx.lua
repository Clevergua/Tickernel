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


-- Pipeline *createPipelinePtr(GfxContext *pGfxContext, RenderPass *pRenderPass, uint32_t subpassIndex, uint32_t spvPathCount, const char **spvPaths, VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo, VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo, VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo, VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo, VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo, VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo, VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo)
function gfx.createPipelinePtr(pGfxContext, pRenderPass, subpassIndex, spvPaths, vkPipelineVertexInputStateCreateInfo,
                               vkPipelineInputAssemblyStateCreateInfo, vkPipelineViewportStateCreateInfo,
                               vkPipelineRasterizationStateCreateInfo, vkPipelineMultisampleStateCreateInfo,
                               vkPipelineDepthStencilStateCreateInfo, vkPipelineColorBlendStateCreateInfo,
                               vkPipelineDynamicStateCreateInfo)
    local pPipeline
    return pPipeline
end

-- void destroyPipelinePtr(GfxContext *pGfxContext, Pipeline *pPipeline);
function gfx.destroyPipelinePtr(pGfxContext, pPipeline)
end

function gfx.createMeshPtr(pGfxContext, vertices, indices)
    local pMesh
    return pMesh
end

function gfx.destroyMeshPtr(pGfxContext, pMesh)
end

gfx.defaultVkPipelineViewportStateCreateInfo = {
    pViewports = {
        {
            x = 0.0,
            y = 0.0,
            width = 0.0,
            height = 0.0,
            minDepth = 0.0,
            maxDepth = 1.0,
        }
    },
    pScissors = {
        {
            offset = { x = 0, y = 0, },
            extent = { width = 0, height = 0, },
        }
    },
}
