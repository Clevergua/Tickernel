local gfx = require("gfx")
local tknRenderPipeline = require("tknRenderPipeline")
local ui = require("ui")
local format = require("format")
local input = require("input")
local tknEngine = {}

function tknEngine.start(pGfxContext, assetsPath)
    print("Lua start")
    tknEngine.assetsPath = assetsPath
    format.createLayouts(pGfxContext)
    local renderPassIndex = 0
    tknRenderPipeline.setup(pGfxContext, assetsPath, format.voxelVertexFormat.pVertexInputLayout, format.instanceFormat.pVertexInputLayout, renderPassIndex)

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
    tknEngine.pGlobalMaterial = gfx.getGlobalMaterialPtr(pGfxContext)
    gfx.updateMaterialPtr(pGfxContext, tknEngine.pGlobalMaterial, inputBindings)

    local pLightsUniformBuffer = {
        directionalLightColor = {1.0, 1.0, 0.9, 1.0},
        directionalLightDirection = {0.5, -1.0, 0.3, 0.0},
        pointLights = {},
        pointLightCount = 0,
    }
    for i = 1, 128 * 8 do
        table.insert(pLightsUniformBuffer.pointLights, 0.0)
    end
    tknEngine.pLightsUniformBuffer = gfx.createUniformBufferPtr(pGfxContext, format.lightsUniformBufferFormat, pLightsUniformBuffer)

    local deferredRenderPass = tknRenderPipeline.deferredRenderPass
    tknEngine.pLightingMaterial = gfx.getSubpassMaterialPtr(pGfxContext, deferredRenderPass.pRenderPass, 1)
    local lightingInputBindings = {{
        vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        pUniformBuffer = tknEngine.pLightsUniformBuffer,
        binding = 3,
    }}
    gfx.updateMaterialPtr(pGfxContext, tknEngine.pLightingMaterial, lightingInputBindings)

    local vertices = {
        position = {-1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0},
        color = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000},
        normal = {0x1, 0x0, 0x0},
    }

    tknEngine.pMesh = gfx.createMeshPtrWithData(pGfxContext, format.voxelVertexFormat.pVertexInputLayout, format.voxelVertexFormat, vertices, 0, nil)
    local instances = {
        model = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    }
    tknEngine.pInstance = gfx.createInstancePtr(pGfxContext, format.instanceFormat.pVertexInputLayout, format.instanceFormat, instances)

    local deferredRenderPass = tknRenderPipeline.deferredRenderPass
    tknEngine.pGeometryDrawCall = gfx.createDrawCallPtr(pGfxContext, deferredRenderPass.pGeometryPipeline, deferredRenderPass.pGeometryMaterial, tknEngine.pMesh, tknEngine.pInstance)
    gfx.insertDrawCallPtr(tknEngine.pGeometryDrawCall, 0)

    renderPassIndex = renderPassIndex + 1
    ui.setup(pGfxContext, tknRenderPipeline.pSwapchainAttachment, assetsPath, renderPassIndex)

    tknEngine.pDefaultImage = gfx.createImagePtrWithPath(pGfxContext, assetsPath .. "/textures/default.astc")
    tknEngine.pDefaultUIMaterial = ui.createMaterialPtr(pGfxContext, tknEngine.pDefaultImage)
    tknEngine.testNode = ui.addNode(pGfxContext, ui.rootNode, #ui.rootNode.children + 1, "testNode", {
        dirty = true,
        horizontal = {
            type = "relative",
            left = 100,
            right = 100,
        },
        vertical = {
            type = "relative",
            bottom = 100,
            top = 100,
        },
        rect = {},
    })
    ui.addImageComponent(pGfxContext, 0xFFFFFFFF, nil, tknEngine.pDefaultUIMaterial, tknEngine.testNode)
end

function tknEngine.stop()
    print("Lua stop")
end

function tknEngine.stopGfx(pGfxContext)
    print("Lua stopGfx")
    ui.teardown(pGfxContext)
    tknEngine.pDefaultUIMaterial = nil
    -- gfx.destroyPipelineMaterialPtr(pGfxContext, tknEngine.pDefaultUIMaterial)
    gfx.destroyImagePtr(pGfxContext, tknEngine.pDefaultImage)
    print("Destroying draw call and instance")
    gfx.destroyDrawCallPtr(pGfxContext, tknEngine.pGeometryDrawCall)
    tknEngine.pGeometryDrawCall = nil
    gfx.destroyInstancePtr(pGfxContext, tknEngine.pInstance)
    tknEngine.pInstance = nil
    gfx.destroyMeshPtr(pGfxContext, tknEngine.pMesh)
    tknEngine.pMesh = nil
    tknEngine.pLightingMaterial = nil
    print("Tearing down render pipeline")
    gfx.destroyUniformBufferPtr(pGfxContext, tknEngine.pGlobalUniformBuffer)
    tknEngine.pGlobalUniformBuffer = nil
    gfx.destroyUniformBufferPtr(pGfxContext, tknEngine.pLightsUniformBuffer)
    tknEngine.pLightsUniformBuffer = nil
    tknRenderPipeline.teardown(pGfxContext)

    format.destroyLayouts(pGfxContext)
end

function tknEngine.updateGameplay()
    print("Lua updateGameplay")
end

function tknEngine.updateUI(pGfxContext)
    local aKeyState = input.getKeyState(input.keyCode.a)
    if aKeyState == input.keyState.down then
        print("A key was just pressed this frame")
    elseif aKeyState == input.keyState.up then
        print("A key was just released this frame")
        -- -- function ui.addNode(pGfxContext, parent, index, name, layout)
        -- local node = ui.addNode(pGfxContext, ui.rootNode, #ui.rootNode.children + 1, "NewNode", {
        --     type = "absolute",
        --     left = 100,
        --     top = 100,
        --     width = 100,
        --     height = 100,
        -- })
        -- -- function ui.addImageComponent(pGfxContext, color, slice, pMaterial, node)
        -- ui.addImageComponent(pGfxContext, 0xFFFFFFFF, nil, pMaterial, node)
    elseif aKeyState == input.keyState.idle then
        -- Key is idle, no action needed
    end
end

function tknEngine.updateGfx(pGfxContext, width, height)
    tknEngine.updateUI(pGfxContext)
    ui.updateLayout(pGfxContext, width, height)
    print("Lua updateGfx")
end

_G.tknEngine = tknEngine
return tknEngine
