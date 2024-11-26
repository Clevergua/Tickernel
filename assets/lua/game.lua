local gameMath = require("gameMath")

local game = {}

-- local humidityStepValue = 0.33
-- local temperatureStepValue = 0.33
local temperatureNoiseScale = 0.097
local humidityNoiseScale = 0.137

function game.GetHumidity(gameData, x, y)
    local level = 2
    local humidity = 0
    local seed = game.humiditySeed;
    for i = 1, level do
        local m = 2 ^ (level - 1)
        humidity = humidity + gameMath.PerlinNoise2D(seed, x * humidityNoiseScale * m, y * humidityNoiseScale * m) / m
        seed = gameMath.LCGRandom(seed)
    end
end

function game.GetTemperature(gameData, x, y)
    local level = 2
    local temperature = 0
    local seed = game.temperatureSeed;
    for i = 1, level do
        local m = 2 ^ (level - 1)
        temperature = temperature +
            gameMath.PerlinNoise2D(seed, x * temperatureNoiseScale * m, y * temperatureNoiseScale * m) / m
        seed = gameMath.LCGRandom(seed)
    end
    temperature = temperature + (x - (gameData.length - 1) / 2) / (gameData.length - 1)


    return temperature, humidity
end

function game.CreateGame(seed, length)
    local gameData = {
        seed = seed,
        temperatureSeed = seed + 1,
        humiditySeed = seed + 2,
        length = length,
        width = 64,
        terrainMap = nil,
        blockMap = nil,
    }
    for x = 1, gameData.length do
        for y = 1, gameData.width do

        end
    end
    return gameData
end

function game.DestroyGame(game)
    game = nil
end

function game.GenerateWorld(length, width, seed)

end

return game
