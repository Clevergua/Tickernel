local srp = require("srp")
local tknEngine = {

}

function tknEngine.start(pGfxContext)
    print("Lua start")
    srp.setUp(pGfxContext)
end

function tknEngine.stop(pGfxContext)
    print("Lua stop")
    srp.tearDown(pGfxContext)
end

function tknEngine.updateGameplay()
    print("Lua updateGameplay")
end

function tknEngine.updateGfx(gfxContext)
    print("Lua updateGfx")
end

-- print(weqe.qweqw.scfdad)
-- For subsequent updates and execution
_G.tknEngine = tknEngine
-- Return so the start function can be executed directly
return tknEngine
