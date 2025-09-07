local tknRenderPipeline = require("tknRenderPipeline")
local tknEngine = {}

function tknEngine.start(pGfxContext, assetsPath)
    print("Lua start")
    tknEngine.assetsPath = assetsPath
    -- binding = 0
    tknEngine.vertexFormat = {{
        name = "position",
        type = TYPE_FLOAT,
        count = 3,
    }, {
        name = "color",
        type = TYPE_UINT8,
        count = 4,
    }, {
        name = "normal",
        type = TYPE_UINT32,
        count = 1,
    }}
    tknEngine.instanceFormat = {{
        name = "model",
        type = TYPE_FLOAT,
        count = 16,
    }}
    tknEngine.pMeshVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, tknEngine.vertexFormat)
    tknEngine.pInstanceVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, tknEngine.instanceFormat);
    tknRenderPipeline.setup(pGfxContext, assetsPath, tknEngine.pMeshVertexInputLayout, tknEngine.pInstanceVertexInputLayout)
    tknEngine.pGlobalMaterial = gfx.getGlobalMaterialPtr(pGfxContext)

    tknEngine.globalUniformBufferFormat = {{
        name = "view",
        type = TYPE_FLOAT,
        count = 16,
    }, {
        name = "proj",
        type = TYPE_FLOAT,
        count = 16,
    }, {
        name = "inv_view_proj",
        type = TYPE_FLOAT,
        count = 16,
    }, {
        name = "pointSizeFactor",
        type = TYPE_FLOAT,
        count = 1,
    }, {
        name = "time",
        type = TYPE_FLOAT,
        count = 1,
    }, {
        name = "frameCount",
        type = TYPE_INT32,
        count = 1,
    }, {
        name = "near",
        type = TYPE_FLOAT,
        count = 1,
    }, {
        name = "far",
        type = TYPE_FLOAT,
        count = 1,
    }, {
        name = "fov",
        type = TYPE_FLOAT,
        count = 1,
    }}
    local pGlobalUniformBuffer = {
        -- View matrix: camera at (5, 5, 5) looking at (0, 0, 0) with up vector (0, 1, 0)
        view = {0.7071, -0.4082, 0.5774, 0, 0, 0.8165, 0.5774, 0, -0.7071, -0.4082, 0.5774, 0, 0, 0, -8.6603, 1},
        -- Perspective projection matrix: 45° FOV, 16:9 aspect ratio, near=0.1, far=100.0
        proj = {1.3584, 0, 0, 0, 0, 2.4142, 0, 0, 0, 0, -1.0020, -1, 0, 0, -0.2002, 0},
        -- Inverse view-projection matrix (computed from the above matrices)
        inv_view_proj = {0.5206, 0, -0.5206, 0, -0.3007, 0.6013, -0.3007, 0, 0.0231, 0.0231, 0.0231, 0, 2.3077, 4.3301, 2.3077, 43.301},
        pointSizeFactor = 1000.0,
        time = 0.0,
        frameCount = 0,
        near = 0.1,
        far = 100.0,
        fov = 90.0,
    }
    tknEngine.pGlobalUniformBuffer = gfx.createUniformBufferPtr(pGfxContext, tknEngine.globalUniformBufferFormat, pGlobalUniformBuffer)
    local inputBindings = {{
        vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        pUniformBuffer = tknEngine.pGlobalUniformBuffer,
        binding = 0,
    }}
    tknEngine.pGlobalMaterialPtr = gfx.getGlobalMaterialPtr(pGfxContext)
    gfx.updateMaterialPtr(pGfxContext, tknEngine.pGlobalMaterialPtr, inputBindings)

    -- -- Create lights uniform buffer for lighting subpass
    -- tknEngine.lightsUniformBufferFormat = {{
    --     -- DirectionalLight: direction(vec3) + strength(float) + color(vec3) + padding(float)
    --     name = "directionalLight_direction",
    --     type = TYPE_FLOAT,
    --     count = 3,
    -- }, {
    --     name = "directionalLight_strength", 
    --     type = TYPE_FLOAT,
    --     count = 1,
    -- }, {
    --     name = "directionalLight_color",
    --     type = TYPE_FLOAT,
    --     count = 3,
    -- }, {
    --     name = "directionalLight_padding",
    --     type = TYPE_FLOAT,
    --     count = 1,
    -- }, {
    --     name = "pointLightCount",
    --     type = TYPE_INT32,
    --     count = 1,
    -- }, {
    --     -- PointLight array padding to align to 16 bytes
    --     name = "pointLightArray_padding",
    --     type = TYPE_INT32,
    --     count = 3,
    -- }}

    -- local pLightsUniformBuffer = {
    --     directionalLight_direction = {0.5, -1.0, 0.3}, -- Light coming from upper right
    --     directionalLight_strength = 1.0,
    --     directionalLight_color = {1.0, 1.0, 0.9}, -- Warm white light
    --     directionalLight_padding = 0.0,
    --     pointLightCount = 0, -- No point lights for now
    --     pointLightArray_padding = {0, 0, 0},
    -- }

    -- tknEngine.pLightsUniformBuffer = gfx.createUniformBufferPtr(pGfxContext, tknEngine.lightsUniformBufferFormat, pLightsUniformBuffer)

    -- Get the lighting subpass material and update it with lights uniform buffer
    -- local deferredRenderPass = tknRenderPipeline.deferredRenderPass
    -- tknEngine.pLightingMaterial = gfx.getSubpassMaterialPtr(pGfxContext, deferredRenderPass.pRenderPass, 1) -- Lighting is subpass 1
    -- local lightingInputBindings = {{
    --     vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    --     pUniformBuffer = tknEngine.pLightsUniformBuffer,
    --     binding = 3,
    -- }}
    -- gfx.updateMaterialPtr(pGfxContext, tknEngine.pLightingMaterial, lightingInputBindings)

    local vertices = {
        -- Triangle vertices
        position = {-1.0, -1.0, 0.0, -- Bottom left
        1.0, -1.0, 0.0, -- Bottom right
        0.0, 1.0, 0.0 -- Top center
        },
        color = {255, 0, 0, 255, -- Red
        0, 255, 0, 255, -- Green  
        0, 0, 255, 255 -- Blue
        },
        normal = {0x0, -- Front facing normal (encoded)
        0x0, 0x0},
    }

    tknEngine.pMesh = gfx.createMeshPtr(pGfxContext, tknEngine.pMeshVertexInputLayout, tknEngine.vertexFormat, vertices, nil)
    local instances = {
        model = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    }
    tknEngine.pInstance = gfx.createInstancePtr(pGfxContext, tknEngine.pInstanceVertexInputLayout, tknEngine.instanceFormat, instances)

    local deferredRenderPass = tknRenderPipeline.deferredRenderPass
    tknEngine.pDrawCall = gfx.addDrawCallPtr(pGfxContext, deferredRenderPass.pGeometryPipeline, deferredRenderPass.pGeometryMaterial, tknEngine.pMesh, tknEngine.pInstance)
end

function tknEngine.stop(pGfxContext)
    print("Lua stop")

    -- First remove drawcalls
    gfx.removeDrawCallPtr(pGfxContext, tknEngine.pDrawCall)
    tknEngine.pDrawCall = nil

    -- Then destroy mesh and instance
    gfx.destroyInstancePtr(pGfxContext, tknEngine.pInstance)
    tknEngine.pInstance = nil
    gfx.destroyMeshPtr(pGfxContext, tknEngine.pMesh)
    tknEngine.pMesh = nil

    -- Clear material references before destroying pipeline
    tknEngine.pGlobalMaterial = nil
    tknEngine.pLightingMaterial = nil

    -- Destroy render pipeline (this will destroy materials that use the uniform buffers)
    tknRenderPipeline.teardown(pGfxContext)

    -- Now it's safe to destroy the uniform buffers
    gfx.destroyUniformBufferPtr(pGfxContext, tknEngine.pGlobalUniformBuffer)
    tknEngine.pGlobalUniformBuffer = nil
    gfx.destroyUniformBufferPtr(pGfxContext, tknEngine.pLightsUniformBuffer)
    tknEngine.pLightsUniformBuffer = nil

    gfx.destroyVertexInputLayoutPtr(pGfxContext, tknEngine.pInstanceVertexInputLayout)
    gfx.destroyVertexInputLayoutPtr(pGfxContext, tknEngine.pMeshVertexInputLayout)
    tknEngine.instanceFormat = nil
    tknEngine.vertexFormat = nil
end

function tknEngine.updateGameplay()
    print("Lua updateGameplay")
end

function tknEngine.updateGfx(pGfxContext)
    print("Lua updateGfx")
end

-- print(weqe.qweqw.scfdad)
-- For subsequent updates and execution
_G.tknEngine = tknEngine
-- Return so the start function can be executed directly
return tknEngine
