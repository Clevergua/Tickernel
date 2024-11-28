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
    sand = { 245, 212, 163, 255 },
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
local terrainToFoundation = {
    voxel.dirt,
    voxel.sand,
    voxel.sand,
    voxel.dirt,
    voxel.sand,
    voxel.andosols,
    voxel.andosols,
}

local length = 64
local width = 32

function engine.Start()
    print("Lua Start")


    -- local modelsPath = engine.assetsPath ..
    --     engine.pathSeparator .. "models" .. engine.pathSeparator
    -- print("Loading models")
    -- local models = {
    --     engine.LoadModel(modelsPath .. "LargeBuilding01_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "SmallBuilding01_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "SmallBuilding02_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "SmallBuilding03_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "SmallBuilding04_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "TallBuilding01_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "TallBuilding02_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "TallBuilding03_0.tknvox"),
    -- }
    -- for index, model in ipairs(models) do
    --     local count = 6
    --     local instances = {}
    --     for i = 1, count do
    --         local x = math.abs(gameMath.LCGRandom((index + 13251) * 525234532 + i * 42342345)) % length
    --         local y = math.abs(gameMath.LCGRandom((x - 2317831) * 431513511 + index * 24141312 - i * 2131204)) % width
    --         instances[i] = {
    --             { modelScale, 0,          0,          x },
    --             { 0,          modelScale, 0,          y },
    --             { 0,          0,          modelScale, 0 },
    --             { 0,          0,          0,          1 },
    --         }
    --     end
    --     engine.DrawModel(instances, model)
    -- end


    local seed = 232
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
            -- Target values
            local temperature = game.GetTemperature(x, y)
            local humidity = game.GetHumidity(x, y)
            for px = 1, voxelCount do
                for py = 1, voxelCount do
                    local dx = (px - (voxelCount + 1) / 2) / voxelCount
                    local dy = (py - (voxelCount + 1) / 2) / voxelCount
                    local deltaNoise = math.max(math.abs(dx), math.abs(dy)) * 2
                    deltaNoise = deltaNoise ^ 4
                    local voxelTemperature = game.GetTemperature((x + dx), (y + dy))
                    local voxelHumidity = game.GetHumidity((x + dx), (y + dy))
                    voxelTemperature = gameMath.Lerp(temperature, voxelTemperature,
                        deltaNoise)
                    voxelHumidity = gameMath.Lerp(humidity, voxelHumidity, deltaNoise)
                    local voxelTerrain = game.GetTerrain(voxelTemperature, voxelHumidity)

                    local roughnessNoiseScale = 0.17
                    local roughnessNoise = gameMath.PerlinNoise2D(3478, roughnessNoiseScale * ((x - 1) * voxelCount + px),
                        roughnessNoiseScale * ((y - 1) * voxelCount + py))
                    roughnessNoiseScale = roughnessNoiseScale * 2
                    roughnessNoise = roughnessNoise + 0.5 *
                        gameMath.PerlinNoise2D(3211, roughnessNoiseScale * ((x - 1) * voxelCount + px),
                            roughnessNoiseScale * ((y - 1) * voxelCount + py))
                    local deltaHeightStep = 0.20
                    roughnessNoise = roughnessNoise * terrainToRoughness[voxelTerrain]
                    local deltaHeight
                    if roughnessNoise > deltaHeightStep then
                        deltaHeight = 1
                    elseif roughnessNoise > -deltaHeightStep then
                        deltaHeight = 0
                    else
                        deltaHeight = -1
                    end
                    -- Foundation
                    local voxelHeightMap = voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py]
                    voxelHeightMap[1] = terrainToFoundation[game.GetTerrain(voxelTemperature, voxelHumidity)]

                    if voxelTerrain == terrain.ice or voxelTerrain == terrain.water or voxelTerrain == terrain.lava then
                        for z = 1, 2 + deltaHeight do
                            voxelHeightMap[z] = terrainToFoundation[voxelTerrain]
                        end
                    else
                        for z = 1, 5 + deltaHeight do
                            voxelHeightMap[z] = terrainToFoundation[voxelTerrain]
                        end
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
