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
            local ns = 0.007
            local t = gameMath.PerlinNoise2D(43214, x * ns, y * ns)
            if t < -0.2 then
                temperatureMap[x][y] = -1
            elseif t < 0.2 then
                temperatureMap[x][y] = 0
            else
                temperatureMap[x][y] = 1
            end
            local h = gameMath.PerlinNoise2D(3321, x * ns, y * ns)
            if h < -0.2 then
                humidityMap[x][y] = -1
            elseif h < 0.2 then
                humidityMap[x][y] = 0
            else
                humidityMap[x][y] = 1
            end
            -- manaMap[x][y] = gameMath.PerlinNoise2D(314, x * ns, y * ns)
            -- terrainMap[x] = {}
        end
    end
end

function game.Start()

end

return game
