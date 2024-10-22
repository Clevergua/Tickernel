local gameState = require("gameState")
local gameMath = require("gameMath")
local game = require("game")
function gameState.Start()
    print("Lua Start")
    local modelsPath = gameState.assetsPath ..
        gameState.pathSeparator .. "models" .. gameState.pathSeparator;
    local models = {
        gameState.LoadModel(modelsPath .. "LargeBuilding01_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "SmallBuilding01_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "SmallBuilding02_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "SmallBuilding03_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "SmallBuilding04_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "TallBuilding01_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "TallBuilding02_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "TallBuilding03_0.tvoxel"),
    }
    for x = 10, 50 do
        for y = 10, 50 do
            if gameMath.LCGRandom(gameMath.CantorPair(x, y) + 32321) % 100 < 5 then
                local buildingIndex = gameMath.LCGRandom(gameMath.CantorPair(x, y) + 13235) % #models + 1
                gameState.DrawModel(x, y, 0, models[buildingIndex])
            end
        end
    end

    local length = 256;
    local width = 256;
    game.GenerateWorld(length, width)
    local vertices = {}
    local colors = {}
    local normals = {}
    for x = 1, length do
        for y = 1, width do
            if game.temperatureMap[x][y] < -0.2 then
                if game.humidityMap[x][y] < -0.2 then
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 156 / 255.0, 174 / 255.0, 185 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                elseif game.humidityMap[x][y] < 0.2 then
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 240 / 255.0, 240 / 255.0, 240 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                else
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 170 / 255.0, 225 / 255.0, 225 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                end
            elseif game.temperatureMap[x][y] < 0.2 then
                if game.humidityMap[x][y] < -0.2 then
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 237 / 255.0, 214 / 255.0, 160 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                elseif game.humidityMap[x][y] < 0.2 then
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 180 / 255.0, 186 / 255.0, 135 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                else
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 100 / 255.0, 110 / 255.0, 255 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                end
            else
                if game.humidityMap[x][y] < -0.2 then
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 196 / 255.0, 156 / 255.0, 107 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                elseif game.humidityMap[x][y] < 0.2 then
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 208 / 255.0, 137 / 255.0, 78 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                else
                    table.insert(vertices, { x, y, 0 })
                    table.insert(colors, { 124 / 255.0, 156 / 255.0, 110 / 255.0, 255 / 255.0 })
                    table.insert(normals, { 0, 0, 1 })
                end
            end
        end
    end
    local index = gameState.AddModel(vertices, colors, normals);
    local modelScale = 0.0625
    local modelMatrix = {
        { modelScale, 0,          0,          0 },
        { 0,          modelScale, 0,          0 },
        { 0,          0,          modelScale, 0 },
        { 0,          0,          0,          1 },
    }
    gameState.UpdateModelUniformBuffer(index, modelMatrix)
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 8 }
local targetPosition = { 32, 32, 0 }
local t = 0;

function gameState.Update()
    print("Lua Update")
    t = t + 0.001
    local distance = gameMath.PingPong(5, 9, t)
    cameraPosition[1] = 32 - distance * math.sin(t)
    cameraPosition[2] = 32 + distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
