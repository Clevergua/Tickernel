-- Example: How to use createImagePtr in Lua

require("assets/lua/vulkan") -- Load Vulkan constants

function createExampleTexture(gfxContext)
    -- Create a simple 4x4 RGBA texture with some data
    local width = 4
    local height = 4
    local depth = 1
    
    -- Create image extent
    local extent = {
        width = width,
        height = height,
        depth = depth
    }
    
    -- Create image data (4x4 RGBA, each pixel is 4 bytes)
    local imageData = {}
    for y = 1, height do
        for x = 1, width do
            -- Red component
            table.insert(imageData, 255)  -- R
            table.insert(imageData, 0)    -- G  
            table.insert(imageData, 0)    -- B
            table.insert(imageData, 255)  -- A
        end
    end
    
    -- Create image with data
    local textureImage = gfx.createImagePtr(
        gfxContext,
        extent,
        VK_FORMAT_R8G8B8A8_UNORM,           -- Format: RGBA8
        VK_IMAGE_TILING_OPTIMAL,             -- Optimal tiling
        VK_IMAGE_USAGE_TEXTURE_BIT,          -- Usage: texture (sampled + transfer dst)
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, -- Device local memory
        VK_IMAGE_ASPECT_COLOR_BIT,           -- Color aspect
        imageData                            -- Image data (optional)
    )
    
    return textureImage
end

function createEmptyRenderTarget(gfxContext, width, height)
    -- Create an empty image for use as render target
    local extent = {
        width = width,
        height = height,
        depth = 1
    }
    
    -- Create empty image (no data)
    local renderTarget = gfx.createImagePtr(
        gfxContext,
        extent,
        VK_FORMAT_R8G8B8A8_UNORM,                    -- Format: RGBA8
        VK_IMAGE_TILING_OPTIMAL,                      -- Optimal tiling
        VK_IMAGE_USAGE_RENDER_TARGET_BIT,             -- Usage: color attachment + transfer dst
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,          -- Device local memory
        VK_IMAGE_ASPECT_COLOR_BIT,                    -- Color aspect
        nil                                           -- No data (empty image)
    )
    
    return renderTarget
end

function createDepthBuffer(gfxContext, width, height)
    -- Create depth buffer
    local extent = {
        width = width,
        height = height,
        depth = 1
    }
    
    local depthBuffer = gfx.createImagePtr(
        gfxContext,
        extent,
        VK_FORMAT_D32_SFLOAT,                         -- 32-bit float depth
        VK_IMAGE_TILING_OPTIMAL,                      -- Optimal tiling
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,  -- Depth/stencil attachment
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,          -- Device local memory
        VK_IMAGE_ASPECT_DEPTH_BIT,                    -- Depth aspect
        nil                                           -- No data (empty)
    )
    
    return depthBuffer
end

-- Usage examples:
--[[
local gfxContext = ... -- Your graphics context

-- Create a texture with data
local texture = createExampleTexture(gfxContext)

-- Create empty render targets
local colorTarget = createEmptyRenderTarget(gfxContext, 1920, 1080)
local depthTarget = createDepthBuffer(gfxContext, 1920, 1080)

-- Don't forget to clean up
gfx.destroyImagePtr(gfxContext, texture)
gfx.destroyImagePtr(gfxContext, colorTarget)
gfx.destroyImagePtr(gfxContext, depthTarget)
--]]