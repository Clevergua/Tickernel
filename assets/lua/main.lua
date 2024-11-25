local gameState = require("gameState")
local gameMath = require("gameMath")
local game = require("game")


local blockToColor = {
    [1] = { 210, 150, 95, 255 },
    [2] = { 221, 160, 221, 255 },
    [3] = { 170, 225, 115, 255 },
    [4] = { 86, 98, 185, 255 },
    [5] = { 225, 225, 225, 255 },
    [6] = { 190, 190, 245, 255 },
    [7] = { 225, 200, 150, 255 },
    [8] = { 225, 43, 67, 255 },
    [9] = { 51, 41, 41, 255 },
    [10] = { 254, 43, 67, 255 },
}
local block = {
    none = 0,
    dirt = 1,
    iceSand = 2,
    grass = 3,
    water = 4,
    snow = 5,
    ice = 6,
    sand = 7,
    lava = 8,
    andosols = 9,
    swamp = 10,
}

local humidityStepValue = 0.2
local temperatureStepValue = 0.2
local temperatureSeed = 3213
local humiditySeed = 56234
local temperatureNoiseScale = 0.047
local humidityNoiseScale = 0.047

local function GetTemperature(x, y)
    local result = 0
    local level = 2
    for i = 0, level - 1 do
        result = result +
            gameMath.PerlinNoise2D(temperatureSeed + i, x * temperatureNoiseScale * (2 ^ level),
                y * temperatureNoiseScale * (2 ^ level)) / level
    end
    return result
end

local function GetHumidity(x, y)
    local result = 0
    local level = 2
    for i = 0, level - 1 do
        result = result +
            gameMath.PerlinNoise2D(humiditySeed + i, x * humidityNoiseScale * (2 ^ level),
                y * humidityNoiseScale * (2 ^ level)) / level
    end
    return result
end

local function GetBlock(temperature, humidity)
    if temperature < -temperatureStepValue then
        if humidity < -humidityStepValue then
            return block.iceSand
        elseif humidity < humidityStepValue then
            return block.snow
        else
            return block.ice
        end
    elseif temperature < temperatureStepValue then
        if humidity < -humidityStepValue then
            return block.sand
        elseif humidity < humidityStepValue then
            return block.grass
        else
            return block.water
        end
    else
        if humidity < -humidityStepValue then
            return block.lava
        elseif humidity < humidityStepValue then
            return block.andosols
        else
            return block.swamp
        end
    end
end
function gameState.Start()
    print("Lua Start")

    local length = 32
    local width = 32

    local pixel = 16
    local scale = 1 / pixel;
    local modelsPath = gameState.assetsPath ..
        gameState.pathSeparator .. "models" .. gameState.pathSeparator
    print("Loading models")
    local models = {
        gameState.LoadModel(modelsPath .. "LargeBuilding01_0.tknvox"),
        gameState.LoadModel(modelsPath .. "SmallBuilding01_0.tknvox"),
        gameState.LoadModel(modelsPath .. "SmallBuilding02_0.tknvox"),
        gameState.LoadModel(modelsPath .. "SmallBuilding03_0.tknvox"),
        gameState.LoadModel(modelsPath .. "SmallBuilding04_0.tknvox"),
        gameState.LoadModel(modelsPath .. "TallBuilding01_0.tknvox"),
        gameState.LoadModel(modelsPath .. "TallBuilding02_0.tknvox"),
        gameState.LoadModel(modelsPath .. "TallBuilding03_0.tknvox"),
    }
    for index, model in ipairs(models) do
        local count = 10
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
        gameState.DrawModel(instances, model)
    end


    print("Generating world..")
    local temperatureMap = {}
    local humidityMap = {}

    for x = 1, length do
        temperatureMap[x] = {}
        humidityMap[x] = {}
        for y = 1, width do
            temperatureMap[x][y] = GetTemperature(x, y)
            humidityMap[x][y] = GetHumidity(x, y)
        end
    end

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
                pixelMap[x][y][z] = block.dirt
            end
        end
    end

    local pointLights = {}
    print("Generating surface..")
    for x = 1, length do
        for y = 1, width do
            local centerTemperature = temperatureMap[x][y]
            local centerHumidity = humidityMap[x][y]
            -- Target values
            local targetTemperature, targetHumidity
            if centerTemperature < -temperatureStepValue then
                targetTemperature = -1
            elseif centerTemperature < temperatureStepValue then
                targetTemperature = 0
            else
                targetTemperature = 1
            end

            if centerHumidity < -humidityStepValue then
                targetHumidity = -1
            elseif centerHumidity < humidityStepValue then
                targetHumidity = 0
            else
                targetHumidity = 1
            end

            if GetBlock(centerTemperature, centerHumidity) == block.lava then
                table.insert(pointLights, {
                    color = { 0.7, 0.4, 0, 1 },
                    position = { x, y, 0.125 },
                    range = 2,
                })
            end

            for px = 1, pixel do
                for py = 1, pixel do
                    local dx = (px - pixel / 2 - 0.5) / pixel * 2
                    local dy = (py - pixel / 2 - 0.5) / pixel * 2
                    local temperature = GetTemperature((x + dx), (y + dy))
                    local humidity = GetHumidity((x + dx), (y + dy))
                    local t = 0.5 * (math.abs(dx) + math.abs(dy))

                    temperature = gameMath.Lerp(targetTemperature, temperature, t)
                    humidity = gameMath.Lerp(targetHumidity, humidity, t)

                    local holeNoiseScale = 0.37
                    local holeNoise = gameMath.PerlinNoise2D(2134, holeNoiseScale * ((x - 1) * pixel + px),
                        holeNoiseScale * ((y - 1) * pixel + py));
                    holeNoise = holeNoise + t
                    if holeNoise < 0.95 then
                        local b = GetBlock(temperature, humidity)
                        if b == block.water or b == block.ice or b == block.lava or b == block.swamp then
                            pixelMap[(x - 1) * pixel + px][(y - 1) * pixel + py][2] = b
                            pixelMap[(x - 1) * pixel + px][(y - 1) * pixel + py][3] = b
                            pixelMap[(x - 1) * pixel + px][(y - 1) * pixel + py][4] = b
                        else
                            pixelMap[(x - 1) * pixel + px][(y - 1) * pixel + py][5] = b
                            if holeNoise < 0.15 then
                                pixelMap[(x - 1) * pixel + px][(y - 1) * pixel + py][6] = b
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
            gameState.SetNormals(blockToVertices[i], blockToNormals[i])
            local index = gameState.AddModel(blockToVertices[i], blockToColors[i], blockToNormals[i])
            local modelMatrix = {
                {
                    { scale, 0,     0,     0 },
                    { 0,     scale, 0,     0 },
                    { 0,     0,     scale, -4 * scale },
                    { 0,     0,     0,     1 },
                },
            }
            gameState.UpdateInstances(index, modelMatrix)
        end
    end
    local directionalLight = {
        color = { 1, 1, 1, 0.618 },
        direction = { 1, -1, -1 },
    }
    gameState.UpdateLightsUniformBuffer(directionalLight, pointLights)
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 12, 15 }
local targetPosition = { 0, 16, 0 }
local t = 0

function gameState.Update()
    print("Lua Update")
    if gameState.frameCount == 0 then
        collectgarbage("collect")
    end
    t = t + 0.001
    local distance = gameMath.PingPong(12, 20, t)
    cameraPosition[1] = distance
    targetPosition[1] = distance
    -- collectgarbage("collect")
    local memoryUsage = collectgarbage("count")
    print("Current memory usage: ", memoryUsage, "KB")
    gameState.UpdateGlobalUniformBuffer(cameraPosition, targetPosition)
    gameState.frameCount = gameState.frameCount + 1
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
