local gameMath = require("gameMath")

local game = {}

function game.GenerateWorld(length, width)
    local length = length
    local width = width
    local temperatureMap = {}
    local humidityMap = {}
    local terrainMap = {}
    local manaMap = {}
    for x = 1, length do
        temperatureMap[x] = {}
        humidityMap[x] = {}
        terrainMap[x] = {}
        manaMap[x] = {}
        for y = 1, width do
            gameMath.PerlinNoise2D(0, x, y)
        end
    end
end

function game.Start()

end

return game
