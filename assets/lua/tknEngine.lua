local tknRenderPipeline = require("tknRenderPipeline")
local tknEngine = {}

function tknEngine.start(pGfxContext, assetsPath)
    print("Lua start")
    tknEngine.assetsPath = assetsPath
    -- binding = 0
    tknEngine.vertexFormat = {{
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
    tknEngine.instanceFormat = {{
        name = "model",
        type = TYPE_FLOAT,
        count = 16
    }}
    tknEngine.pMeshVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, tknEngine.vertexFormat)
    tknEngine.pInstanceVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, tknEngine.instanceFormat);
    tknRenderPipeline.setup(pGfxContext, assetsPath, tknEngine.pMeshVertexInputLayout, tknEngine.pInstanceVertexInputLayout)
    tknEngine.pGlobalMaterialPtr = gfx.getGlobalMaterialPtr(pGfxContext)

    tknEngine.globalUniformBufferFormat = {{
        name = "view",
        type = TYPE_FLOAT,
        count = 16
    }, {
        name = "projection",
        type = TYPE_FLOAT,
        count = 16
    }, {
        name = "viewProjection",
        type = TYPE_FLOAT,
        count = 16
    }, {
        name = "cameraPosition",
        type = TYPE_FLOAT,
        count = 4
    }, {
        name = "lightDirection",
        type = TYPE_FLOAT,
        count = 4
    }, {
        name = "lightColor",
        type = TYPE_FLOAT,
        count = 4
    }}
    local pGlobalUniformBuffer =
    {
        view = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        projection = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        viewProjection = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        cameraPosition = {0, 0, 5, 1},
        lightDirection = {0.57735, -0.57735, -0.57735, 0},
        lightColor = {1, 1, 1, 1}
    }
    gfx.createUniformBufferPtr(pGfxContext, tknEngine.globalUniformBufferFormat, pGlobalUniformBuffer)


    local vertices = {
        position = {0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0},
        color = {255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255},
        normal = {0x0, 0x0, 0x0, 0x0}
    }
    tknEngine.pMesh = gfx.createMeshPtr(pGfxContext, tknEngine.pMeshVertexInputLayout, tknEngine.vertexFormat, vertices, nil)
    local instances = {
        model = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}
    }
    tknEngine.pInstance = gfx.createInstancePtr(pGfxContext, tknEngine.pInstanceVertexInputLayout, tknEngine.instanceFormat, instances)

    local deferredRenderPass = tknRenderPipeline.deferredRenderPass
    tknEngine.pDrawCall = gfx.addDrawCallPtr(pGfxContext, deferredRenderPass.pGeometryPipeline, deferredRenderPass.pGeometryMaterial, tknEngine.pMesh, nil)
end

function tknEngine.stop(pGfxContext)
    print("Lua stop")
    gfx.removeDrawCallPtr(pGfxContext, tknEngine.pDrawCall)
    tknEngine.pDrawCall = nil
    gfx.destroyInstancePtr(pGfxContext, tknEngine.pInstance)
    tknEngine.pInstance = nil
    gfx.destroyMeshPtr(pGfxContext, tknEngine.testMesh)
    tknEngine.pMesh = nil

    tknEngine.pGlobalMaterialPtr = nil
    tknRenderPipeline.tearDown(pGfxContext)
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
