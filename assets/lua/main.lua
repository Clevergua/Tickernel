local engine = require("engine")
local gameMath = require("gameMath")
local game = require("game")

local voxelCount = 16
local modelScale = 1 / voxelCount;
local voxel = {
    none = { 0, 0, 0, 0 },
    dirt = { 210, 150, 95, 255 },
    snow = { 225, 225, 225, 255 },
    ice = { 190, 190, 245, 255 },
    sand = { 225, 200, 150, 255 },
    grass = { 170, 225, 115, 255 },
    water = { 86, 98, 185, 255 },
    lava = { 225, 43, 67, 255 },
    andosols = { 51, 41, 41, 255 },
}
local terrainToRoughness = {
    0.2,
    0.2,
    0.2,
    0.5,
    0.5,
    0.8,
    0.8,
}

local length = 32
local width = 32

function engine.Start()
    print("Lua Start")


    local modelsPath = engine.assetsPath ..
        engine.pathSeparator .. "models" .. engine.pathSeparator
    print("Loading models")
    local models = {
        engine.LoadModel(modelsPath .. "LargeBuilding01_0.tknvox"),
        engine.LoadModel(modelsPath .. "SmallBuilding01_0.tknvox"),
        engine.LoadModel(modelsPath .. "SmallBuilding02_0.tknvox"),
        engine.LoadModel(modelsPath .. "SmallBuilding03_0.tknvox"),
        engine.LoadModel(modelsPath .. "SmallBuilding04_0.tknvox"),
        engine.LoadModel(modelsPath .. "TallBuilding01_0.tknvox"),
        engine.LoadModel(modelsPath .. "TallBuilding02_0.tknvox"),
        engine.LoadModel(modelsPath .. "TallBuilding03_0.tknvox"),
    }
    for index, model in ipairs(models) do
        local count = 6
        local instances = {}
        for i = 1, count do
            local x = math.abs(gameMath.LCGRandom((index + 13251) * 525234532 + i * 42342345)) % length
            local y = math.abs(gameMath.LCGRandom((x - 2317831) * 431513511 + index * 24141312 - i * 2131204)) % width
            instances[i] = {
                { modelScale, 0,          0,          x },
                { 0,          modelScale, 0,          y },
                { 0,          0,          modelScale, 0 },
                { 0,          0,          0,          1 },
            }
        end
        engine.DrawModel(instances, model)
    end


    local seed = 0
    game.GenerateWorld(seed, length, width)
    print("Generating world..")

    local voxelMap = {}
    local height = 8
    for x = 1, length * voxelCount do
        voxelMap[x] = {}
        for y = 1, width * voxelCount do
            voxelMap[x][y] = {}
            for z = 1, height do
                voxelMap[x][y][z] = voxel.none
            end
        end
    end

    local pointLights = {}
    print("Generating surface..")
    local terrain = game.terrain
    for x = 1, length do
        for y = 1, width do
            local currentTerrain = game.terrainMap[x][y]
            -- Target values
            local targetTemperature, targetHumidity
            if currentTerrain == terrain.snow then
                targetTemperature = -1
                targetHumidity = 0
            elseif currentTerrain == terrain.ice then
                targetTemperature = -1
                targetHumidity = 1
            elseif currentTerrain == terrain.sand then
                targetTemperature = 0
                targetHumidity = -1
            elseif currentTerrain == terrain.grass then
                targetTemperature = 0
                targetHumidity = 0
            elseif currentTerrain == terrain.water then
                targetTemperature = 0
                targetHumidity = 1
            elseif currentTerrain == terrain.lava then
                targetTemperature = 1
                targetHumidity = -1
            elseif currentTerrain == terrain.andosols then
                targetTemperature = 0
                targetHumidity = 0
            else
                error("Unknown terrain")
            end

            for px = 1, voxelCount do
                for py = 1, voxelCount do
                    local dx = (px - voxelCount / 2 - 0.5) / voxelCount
                    local dy = (py - voxelCount / 2 - 0.5) / voxelCount
                    local deltaNoise = math.max(math.abs(dx), math.abs(dy)) * 2
                    local temperature = gameMath.Lerp(targetTemperature, game.GetTemperature((x + dx), (y + dy)),
                        deltaNoise)
                    local humidity = gameMath.Lerp(targetHumidity, game.GetHumidity((x + dx), (y + dy)), deltaNoise)
                    local voxelTerrain = game.GetTerrain(temperature, humidity)

                    local roughnessNoiseScale = 0.27
                    local roughnessNoise = gameMath.PerlinNoise2D(3478, roughnessNoiseScale * ((x - 1) * voxelCount + px),
                        roughnessNoiseScale * ((y - 1) * voxelCount + py))
                    local deltaHeight
                    local deltaHeightStep = 0.15
                    roughnessNoise = roughnessNoise * terrainToRoughness[voxelTerrain]
                    if roughnessNoise > deltaHeightStep then
                        deltaHeight = 1
                    elseif roughnessNoise > -deltaHeightStep then
                        deltaHeight = 0
                    else
                        deltaHeight = -1
                    end
                    for z = 1, 4 + deltaHeight do
                        voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py][z] = voxel.dirt
                    end
                end
            end
        end
    end

    local vertices = {}
    local colors = {}
    local normals = {}
    for x = 1, length * voxelCount do
        for y = 1, width * voxelCount do
            for z = 1, height do
                if voxelMap[x][y][z] ~= voxel.none then
                    table.insert(vertices, { x, y, z - 4 })
                    table.insert(colors, voxelMap[x][y][z])
                    table.insert(normals, { 0, 0, 0 })
                end
            end
        end
    end
    print("Drawing models..")
    engine.SetNormals(vertices, normals)
    local index = engine.AddModel(vertices, colors, normals)
    local modelMatrix = {
        {
            { modelScale, 0,          0,          0 },
            { 0,          modelScale, 0,          0 },
            { 0,          0,          modelScale, -4 * modelScale },
            { 0,          0,          0,          1 },
        },
    }

    engine.UpdateInstances(index, modelMatrix)

    -- print("Generating vertices..")
    -- local voxelToVertices = {}
    -- local voxelToColors = {}
    -- local voxelToNormals = {}
    -- for x = 1, length * voxelCount do
    --     for y = 1, width * voxelCount do
    --         for z = 1, height do
    --             if voxelMap[x][y][z] ~= voxel.none then
    --                 if voxelToVertices[voxelMap[x][y][z]] == nil then
    --                     voxelToVertices[voxelMap[x][y][z]] = {}
    --                     voxelToColors[voxelMap[x][y][z]] = {}
    --                     voxelToNormals[voxelMap[x][y][z]] = {}
    --                 end
    --                 table.insert(voxelToVertices[voxelMap[x][y][z]], { x, y, z - 4 })
    --                 table.insert(voxelToColors[voxelMap[x][y][z]], voxelMap[x][y][z])
    --                 table.insert(voxelToNormals[voxelMap[x][y][z]], { 0, 0, 0 })
    --             end
    --         end
    --     end
    -- end
    -- print("Drawing models..")
    -- for i = 1, 10 do
    --     if voxelToVertices[i] ~= nil then
    --         engine.SetNormals(voxelToVertices[i], voxelToNormals[i])
    --         local index = engine.AddModel(voxelToVertices[i], voxelToColors[i], voxelToNormals[i])
    --         local modelMatrix = {
    --             {
    --                 { modelScale, 0,          0,          0 },
    --                 { 0,          modelScale, 0,          0 },
    --                 { 0,          0,          modelScale, -4 * modelScale },
    --                 { 0,          0,          0,          1 },
    --             },
    --         }
    --         engine.UpdateInstances(index, modelMatrix)
    --     end
    -- end
    local directionalLight = {
        color = { 1, 1, 1, 1 },
        direction = { 1, -1, -1 },
    }
    engine.UpdateLightsUniformBuffer(directionalLight, pointLights)
end

function engine.End()
    print("Lua Start")
end

local cameraPosition = { 0, 12, 18 }
local targetPosition = { 0, 16, 0 }
local t = 0

function engine.Update()
    print("Lua Update")
    if engine.frameCount == 0 then
        collectgarbage("collect")
    end
    t = t + 0.001
    local distance = gameMath.PingPong(0, length, t)
    cameraPosition[1] = distance
    targetPosition[1] = distance
    -- collectgarbage("collect")
    local memoryUsage = collectgarbage("count")
    print("Current memory usage: ", memoryUsage, "KB")
    engine.UpdateGlobalUniformBuffer(cameraPosition, targetPosition)
    engine.frameCount = engine.frameCount + 1
end

_G.engine = engine

print("Lua initialized!")
return engine
