local tknRenderPipeline = require("tknRenderPipeline")
local format = require("format")
local tknEngine = {}

function tknEngine.start(pGfxContext, assetsPath)
    print("Lua start")
    tknEngine.assetsPath = assetsPath
    format.createLayouts(pGfxContext)
    tknRenderPipeline.setup(pGfxContext, assetsPath, format.pVoxelMeshVertexInputLayout, format.pInstanceVertexInputLayout)

    local pGlobalUniformBuffer = {
        view = {0.7071, -0.4082, 0.5774, 0, 0, 0.8165, 0.5774, 0, -0.7071, -0.4082, 0.5774, 0, 0, 0, -8.6603, 1},
        proj = {1.3584, 0, 0, 0, 0, 2.4142, 0, 0, 0, 0, -1.0020, -1, 0, 0, -0.2002, 0},
        inv_view_proj = {0.5206, 0, -0.5206, 0, -0.3007, 0.6013, -0.3007, 0, 0.0231, 0.0231, 0.0231, 0, 2.3077, 4.3301, 2.3077, 43.301},
        pointSizeFactor = 1000.0,
        time = 0.0,
        frameCount = 0,
        near = 0.1,
        far = 100.0,
        fov = 90.0,
    }
    tknEngine.pGlobalUniformBuffer = gfx.createUniformBufferPtr(pGfxContext, format.globalUniformBufferFormat, pGlobalUniformBuffer)
    local inputBindings = {{
        vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        pUniformBuffer = tknEngine.pGlobalUniformBuffer,
        binding = 0,
    }}
    tknEngine.pGlobalMaterialPtr = gfx.getGlobalMaterialPtr(pGfxContext)
    gfx.updateMaterialPtr(pGfxContext, tknEngine.pGlobalMaterialPtr, inputBindings)

    local pLightsUniformBuffer = {
        directionalLight_color = {1.0, 1.0, 0.9, 1.0},
        directionalLight_direction = {0.5, -1.0, 0.3, 0.0},
        pointLights = {},
        pointLightCount = 0,
    }
    for i = 1, 128 * 8 do
        table.insert(pLightsUniformBuffer.pointLights, 0.0)
    end
    tknEngine.pLightsUniformBuffer = gfx.createUniformBufferPtr(pGfxContext, format.lightsUniformBufferFormat, pLightsUniformBuffer)

    local deferredRenderPass = tknRenderPipeline.deferredRenderPass
    tknEngine.pLightingMaterialPtr = gfx.getSubpassMaterialPtr(pGfxContext, deferredRenderPass.pRenderPass, 1)
    local lightingInputBindings = {{
        vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        pUniformBuffer = tknEngine.pLightsUniformBuffer,
        binding = 3,
    }}
    gfx.updateMaterialPtr(pGfxContext, tknEngine.pLightingMaterialPtr, lightingInputBindings)

    local vertices = {
        position = {-1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0},
        color = {0xFF0000FF, 0x00FF00FF, 0x0000FFFF},
        normal = {0x1, 0x0, 0x0},
    }

    tknEngine.pMesh = gfx.createMeshPtrWithData(pGfxContext, format.pVoxelMeshVertexInputLayout, format.voxelVertexFormat, vertices, 0, nil)
    local instances = {
        model = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    }
    tknEngine.pInstance = gfx.createInstancePtr(pGfxContext, format.pInstanceVertexInputLayout, format.instanceFormat, instances)

    local deferredRenderPass = tknRenderPipeline.deferredRenderPass
    tknEngine.pDrawCall = gfx.addDrawCallPtr(pGfxContext, deferredRenderPass.pGeometryPipeline, deferredRenderPass.pGeometryMaterial, tknEngine.pMesh, tknEngine.pInstance)
end

function tknEngine.stop()
    print("Lua stop")
end

function tknEngine.stopGfx(pGfxContext)
    print("Lua stopGfx")

    gfx.removeDrawCallPtr(pGfxContext, tknEngine.pDrawCall)
    tknEngine.pDrawCall = nil

    gfx.destroyInstancePtr(pGfxContext, tknEngine.pInstance)
    tknEngine.pInstance = nil
    gfx.destroyMeshPtr(pGfxContext, tknEngine.pMesh)
    tknEngine.pMesh = nil

    tknEngine.pLightingMaterialPtr = nil

    tknRenderPipeline.teardown(pGfxContext)

    gfx.destroyUniformBufferPtr(pGfxContext, tknEngine.pGlobalUniformBuffer)
    tknEngine.pGlobalUniformBuffer = nil
    gfx.destroyUniformBufferPtr(pGfxContext, tknEngine.pLightsUniformBuffer)
    tknEngine.pLightsUniformBuffer = nil

    format.destroyLayouts(pGfxContext)
end

function tknEngine.updateGameplay()
    print("Lua updateGameplay")
end

function tknEngine.updateGfx(pGfxContext)
    print("Lua updateGfx")
end

_G.tknEngine = tknEngine
return tknEngine
