local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    local model = gameState.LoadModel(gameState.assetsPath ..
        gameState.pathSeparator .. "models" .. gameState.pathSeparator .. "LargeBuilding01.ply");
    gameState.DrawModel(-3, -3, 0, model)
    gameState.DrawModel(3, 3, 0, model)
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 10 }
local targetPosition = { 0, 0, 0 }
local t = 0;

function gameState.Update()
    print("Lua Update")
    t = t + 0.001
    local distance = gameMath.PingPong(5, 10, t)
    cameraPosition[1] = -distance * math.sin(t)
    cameraPosition[2] = distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
