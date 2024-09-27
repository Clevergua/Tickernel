local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    -- local vertices = {}
    -- local colors = {}
    -- local dirtColor = { 0.6, 0.3, 0.3, 0 }
    -- local grassColor = { 0.3, 0.5, 0.2, 0 }
    -- local x_max = 100
    -- local y_max = 100

    -- for y = 1, y_max do
    --     for x = 1, x_max do
    --         table.insert(vertices, { (x - x_max / 2) * 0.01, (y - y_max / 2) * 0.01, 0.5 })
    --         table.insert(colors, dirtColor)
    --     end
    -- end

    -- for y = 1, y_max do
    --     for x = 1, x_max do
    --         table.insert(vertices, { (x - x_max / 2) * 0.01, (y - y_max / 2) * 0.01, 0.0 })
    --         table.insert(colors, grassColor)
    --     end
    -- end

    -- local vertices = {}
    -- local colors = {}
    -- local dirtColor = { 0.6, 0.3, 0.3, 0 }
    -- local grassColor = { 0.3, 0.5, 0.2, 0 }
    --     for y = 1, 100 do
    --         for x = 1, 100 do
    --             local isFirst = true

    --             for z = 1, 100 do

    --             local noiseValue = gameMath.PerlinNoise3D(0, x * 0.007, y * 0.007, z * 0.007)
    --             local temperature = gameMath.PerlinNoise3D(321, x * 0.07, y * 0.07, z * 0.07)
    --             local density = noiseValue - z * 0.015 + 1
    --             if density > 0.3 then
    --                 if isFirst then
    --                     table.insert(vertices, { 0.01 * x - 0.5, 0.01 * y - 0.5, 0.01 * z - 0.5 })

    --                     if temperature > 0.1 then
    --                         local targetColor = { dirtColor[1] + 0.3, dirtColor[2] + 0.3, dirtColor[3] + 0.3, dirtColor
    --                             [4] }
    --                         table.insert(colors, targetColor)
    --                     else
    --                         local targetColor = { grassColor[1] + 0.3, grassColor[2] + 0.3, grassColor[3] + 0.3,
    --                             grassColor
    --                                 [4] }
    --                         table.insert(colors, targetColor)
    --                     end
    --                     isFirst = false
    --                 else
    --                     table.insert(vertices, { 0.01 * x - 0.5, 0.01 * y - 0.5, 0.01 * z - 0.5 })
    --                     if temperature > 0.1 then
    --                         table.insert(colors, dirtColor)
    --                     else
    --                         table.insert(colors, grassColor)
    --                     end
    --                 end
    --             end
    --         end
    --     end
    -- end

    -- local vertices = {}
    -- local colors = {}
    -- local xMax = 200;
    -- local yMax = 200;
    -- local zMax = 200;
    -- local center = { xMax / 2.0, yMax / 2.0, zMax / 2.0 }
    -- local r = 90
    -- local color = { 0.2, 0.2, 0.6, 1 }
    -- local light = 0.3
    -- local color2 = { color[1] + light, color[2] + light, color[3] + light, 1 }
    -- for i = 1, xMax do
    --     for j = 1, yMax do
    --         local isFirst = true
    --         for k = 1, zMax do
    --             local rk = zMax - k
    --             local x = i - center[1]
    --             local y = j - center[2]
    --             local z = rk - center[3]
    --             if x * x + y * y + z * z < r * r then
    --                 if isFirst then
    --                     isFirst = false
    --                     table.insert(colors, color2)
    --                 else
    --                     table.insert(colors, color)
    --                 end
    --                 table.insert(vertices, { x / xMax, y / yMax, z / zMax })
    --             end
    --         end
    --     end
    -- end


    local vertices = {}
    local colors = {}
    local xMax = 1024;
    local zMax = 1024;
    for i = 1, xMax do
        for k = 1, zMax do
            table.insert(colors, { i / xMax, k / zMax, 1, 1 })
            table.insert(vertices, { i - xMax / 2, 0, k - zMax / 2 })
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

local cameraPosition = { 0, -1024, 0 }
local targetPosition = { 0, 0, 0 }
function gameState.Update()
    print("Lua Update")
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
