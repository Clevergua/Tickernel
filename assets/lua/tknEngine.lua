local tknEngine = {

}

function tknEngine.start()
    print("Lua start")
end

function tknEngine.stop()
    print("Lua stop")
end

function tknEngine.updateGameplay()
    print("Lua updateGameplay")
end

function tknEngine.updateGfx(gfxContext)
    print("Lua updateGfx")
end

print(weqe.qweqw.scfdad)
-- For subsequent updates and execution
_G.tknEngine = tknEngine
-- Return so the start function can be executed directly
return tknEngine