local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    -- local modelsPath = gameState.assetsPath ..
    --     gameState.pathSeparator .. "models" .. gameState.pathSeparator;
    -- local models = {
    --     gameState.LoadModel(modelsPath .. "LargeBuilding01_0.tvoxel"),
    --     gameState.LoadModel(modelsPath .. "SmallBuilding01_0.tvoxel"),
    --     gameState.LoadModel(modelsPath .. "SmallBuilding02_0.tvoxel"),
    --     gameState.LoadModel(modelsPath .. "SmallBuilding03_0.tvoxel"),
    --     gameState.LoadModel(modelsPath .. "SmallBuilding04_0.tvoxel"),
    --     gameState.LoadModel(modelsPath .. "TallBuilding01_0.tvoxel"),
    --     gameState.LoadModel(modelsPath .. "TallBuilding02_0.tvoxel"),
    --     gameState.LoadModel(modelsPath .. "TallBuilding03_0.tvoxel"),
    -- }
    -- for x = 1, 20 do
    --     for y = 1, 20 do
    --         if gameMath.LCGRandom(gameMath.CantorPair(x, y) + 32321) % 100 < 30 then
    --             local buildingIndex = gameMath.LCGRandom(gameMath.CantorPair(x, y) + 13235) % #models + 1
    --             gameState.DrawModel(x - 10, y - 10, 0, models[buildingIndex])
    --         end
    --     end
    -- end

    -- local model2 = gameState.LoadModel(modelsPath .. "TallBuilding01.ply");
    -- gameState.DrawModel(-0, -0, 0, model2)
    -- local model3 = gameState.LoadModel(modelsPath .. "SmallBuilding02.ply");
    -- gameState.DrawModel(-0, -0, 0, model3)
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 7 }
local targetPosition = { 0, 0, 0 }
local t = 0;

function gameState.Update()
    print("Lua Update")
    t = t + 0.001
    local distance = gameMath.PingPong(10, 15, t)
    cameraPosition[1] = -distance * math.sin(t)
    cameraPosition[2] = distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
