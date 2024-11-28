local engine = require("engine")
local gameMath = require("gameMath")
local game = require("game")

local voxelCount = 16
local modelScale = 1 / voxelCount;
local voxel = {
    none = { 0, 0, 0, 0 },
    dirt = { 210, 150, 95, 255 },
    snow = { 250, 250, 250, 255 },
    ice = { 190, 190, 245, 255 },
    sand = { 245, 212, 163, 255 },
    grass = { 170, 225, 115, 255 },
    water = { 86, 98, 185, 255 },
    lava = { 225, 100, 67, 255 },
    volcanicRock = { 51, 41, 41, 255 },
    volcanicAsh = { 128, 128, 128, 255 },
}

local terrainToView = {
    {
        roughness = 0.3,
        foundation = voxel.dirt,
        roughnessNoiseScale = 0.057,
        surface = voxel.snow,
        surfaceNoiseScale = 0.07,
        surfaceStep = -0.7,
    },
    {
        roughness = 0.3,
        foundation = voxel.sand,
        surface = voxel.ice,
        roughnessNoiseScale = 0.057,
        surfaceNoiseScale = 0.07,
        surfaceStep = -0.3,
    },
    {
        roughness = 0.3,
        foundation = voxel.sand,
        surface = voxel.sand,
        roughnessNoiseScale = 0.057,
        surfaceNoiseScale = 0.09,
        surfaceStep = 0,
    },
    {
        roughness = 0.6,
        foundation = voxel.dirt,
        surface = voxel.grass,
        roughnessNoiseScale = 0.37,
        surfaceNoiseScale = 0.19,
        surfaceStep = -0.1,
    },
    {
        roughness = 0.6,
        foundation = voxel.sand,
        surface = voxel.water,
        roughnessNoiseScale = 0.03,
        surfaceNoiseScale = 0,
    },
    {
        roughness = 0.8,
        foundation = voxel.volcanicRock,
        surface = voxel.lava,
        roughnessNoiseScale = 0.17,
        surfaceNoiseScale = 0.09,
        surfaceStep = 0,
    },
    {
        roughness = 0.8,
        foundation = voxel.volcanicRock,
        surface = voxel.volcanicAsh,
        roughnessNoiseScale = 0.17,
        surfaceNoiseScale = 0.17,
        surfaceStep = 0.5,
    },
}

local length = 128
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


    local seed = 6345
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
            local random = gameMath.LCGRandom(gameMath.CantorPair(x, y)) % 100
            if random < 5 and #pointLights < 256 then
                -- if random < 3 then
                table.insert(pointLights, {
                    color = { 0.8, 0.4, 0, 0.8 },
                    position = { x, y, 0.5 },
                    range = 4,
                })
                -- else
                --     table.insert(pointLights, {
                --         color = { 0.9, 0.2, 0.1, 0.8 },
                --         position = { x, y, 0.5 },
                --         range = 3,
                --     })
                -- end
            end
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
                    local terrainView = terrainToView[voxelTerrain]
                    local roughnessNoiseScale = terrainView.roughnessNoiseScale
                    local roughnessNoise = gameMath.PerlinNoise2D(3478, roughnessNoiseScale * ((x - 1) * voxelCount + px),
                        roughnessNoiseScale * ((y - 1) * voxelCount + py))
                    roughnessNoiseScale = roughnessNoiseScale * 2
                    roughnessNoise = roughnessNoise + 0.5 *
                        gameMath.PerlinNoise2D(3211, roughnessNoiseScale * ((x - 1) * voxelCount + px),
                            roughnessNoiseScale * ((y - 1) * voxelCount + py))
                    local deltaHeightStep = 0.1
                    roughnessNoise = roughnessNoise * terrainView.roughness
                    local deltaHeight
                    if roughnessNoise > deltaHeightStep then
                        deltaHeight = 1
                    elseif roughnessNoise > -deltaHeightStep then
                        deltaHeight = 0
                    else
                        deltaHeight = -1
                    end
                    if voxelTerrain == terrain.snow then
                        deltaHeight = deltaHeight + 1
                    end
                    -- Foundation
                    local voxelHeightMap = voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py]
                    voxelHeightMap[1] = terrainView.foundation

                    if voxelTerrain == terrain.ice or voxelTerrain == terrain.water or voxelTerrain == terrain.lava then
                        for z = 1, 2 + deltaHeight do
                            voxelHeightMap[z] = terrainView.foundation
                        end
                        if voxelTerrain == terrain.water then
                            for z = 2 + deltaHeight, 4 do
                                voxelHeightMap[z] = terrainView.surface
                            end
                        else
                            for z = 2 + deltaHeight, 5 do
                                voxelHeightMap[z] = terrainView.surface
                            end
                        end
                    else
                        local surfaceNoiseScale = terrainView.surfaceNoiseScale
                        local surfaceNoise = gameMath.PerlinNoise2D(453980,
                            surfaceNoiseScale * ((x - 1) * voxelCount + px),
                            surfaceNoiseScale * ((y - 1) * voxelCount + py))

                        if surfaceNoise > terrainView.surfaceStep + 0.1 then
                            for z = 1, 3 + deltaHeight do
                                voxelHeightMap[z] = terrainView.foundation
                            end
                            voxelHeightMap[5 + deltaHeight] = terrainView.surface
                        elseif surfaceNoise > terrainView.surfaceStep then
                            for z = 1, 4 + deltaHeight do
                                voxelHeightMap[z] = terrainView.foundation
                            end
                            voxelHeightMap[5 + deltaHeight] = terrainView.surface
                        else
                            for z = 1, 5 + deltaHeight do
                                voxelHeightMap[z] = terrainView.foundation
                            end
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
            { 0,          0,          modelScale, 0 },
            { 0,          0,          0,          1 },
        },
    }
    engine.UpdateInstances(index, modelMatrix)

    local directionalLight = {
        color = { 0.2, 0.2, 1, 0.6 },
        direction = { -0.618, -0.618, -1 },
    }
    engine.UpdateLightsUniformBuffer(directionalLight, pointLights)
end

function engine.End()
    print("Lua Start")
end

local cameraPosition = { 0, 4, 10 }
local targetPosition = { 0, 8, 0 }
local t = 0

function engine.Update()
    print("Lua Update")
    if engine.frameCount == 0 then
        collectgarbage("collect")
    end
    t = t + 0.001
    local distance = gameMath.PingPong(0, length, t * 0.2)
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
