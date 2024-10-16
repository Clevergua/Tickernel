local gameMath = require("gameMath")

local game = {}

function game.GenerateWorld(length, width)
    local world = {
        length = length,
        width = width,
        temperatureMap = {},
        humidityMap = {},
        terrainMap = {},
        manaMap = {},
    }
    for x = 1, length do
        world.temperatureMap[x] = {}
        world.humidityMap[x] = {}
        world.terrainMap[x] = {}
        world.manaMap[x] = {}
        for y = 1, width do
            gameMath.PerlinNoise2D(0, x, y)
        end
    end

    return world
end

function game.Start()

end

return game
