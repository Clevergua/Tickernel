local engine = require("engine")
local graphic = require("graphic")
_G.engine = engine
function engine.start()
    print("Lua Start")


    graphic.createRenderPipelines();
end

function engine.stop()
    print("Lua stop")
    graphic.destroyDeferredRenderPipeline();
end

function engine.update()
    
end

print("Lua initialized!")
return engine
