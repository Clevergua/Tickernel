local gameMath = require("gameMath")

local game = {
    terrain = {
        snow = 1,
        ice = 2,
        sand = 3,
        grass = 4,
        water = 5,
        lava = 6,
        andosols = 7,
    },
    seed = 0,
    temperatureSeed = 0,
    humiditySeed = 0,
    length = 0,
    width = 0,
    terrainMap = nil,
    blockMap = nil,
}

local humidityStepValue = 0.2
local temperatureStepValue = 0.2
local temperatureNoiseScale = 0.11
local humidityNoiseScale = 0.11


function game.GetTerrain(temperature, humidity)
    if temperature < -temperatureStepValue then
        if humidity < humidityStepValue then
            return game.terrain.snow
        else
            return game.terrain.ice
        end
    elseif temperature < temperatureStepValue then
        if humidity < -humidityStepValue then
            return game.terrain.sand
        elseif humidity < humidityStepValue then
            return game.terrain.grass
        else
            return game.terrain.water
        end
    else
        if humidity < -humidityStepValue then
            return game.terrain.lava
        else
            return game.terrain.andosols
        end
    end
end

function game.GetHumidity(x, y)
    local level = 2
    local humidity = 0
    local seed = game.humiditySeed;
    for i = 1, level do
        local m = 2 ^ (level - 1)
        humidity = humidity + gameMath.PerlinNoise2D(seed, x * humidityNoiseScale * m, y * humidityNoiseScale * m) / m
        seed = gameMath.LCGRandom(seed)
    end
    return humidity
end

function game.GetTemperature(x, y)
    local level = 2
    local temperature = 0
    local seed = game.temperatureSeed;
    for i = 1, level do
        local m = 2 ^ (level - 1)
        temperature = temperature +
            gameMath.PerlinNoise2D(seed, x * temperatureNoiseScale * m, y * temperatureNoiseScale * m) / m
        seed = gameMath.LCGRandom(seed)
    end
    temperature = temperature + (x - (game.length - 1) / 2) / game.length
    return temperature
end

function game.GenerateWorld(seed, length, width)
    game.seed = seed
    game.temperatureSeed = seed + 1
    game.humiditySeed = seed + 2
    game.length = length
    game.width = width
    game.terrainMap = {}
    game.blockMap = {}
    for x = 1, game.length do
        game.terrainMap[x] = {}
        for y = 1, game.width do
            local temperature = game.GetTemperature(x, y)
            local humidity = game.GetHumidity(x, y)
            game.terrainMap[x][y] = game.GetTerrain(temperature, humidity)
        end
    end
end

return game
