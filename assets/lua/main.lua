local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    local vertices = {}
    local colors = {}
    local dirtColor = { 0.6, 0.3, 0.3 ,0}
    local grassColor = { 0.3, 0.5, 0.2 ,0}
    for z = 1, 100 do
        for y = 1, 200 do
            for x = 1, 200 do
                local noiseValue = gameMath.PerlinNoise3D(0, x * 0.007, y * 0.007, z * 0.007)
                local temperature = gameMath.PerlinNoise3D(321, x * 0.07, y * 0.07, z * 0.07)
                local density = noiseValue - z * 0.015 + 1
                if density > 0.3 then
                    table.insert(vertices, { x - 50, y - 50, z - 50 })
                    if temperature > 0.1 then
                        table.insert(colors, dirtColor)
                    else
                        table.insert(colors, grassColor)
                    end
                end
            end
        end
    end
    local index = gameState.AddModel(vertices, colors)
    local model = {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 },
    }
    gameState.UpdateModel(index, model)
end

function gameState.End()
    print("Lua Start")
end

function gameState.Update()
    print("Lua Update")
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
