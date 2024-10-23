local gameState = require("gameState")
local gameMath = require("gameMath")
local game = require("game")
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
    -- local buildingIndex = 1
    -- for x = 0, 50 do
    --     for y = 0, 50 do
    --         if gameMath.LCGRandom(gameMath.CantorPair(x, y) + 32321) % 100 < 3 then
    --             buildingIndex = (buildingIndex + 1) % 8;
    --             gameState.DrawModel(x, y, 0, models[buildingIndex + 1])
    --         end
    --     end
    -- end

    -- local length = 256;
    -- local width = 256;
    -- game.GenerateWorld(length, width)
    -- local vertices = {}
    -- local colors = {}
    -- local normals = {}
    -- local stepValue = 0.5
    -- local pixels = 4
    -- for x = 1, length do
    --     for y = 1, width do
    --         for i = 1, pixels do
    --             for j = 1, pixels do
    --                 table.insert(vertices, { x * pixels + i, y * pixels + j, 0 })
    --                 if game.temperatureMap[x][y] < -stepValue then
    --                     if game.humidityMap[x][y] < -stepValue then

    --                         table.insert(colors, { 156 / 255.0, 174 / 255.0, 185 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     elseif game.humidityMap[x][y] < stepValue then
    --                         table.insert(colors, { 240 / 255.0, 240 / 255.0, 240 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     else
    --                         table.insert(colors, { 170 / 255.0, 225 / 255.0, 225 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     end
    --                 elseif game.temperatureMap[x][y] < stepValue then
    --                     if game.humidityMap[x][y] < -stepValue then
    --                         table.insert(colors, { 237 / 255.0, 214 / 255.0, 160 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     elseif game.humidityMap[x][y] < stepValue then
    --                         table.insert(colors, { 220 / 255.0, 120 / 255.0, 135 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     else
    --                         table.insert(colors, { 100 / 255.0, 110 / 255.0, 255 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     end
    --                 else
    --                     if game.humidityMap[x][y] < -stepValue then
    --                         table.insert(colors, { 196 / 255.0, 156 / 255.0, 107 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     elseif game.humidityMap[x][y] < stepValue then
    --                         table.insert(colors, { 208 / 255.0, 137 / 255.0, 78 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     else
    --                         table.insert(colors, { 124 / 255.0, 156 / 255.0, 110 / 255.0, 255 / 255.0 })
    --                         table.insert(normals, { 0, 0, 1 })
    --                     end
    --                 end
    --             end
    --         end
    --     end
    -- end
    -- local index = gameState.AddModel(vertices, colors, normals);
    -- local modelScale = 0.0625
    -- local modelMatrix = {
    --     { modelScale, 0,          0,          0 },
    --     { 0,          modelScale, 0,          0 },
    --     { 0,          0,          modelScale, 0 },
    --     { 0,          0,          0,          1 },
    -- }
    -- gameState.UpdateModelUniformBuffer(index, modelMatrix)
    
    -- local length = 1024
    -- local width = 1024
    -- local vertices = {}
    -- local colors = {}
    -- local normals = {}
    -- for x = 1, length do
    --     for y = 1, width do
    --         for z = 1, 2 do
    --             table.insert(vertices, { x, y, z })
    --             table.insert(colors, { 105 / 255.0, 105 / 255.0, 105 / 255.0, 255 / 255.0 })
    --             table.insert(normals, { 0, 0, 0 })
    --         end
    --         for z = 3, 4 do
    --             local n = gameMath.PerlinNoise3D(12, x * 0.37, y * 0.37, z * 0.37)
    --             if n < -0.4 then
    --                 table.insert(vertices, { x, y, z })
    --                 table.insert(colors, { 101 / 255.0, 67 / 255.0, 33 / 255.0, 255 / 255.0 })
    --                 table.insert(normals, { 0, 0, 0 })
    --             elseif n < 0.5 then
    --                 table.insert(vertices, { x, y, z })
    --                 table.insert(colors, { 75 / 255.0, 58 / 255.0, 42 / 255.0, 255 / 255.0 })
    --                 table.insert(normals, { 0, 0, 0 })
    --             else

    --             end
    --         end
    --         for z = 5, 5 do
    --             local n = gameMath.PerlinNoise2D(13241, x * 0.37, y * 0.37)
    --             if n > 0.65 then
    --                 table.insert(vertices, { x, y, z })
    --                 table.insert(colors, { 169 / 255.0, 169 / 255.0, 169 / 255.0, 255 / 255.0 })
    --                 table.insert(normals, { 0, 0, 0 })
    --             elseif n < -0.25 then
    --                 local height = gameMath.LCGRandom(gameMath.CantorPair(x, y)) % 3
    --                 for h = 1, height do
    --                     table.insert(vertices, { x, y, z + h - 1 })
    --                     table.insert(colors, { 95 / 255.0, 165 / 255.0, 85 / 255.0, 255 / 255.0 })
    --                     table.insert(normals, { 0, 0, 0 })
    --                 end
    --             end
    --         end
    --     end
    -- end
    -- gameState.SetNormals(vertices, normals);
    -- local index = gameState.AddModel(vertices, colors, normals);
    -- local modelScale = 0.0625
    -- local modelMatrix = {
    --     { modelScale, 0,          0,          0 },
    --     { 0,          modelScale, 0,          0 },
    --     { 0,          0,          modelScale, -4 * modelScale },
    --     { 0,          0,          0,          1 },
    -- }
    -- gameState.UpdateModelUniformBuffer(index, modelMatrix)
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 15 }
local targetPosition = { 32, 32, 0 }
local t = 0;

function gameState.Update()
    print("Lua Update")
    t = t + 0.001
    local distance = gameMath.PingPong(2, 6, t)
    cameraPosition[1] = 32 - distance * math.sin(t)
    cameraPosition[2] = 32 + distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
