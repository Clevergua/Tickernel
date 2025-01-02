local gameMath = require("gameMath")

local game = {
    terrain = {
        snow = 1,
        ice = 2,
        sand = 3,
        grass = 4,
        water = 5,
        lava = 6,
        volcanic = 7,
    },
    seed = 0,
    temperatureSeed = 0,
    humiditySeed = 0,
    length = 0,
    width = 0,
    terrainMap = nil,
    blockMap = nil,
    humidityStep = 0.15,
    temperatureStep = 0.23,
}


local temperatureNoiseScale = 0.23
local humidityNoiseScale = 0.23

function game.GetTerrain(temperature, humidity)
    local terrain
    if temperature < -game.temperatureStep then
        if humidity < -game.humidityStep then
            terrain = game.terrain.snow
        elseif humidity < game.humidityStep then
            terrain = game.terrain.snow
        else
            terrain = game.terrain.ice
        end
    elseif temperature < game.temperatureStep then
        if humidity < -game.humidityStep then
            terrain = game.terrain.sand
        elseif humidity < game.humidityStep then
            terrain = game.terrain.grass
        else
            terrain = game.terrain.water
        end
    else
        if humidity < -game.humidityStep then
            terrain = game.terrain.lava
        elseif humidity < game.humidityStep then
            terrain = game.terrain.volcanic
        else
            terrain = game.terrain.volcanic
        end
    end


    return terrain
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
