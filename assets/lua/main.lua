local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    local vertices = {}
    local colors = {}
    local normalFlags = {}
    local xMax = 200.0;
    local yMax = 200.0;
    local indexMap = {}
    for i = 1, xMax do
        indexMap[i] = {}
        for j = 1, yMax do
            table.insert(vertices, { i - xMax / 2, j - yMax / 2, 0 })
            table.insert(colors, { i / xMax, j / yMax, 0, 1 })
            table.insert(normalFlags, 0)
            indexMap[i][j] = #vertices;
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

local cameraPosition = { 0, -200, 0 }
local targetPosition = { 0, 0, 0 }
function gameState.Update()
    print("Lua Update")
    cameraPosition[3] = cameraPosition[3] + 1
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
