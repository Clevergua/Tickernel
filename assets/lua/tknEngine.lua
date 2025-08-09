local srp = require("srp")
local tknEngine = {
    assetsPath = nil,
}

function tknEngine.start(pGfxContext, assetsPath)
    print("Lua start")
    tknEngine.assetsPath = assetsPath
    srp.setUp(pGfxContext, assetsPath)
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
