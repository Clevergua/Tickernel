local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    local modelsPath = gameState.assetsPath ..
        gameState.pathSeparator .. "models" .. gameState.pathSeparator;
    local model = gameState.LoadModel(modelsPath .. "LargeBuilding01.ply");
    gameState.DrawModel(-1, -1, 0, model)
    gameState.DrawModel(1, 1, 0, model)
    -- local model2 = gameState.LoadModel(modelsPath .. "TallBuilding01.ply");
    -- gameState.DrawModel(-0, -0, 0, model2)
    -- local model3 = gameState.LoadModel(modelsPath .. "SmallBuilding02.ply");
    -- gameState.DrawModel(-0, -0, 0, model3)
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 3 }
local targetPosition = { 0, 0, 0 }
local t = 0;

function gameState.Update()
    print("Lua Update")
    t = t + 0.001
    local distance = gameMath.PingPong(3, 4, t)
    cameraPosition[1] = -distance * math.sin(t)
    cameraPosition[2] = distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
