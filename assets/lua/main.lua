local engine = require("engine")
local gameMath = require("gameMath")
local game = require("game")


local blockToColor = {
    [1] = { 210, 150, 95, 255 },
    [2] = { 225, 225, 225, 255 },
    [3] = { 190, 190, 245, 255 },
    [4] = { 225, 200, 150, 255 },
    [5] = { 170, 225, 115, 255 },
    [6] = { 86, 98, 185, 255 },
    [7] = { 225, 43, 67, 255 },
    [8] = { 51, 41, 41, 255 },
}
local block = {
    none = 0,
    dirt = 1,

    snow = 2,
    ice = 3,

    sand = 4,
    grass = 5,
    water = 6,

    lava = 7,
    andosols = 8,
}

-- local humidityStepValue = 0.33
-- local temperatureStepValue = 0.33
-- local temperatureSeed = 4214
-- local humiditySeed = 7645
-- local temperatureNoiseScale = 0.097
-- local humidityNoiseScale = 0.137
local length = 64
local width = 32
local pixel = 16
local scale = 1 / pixel;

-- local function GetTemperature(x, y)
--     local result = 0
--     local level = 1
--     local seed = temperatureSeed;
--     for i = 1, level do
--         local m = 2 ^ (level - 1)
--         result = result +
--             gameMath.PerlinNoise2D(seed, x * temperatureNoiseScale * m, y * temperatureNoiseScale * m) / m
--         seed = gameMath.LCGRandom(seed)
--     end
--     return result
-- end

-- local function GetHumidity(x, y)
--     local result = 0
--     local level = 1
--     local seed = humiditySeed;
--     for i = 1, level do
--         local m = 2 ^ (level - 1)
--         result = result + gameMath.PerlinNoise2D(seed, x * humidityNoiseScale * m, y * humidityNoiseScale * m) / m
--         seed = gameMath.LCGRandom(seed)
--     end
--     return result
-- end

-- local function GetBlock(temperature, humidity)
--     if temperature < -temperatureStepValue then
--         if humidity < -humidityStepValue then
--             return block.iceSand
--         elseif humidity < humidityStepValue then
--             return block.snow
--         else
--             return block.ice
--         end
--     elseif temperature < temperatureStepValue then
--         if humidity < -humidityStepValue then
--             return block.sand
--         elseif humidity < humidityStepValue then
--             return block.grass
--         else
--             return block.water
--         end
--     else
--         if humidity < -humidityStepValue then
--             return block.lava
--         elseif humidity < humidityStepValue then
--             return block.andosols
--         else
--             return block.swamp
--         end
--     end
-- end
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
        local count = 4
        local instances = {}
        for i = 1, count do
            local x = math.abs(gameMath.LCGRandom((index + 13251) * 525234532 + i * 42342345)) % length
            local y = math.abs(gameMath.LCGRandom((x - 2317831) * 431513511 + index * 24141312 - i * 2131204)) % width
            instances[i] = {
                { scale, 0,     0,     x },
                { 0,     scale, 0,     y },
                { 0,     0,     scale, 0 },
                { 0,     0,     0,     1 },
            }
        end
        engine.DrawModel(instances, model)
    end


    -- print("Generating world..")
    -- local temperatureMap = {}
    -- local humidityMap = {}

    -- for x = 1, length do
    --     temperatureMap[x] = {}
    --     humidityMap[x] = {}
    --     for y = 1, width do
    --         temperatureMap[x][y] = GetTemperature(x, y)
    --         humidityMap[x][y] = GetHumidity(x, y)
    --     end
    -- end

    -- local pixelMap = {}
    -- local height = 8
    -- for x = 1, length * pixel do
    --     pixelMap[x] = {}
    --     for y = 1, width * pixel do
    --         pixelMap[x][y] = {}
    --         for z = 1, height do
    --             pixelMap[x][y][z] = block.none
    --         end
    --     end
    -- end
    -- print("Generating dirt..")
    -- for x = 1, length * pixel do
    --     for y = 1, width * pixel do
    --         for z = 1, 4 do
    --             pixelMap[x][y][z] = block.dirt
    --         end
    --     end
    -- end

    -- local pointLights = {}
    -- print("Generating surface..")
    -- for x = 1, length do
    --     for y = 1, width do
    --         local centerTemperature = temperatureMap[x][y]
    --         local centerHumidity = humidityMap[x][y]
    --         -- Target values
    --         local targetTemperature, targetHumidity
    --         if centerTemperature < -temperatureStepValue then
    --             targetTemperature = -1
    --         elseif centerTemperature < temperatureStepValue then
    --             targetTemperature = 0
    --         else
    --             targetTemperature = 1
    --         end

    --         if centerHumidity < -humidityStepValue then
    --             targetHumidity = -1
    --         elseif centerHumidity < humidityStepValue then
    --             targetHumidity = 0
    --         else
    --             targetHumidity = 1
    --         end

    --         if GetBlock(centerTemperature, centerHumidity) == block.lava then
    --             if #pointLights < 256 then
    --                 table.insert(pointLights, {
    --                     color = { 0.7, 0.4, 0, 1 },
    --                     position = { x, y, 0.125 },
    --                     range = 2,
    --                 })
    --             else
    --                 break
    --             end
    --         end

    --         for px = 1, pixel do
    --             for py = 1, pixel do
    --                 local dx = (px - pixel / 2 - 0.5) / pixel
    --                 local dy = (py - pixel / 2 - 0.5) / pixel
    --                 local temperature = GetTemperature((x + dx), (y + dy))
    --                 local humidity = GetHumidity((x + dx), (y + dy))
    --                 local t = math.max(math.abs(dx), math.abs(dy)) * 2
    --                 temperature = gameMath.Lerp(targetTemperature, temperature, t)
    --                 humidity = gameMath.Lerp(targetHumidity, humidity, t)
    --                 local holeNoiseScale = 0.27
    --                 local holeNoise = gameMath.PerlinNoise2D(2134, holeNoiseScale * ((x - 1) * pixel + px),
    --                     holeNoiseScale * ((y - 1) * pixel + py));
    --                 holeNoise = holeNoise + t
    --                 if holeNoise < 1.25 then
    --                     local b = GetBlock(temperature, humidity)
    --                     local heightMap = pixelMap[(x - 1) * pixel + px][(y - 1) * pixel + py]
    --                     if b == block.water or b == block.ice or b == block.lava or b == block.swamp then
    --                         heightMap[2] = block.none
    --                         heightMap[3] = block.none
    --                         heightMap[4] = block.none
    --                     else
    --                         heightMap[5] = b
    --                         if holeNoise < 0.4 then
    --                             heightMap[6] = b
    --                         end
    --                     end
    --                 else
    --                     -- skip
    --                 end
    --             end
    --         end
    --     end
    -- end
    print("Generating vertices..")
    local blockToVertices = {}
    local blockToColors = {}
    local blockToNormals = {}
    for x = 1, length * pixel do
        for y = 1, width * pixel do
            for z = 1, height do
                if pixelMap[x][y][z] ~= block.none then
                    if blockToVertices[pixelMap[x][y][z]] == nil then
                        blockToVertices[pixelMap[x][y][z]] = {}
                        blockToColors[pixelMap[x][y][z]] = {}
                        blockToNormals[pixelMap[x][y][z]] = {}
                    end
                    table.insert(blockToVertices[pixelMap[x][y][z]], { x, y, z - 4 })
                    table.insert(blockToColors[pixelMap[x][y][z]], blockToColor[pixelMap[x][y][z]])
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
                    { scale, 0,     0,     0 },
                    { 0,     scale, 0,     0 },
                    { 0,     0,     scale, -4 * scale },
                    { 0,     0,     0,     1 },
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
