local srp = require("srp")
local tknEngine = {
    assetsPath = nil,
}

function tknEngine.start(pGfxContext, assetsPath)
    print("Lua start")
    tknEngine.assetsPath = assetsPath
    -- binding = 0

    srp.setup(pGfxContext, assetsPath)

    local mesh = {
        vertices = {
            { { 0, 0, 0, }, { 255, 0, 0, 255 }, { 0x0 } },
            { { 0, 1, 0, }, { 255, 0, 0, 255 }, { 0x0 } },
            { { 0, 2, 0, }, { 255, 0, 0, 255 }, { 0x0 } },
            { { 0, 3, 0, }, { 255, 0, 0, 255 }, { 0x0 } },
        },
        instances = {
            {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1,
            }
        }
    }
    gfx.createMeshPtr(pGfxContext, mesh)
end

function tknEngine.stop(pGfxContext)
    print("Lua stop")
    srp.tearDown(pGfxContext)
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
