local gameMath = require("gameMath")
local floor = require("floor")
local game = {
    seed = 0,
    temperatureSeed = 0,
    humiditySeed = 0,
    length = 0,
    width = 0,
    floorMap = nil,
    blockMap = nil,
    humidityStep = 0.21,
    temperatureStep = 0.27,
}

local temperatureNoiseScale = 0.07
local humidityNoiseScale = 0.07

function game.GetFloor(temperature, humidity)
    local floor
    if temperature < -game.temperatureStep then
        if humidity < -game.humidityStep then
            floor = floor.snow
        elseif humidity < game.humidityStep then
            floor = floor.snow
        else
            floor = floor.ice
        end
    elseif temperature < game.temperatureStep then
        if humidity < -game.humidityStep then
            floor = floor.sand
        elseif humidity < game.humidityStep then
            floor = floor.grass
        else
            floor = floor.water
        end
    else
        if humidity < -game.humidityStep then
            floor = floor.lava
        elseif humidity < game.humidityStep then
            floor = floor.volcanic
        else
            floor = floor.volcanic
        end
    end


    return floor
end

function game.GetHumidity(seed, x, y)
    local level = 2
    local humidity = 0
    for i = 1, level do
        local m = 2 ^ (level - 1)
        humidity = humidity + gameMath.PerlinNoise2D(seed, x * humidityNoiseScale * m, y * humidityNoiseScale * m) / m
        seed = gameMath.LCGRandom(seed)
    end
    return humidity
end

function game.GetTemperature(seed, x, y)
    local level = 2
    local temperature = 0
    for i = 1, level do
        local m = 2 ^ (level - 1)
        temperature = temperature +
            gameMath.PerlinNoise2D(seed, x * temperatureNoiseScale * m, y * temperatureNoiseScale * m) / m
        seed = gameMath.LCGRandom(seed)
    end
    -- temperature = temperature + (x - (game.length - 1) / 2) / game.length
    return temperature
end

function game.GenerateWorld(seed, length, width)
    game.seed = seed
    game.temperatureSeed = seed + 1
    game.humiditySeed = seed + 2
    game.length = length
    game.width = width
    floorMap = {}
    game.blockMap = {}
    for x = 1, game.length do
        floorMap[x] = {}
        for y = 1, game.width do
            local temperature = game.GetTemperature(game.temperatureSeed, x, y)
            local humidity = game.GetHumidity(game.humiditySeed, x, y)
            floorMap[x][y] = game.GetFloor(temperature, humidity)
        end
    end
end

return game
