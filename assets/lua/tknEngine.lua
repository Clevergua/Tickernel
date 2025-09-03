local tknRenderPipeline = require("tknRenderPipeline")
local tknEngine = {}

function tknEngine.start(pGfxContext, assetsPath)
    print("Lua start")
    tknEngine.assetsPath = assetsPath
    -- binding = 0
    tknEngine.vertexLayout = {{
        name = "position",
        type = TYPE_FLOAT,
        count = 3
    }, {
        name = "color",
        type = TYPE_UINT8,
        count = 4
    }, {
        name = "normal",
        type = TYPE_UINT32,
        count = 1
    }}
    tknEngine.instanceLayout = {{
        name = "model",
        type = TYPE_FLOAT,
        count = 16
    }}
    tknEngine.pMeshVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, tknEngine.vertexLayout)
    tknEngine.pInstanceVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, tknEngine.instanceLayout);

    tknRenderPipeline.setup(pGfxContext, assetsPath, tknEngine.pMeshVertexInputLayout, tknEngine.pInstanceVertexInputLayout)
    tknEngine.pGlobalMaterialPtr = gfx.getGlobalMaterialPtr(pGfxContext)

    local vertices = {{{0, 0, 0}, {255, 0, 0, 255}, {0x0}}, {{0, 1, 0}, {255, 0, 0, 255}, {0x0}}, {{0, 2, 0}, {255, 0, 0, 255}, {0x0}}, {{0, 3, 0}, {255, 0, 0, 255}, {0x0}}}
    tknEngine.pTestMesh = gfx.createMeshPtr(pGfxContext, tknEngine.pMeshVertexInputLayout, tknEngine.vertexLayout, vertices, nil)
    local deferredRenderPass = tknRenderPipeline.deferredRenderPass
    tknEngine.pDrawCall = gfx.addDrawCallPtr(pGfxContext, deferredRenderPass.pGeometryPipeline, deferredRenderPass.pGeometryMaterial, tknEngine.pTestMesh, nil)
end

function tknEngine.stop(pGfxContext)
    print("Lua stop")
    gfx.removeDrawCallPtr(pGfxContext, tknEngine.pDrawCall)
    tknEngine.pDrawCall = nil
    gfx.destroyMeshPtr(pGfxContext, tknEngine.testMesh)
    tknEngine.pTestMesh = nil

    tknEngine.pGlobalMaterialPtr = nil
    tknRenderPipeline.tearDown(pGfxContext)

    gfx.destroyVertexInputLayoutPtr(pGfxContext, tknEngine.pInstanceVertexInputLayout)
    gfx.destroyVertexInputLayoutPtr(pGfxContext, tknEngine.pMeshVertexInputLayout)
    tknEngine.instanceLayout = nil
    tknEngine.vertexLayout = nil
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
