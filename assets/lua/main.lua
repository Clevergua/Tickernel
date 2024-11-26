local engine = require("engine")
local gameMath = require("gameMath")
local game = require("game")

local pixelCount = 16
local modelScale = 1 / pixelCount;
local pixel = {
    none = { 0, 0, 0, 0 },
    snow = { 225, 225, 225, 255 },
    ice = { 190, 190, 245, 255 },
    sand = { 225, 200, 150, 255 },
    grass = { 170, 225, 115, 255 },
    water = { 86, 98, 185, 255 },
    lava = { 225, 43, 67, 255 },
    andosols = { 51, 41, 41, 255 },
}
function engine.Start()
    print("Lua Start")

    local length = 64
    local width = 32

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
        local count = 4
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

    local pixelMap = {}
    local height = 8
    for x = 1, length * pixelCount do
        pixelMap[x] = {}
        for y = 1, width * pixelCount do
            pixelMap[x][y] = {}
            for z = 1, height do
                pixelMap[x][y][z] = pixel.none
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

            for px = 1, pixelCount do
                for py = 1, pixelCount do
                    local dx = (px - pixelCount / 2 - 0.5) / pixelCount
                    local dy = (py - pixelCount / 2 - 0.5) / pixelCount
                    local temperature = game.GetTemperature((x + dx), (y + dy))
                    local humidity = game.GetHumidity((x + dx), (y + dy))
                    local t = math.max(math.abs(dx), math.abs(dy)) * 2
                    temperature = gameMath.Lerp(targetTemperature, temperature, t)
                    humidity = gameMath.Lerp(targetHumidity, humidity, t)
                    local holeNoiseScale = 0.27
                    local holeNoise = gameMath.PerlinNoise2D(2134, holeNoiseScale * ((x - 1) * pixelCount + px),
                        holeNoiseScale * ((y - 1) * pixelCount + py))
                    holeNoise = holeNoise + t
                    if holeNoise < 1.25 then
                        local pixelTerrain = game.GetTerrain(temperature, humidity)
                        local heightMap = pixelMap[(x - 1) * pixelCount + px][(y - 1) * pixelCount + py]
                        if pixelTerrain == terrain.water or pixelTerrain == terrain.ice or pixelTerrain == terrain.lava then
                            heightMap[2] = 0
                            heightMap[3] = 0
                            heightMap[4] = 0
                        else
                            heightMap[5] = pixelTerrain
                            if holeNoise < 0.4 then
                                heightMap[6] = pixelTerrain
                            end
                        end
                    else
                        -- skip
                    end
                end
            end
        end
    end
    print("Generating vertices..")
    local blockToVertices = {}
    local blockToColors = {}
    local blockToNormals = {}
    for x = 1, length * pixelCount do
        for y = 1, width * pixelCount do
            for z = 1, height do
                if pixelMap[x][y][z] ~= pixel.none then
                    if blockToVertices[pixelMap[x][y][z]] == nil then
                        blockToVertices[pixelMap[x][y][z]] = {}
                        blockToColors[pixelMap[x][y][z]] = {}
                        blockToNormals[pixelMap[x][y][z]] = {}
                    end
                    table.insert(blockToVertices[pixelMap[x][y][z]], { x, y, z - 4 })
                    table.insert(blockToColors[pixelMap[x][y][z]], pixelMap[x][y][z])
                    table.insert(blockToNormals[pixelMap[x][y][z]], { 0, 0, 0 })
                end
            end
        end
    end
    print("Drawing models..")
    for i = 1, 10 do
        if blockToVertices[i] ~= nil then
            engine.SetNormals(blockToVertices[i], blockToNormals[i])
            local index = engine.AddModel(blockToVertices[i], blockToColors[i], blockToNormals[i])
            local modelMatrix = {
                {
                    { modelScale, 0,          0,          0 },
                    { 0,          modelScale, 0,          0 },
                    { 0,          0,          modelScale, -4 * modelScale },
                    { 0,          0,          0,          1 },
                },
            }
            engine.UpdateInstances(index, modelMatrix)
        end
    end
    local directionalLight = {
        color = { 1, 1, 1, 0.382 },
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
