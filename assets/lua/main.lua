local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    gameState.DrawModel(-30, -30, 0, "LargeBuilding01.ply")
    gameState.DrawModel(30, 30, 0, "LargeBuilding01.ply")
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 80 }
local targetPosition = { 0, 0, 0 }
local t = 0;

function gameState.Update()
    print("Lua Update")
    t = t + 0.001
    local distance = gameMath.PingPong(100, 150, t)
    cameraPosition[1] = -distance * math.sin(t)
    cameraPosition[2] = distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
