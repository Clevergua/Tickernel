local gameState = require("gameState")
local gameMath = require("gameMath")

function CalculateNormalFlag(indexMap, xMax, yMax, zMax, cx, cy, cz)
    local normalFlag = 0
    -- local index = 0
    -- for dz = -1, 1 do
    --     for dy = -1, 1 do
    --         for dx = -1, 1 do
    --             if dx == 0 and dy == 0 and dz == 0 then
    --                 -- continue
    --             else
    --                 local x = cx + dx
    --                 local y = cy + dy
    --                 local z = cz + dz
    --                 if x < 1 or x > xMax or y < 1 or y > yMax or z < 1 or z > zMax or indexMap[x][y][z] == 0 then
    --                     normalFlag = normalFlag + 2 ^ index
    --                 else
    --                     -- do nothing
    --                 end
    --             end
    --             index = index + 1
    --         end
    --     end
    -- end
    if cx - 1 < 1 or indexMap[cx - 1][cy][cz] == 0 then
        normalFlag = normalFlag + 2 ^ 0
    end
    if cx + 1 > xMax or indexMap[cx + 1][cy][cz] == 0 then
        normalFlag = normalFlag + 2 ^ 1
    end
    if cy - 1 < 1 or indexMap[cx][cy - 1][cz] == 0 then
        normalFlag = normalFlag + 2 ^ 2
    end
    if cy + 1 > yMax or indexMap[cx][cy + 1][cz] == 0 then
        normalFlag = normalFlag + 2 ^ 3
    end
    if cz - 1 < 1 or indexMap[cx][cy][cz - 1] == 0 then
        normalFlag = normalFlag + 2 ^ 4
    end
    if cz + 1 > zMax or indexMap[cx][cy][cz + 1] == 0 then
        normalFlag = normalFlag + 2 ^ 5
    end
    return normalFlag
end

function gameState.Start()
    print("Lua Start")
    local vertices = {}
    local colors = {}
    local normals = {}
    local xMax = 100.0;
    local yMax = 100.0;
    local zMax = 1.0;
    local indexMap = {}
    for i = 1, xMax do
        indexMap[i] = {}
        for j = 1, yMax do
            indexMap[i][j] = {}
            for k = 1, zMax do
                if true then
                    table.insert(vertices, { i - xMax / 2, j - yMax / 2, k - zMax / 2 })
                    -- table.insert(colors, { i / xMax, j / yMax, k / zMax, 1 })
                    table.insert(colors, { 1, 1, 1, 1 })
                    table.insert(normals, 0)
                    indexMap[i][j][k] = #vertices;
                else
                    indexMap[i][j][k] = 0;
                end
            end
        end
    end

    for x = 1, xMax do
        for y = 1, yMax do
            for z = 1, zMax do
                local index = indexMap[x][y][z]
                normals[index] = CalculateNormalFlag(indexMap, xMax, yMax, zMax, x, y, z)
            end
        end
    end
    local index = gameState.AddModel(vertices, colors, normals)
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
