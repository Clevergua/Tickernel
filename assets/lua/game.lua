local gameMath = require("gameMath")

local game = {
    temperatureMap = {},
    humidityMap = {}
}

function game.GenerateWorld(length, width)
    local length = length
    local width = width
    local temperatureMap = game.temperatureMap
    local humidityMap = game.humidityMap
    game.gridPixelLength = 4
    for x = 1, length * game.gridPixelLength do
        temperatureMap[x] = {}
        humidityMap[x] = {}
        for y = 1, width * game.gridPixelLength do
            local ns = 0.017
            temperatureMap[x][y] = gameMath.PerlinNoise2D(43214, x * ns, y * ns) +
                gameMath.PerlinNoise2D(3123, x * ns * 0.5, y * ns * 0.5)

            humidityMap[x][y] = gameMath.PerlinNoise2D(3321, x * ns, y * ns) +
                gameMath.PerlinNoise2D(689, x * ns * 0.5, y * ns * 0.5)
        end
    end
end

function game.Start()

end

return game
