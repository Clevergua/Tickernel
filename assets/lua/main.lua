local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    local vertices = {}
    local colors = {}
    local dirtColor = { 0.6, 0.3, 0.3, 0 }
    local grassColor = { 0.3, 0.5, 0.2, 0 }
    local x_max = 100
    local y_max = 100

    for y = 1, y_max do
        for x = 1, x_max do
            table.insert(vertices, { (x - x_max / 2) * 0.01, (y - y_max / 2) * 0.01, 0.5 })
            table.insert(colors, dirtColor)
        end
    end

    for y = 1, y_max do
        for x = 1, x_max do
            table.insert(vertices, { (x - x_max / 2) * 0.01, (y - y_max / 2) * 0.01, 0.0 })
            table.insert(colors, grassColor)
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

local cameraPosition = { 0, -2, 2 }
local targetPosition = { 0, 0, 0 }
function gameState.Update()
    print("Lua Update")
    cameraPosition[2] = cameraPosition[2] + 0.1
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
