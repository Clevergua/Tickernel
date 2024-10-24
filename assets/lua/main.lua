local gameState = require("gameState")
local gameMath = require("gameMath")
local game = require("game")


local blockToColor = {
    [1] = { 210, 150, 95, 255 },
    [2] = { 175, 130, 70, 255 },
    [3] = { 60, 140, 85, 255 },
    [4] = { 86, 98, 185, 255 },
}
local block = {
    none = 0,
    dirt = 1,
    dirt2 = 2,
    grass = 3,
    water = 4
}

local seed = {
    heightSeed = 1,
    dirtColorSeed = 2,
    grassSeed = 1,
}
function gameState.Start()
    print("Lua Start")

    local pixel = 16

    -- local modelsPath = gameState.assetsPath ..
    --     gameState.pathSeparator .. "models" .. gameState.pathSeparator
    -- print("Loading models")
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
    --             buildingIndex = (buildingIndex + 1) % 8
    --             gameState.DrawModel(x, y, 0, 1 / pixel, models[buildingIndex + 1])
    --         end
    --     end
    -- end


    local length = 64
    local width = 64
    local humidityStepValue = 0.2
    local temperatureStepValue = 0.2
    print("Generating world..")
    local temperatureMap = {}
    local humidityMap = {}
    for x = 1, length do
        temperatureMap[x] = {}
        humidityMap[x] = {}
        for y = 1, width do
            local ns = 0.237
            temperatureMap[x][y] = gameMath.PerlinNoise2D(43214, x * ns, y * ns) +
                gameMath.PerlinNoise2D(3123, x * ns * 0.5, y * ns * 0.5)
            ns = 0.47
            humidityMap[x][y] = gameMath.PerlinNoise2D(3321, x * ns, y * ns) +
                gameMath.PerlinNoise2D(689, x * ns * 0.5, y * ns * 0.5)
        end
    end

    local vertices = {}
    local colors = {}
    local normals = {}

    local pixelMap = {}
    local height = 8
    for x = 1, length * pixel do
        pixelMap[x] = {}
        for y = 1, width * pixel do
            pixelMap[x][y] = {}
            for z = 1, height do
                pixelMap[x][y][z] = block.none
            end
        end
    end
    print("Generating dirt..")
    for x = 1, length * pixel do
        for y = 1, width * pixel do
            for z = 1, 4 do
                local dirtColorNoiseScale = 0.07
                local dirtColorNoise = gameMath.PerlinNoise3D(seed.dirtColorSeed, x * dirtColorNoiseScale,
                    y * dirtColorNoiseScale, z * dirtColorNoiseScale)
                if dirtColorNoise < -0.2 then
                    pixelMap[x][y][z] = block.dirt
                else
                    pixelMap[x][y][z] = block.dirt2
                end
            end
        end
    end

    print("Generating surface..")
    for x = 1, length * pixel do
        for y = 1, width * pixel do
            local cx = (x + pixel - 1) // pixel
            local cy = (y + pixel - 1) // pixel
            local ct = temperatureMap[cx][cy]
            local ch = humidityMap[cx][cy]
            local t = temperatureMap[x]
            local h = humidityMap[y]
            -- Target values
            local tt, th
            if ct < -temperatureStepValue then
                tt = -1
            elseif ct < temperatureStepValue then
                tt = 0
            else
                tt = 1
            end

            if ch < -humidityStepValue then
                th = -1
            elseif ch < humidityStepValue then
                th = 0
            else
                th = 1
            end
        end
    end

    for x = 1, length * pixel do
        for y = 1, width * pixel do
            for z = 1, height do
                if pixelMap[x][y][z] ~= block.none then
                    table.insert(vertices, { x, y, z })
                    table.insert(colors, blockToColor[pixelMap[x][y][z]])
                    table.insert(normals, { 0, 0, 0 })
                end
            end
        end
    end

    gameState.SetNormals(vertices, normals)
    local index = gameState.AddModel(vertices, colors, normals)
    local modelScale = 1 / pixel
    local modelMatrix = {
        { modelScale, 0,          0,          0 },
        { 0,          modelScale, 0,          0 },
        { 0,          0,          modelScale, -4 * modelScale },
        { 0,          0,          0,          1 },
    }
    gameState.UpdateModelUniformBuffer(index, modelMatrix)
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 18, 32, 8 }
local targetPosition = { 20, 32, 0 }
local t = 0

function gameState.Update()
    print("Lua Update")
    t = t + 0.005
    -- local distance = gameMath.PingPong(-3, 3, t)
    -- targetPosition[3] = distance
    -- cameraPosition[1] = 32 - distance * math.sin(t)
    -- cameraPosition[2] = 32 + distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
