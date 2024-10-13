local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.Start()
    print("Lua Start")
    local vertices = {}
    local colors = {}
    local normals = {}
    local xMax = 100.0;
    local yMax = 100.0;
    local zMax = 100.0;
    local indexMap = {}
    local r = 50
    for x = 1, xMax do
        indexMap[x] = {}
        for y = 1, yMax do
            indexMap[x][y] = {}
            for z = 1, zMax do
                local n = gameMath.PerlinNoise3D(312, x * 0.03, y * 0.03, z * 0.03)
                if n  < 0.2  then
                -- if (x - r) ^ 2 + (y - r) ^ 2 + (z - r) ^ 2 < r ^ 2 then
                    table.insert(vertices, { x - xMax / 2, y - yMax / 2, z - zMax / 2 })
                    table.insert(colors, { x / xMax, y / yMax, z / zMax, 1 })
                    table.insert(normals, { 0, 0, 0 })
                    indexMap[x][y][z] = #vertices;
                else
                    indexMap[x][y][z] = 0;
                end
            end
        end
    end
    for x = 1, xMax do
        for y = 1, yMax do
            for z = 1, zMax do
                if indexMap[x][y][z] == 0 then
                else
                    local index = indexMap[x][y][z]
                    if x - 1 < 1 or indexMap[x - 1][y][z] == 0 then
                        normals[index][1] = normals[index][1] + 1;
                    end
                    if x + 1 > xMax or indexMap[x + 1][y][z] == 0 then
                        normals[index][1] = normals[index][1] - 1;
                    end

                    if y - 1 < 1 or indexMap[x][y - 1][z] == 0 then
                        normals[index][2] = normals[index][2] + 1;
                    end
                    if y + 1 > yMax or indexMap[x][y + 1][z] == 0 then
                        normals[index][2] = normals[index][2] - 1;
                    end

                    if z - 1 < 1 or indexMap[x][y][z - 1] == 0 then
                        normals[index][3] = normals[index][3] + 1;
                    end
                    if z + 1 > zMax or indexMap[x][y][z + 1] == 0 then
                        normals[index][3] = normals[index][3] - 1;
                    end

                    local length = 1 / 1.414 / 1.414;
                    if x - 1 < 1 or y - 1 < 1 or z - 1 < 1 or indexMap[x - 1][y - 1][z - 1] == 0 then
                        normals[index][1] = normals[index][1] + length
                        normals[index][2] = normals[index][2] + length
                        normals[index][3] = normals[index][3] + length
                    end
                    if x - 1 < 1 or y + 1 > yMax or z - 1 < 1 or indexMap[x - 1][y + 1][z - 1] == 0 then
                        normals[index][1] = normals[index][1] + length
                        normals[index][2] = normals[index][2] - length
                        normals[index][3] = normals[index][3] + length
                    end
                    if x - 1 < 1 or y - 1 < 1 or z + 1 > zMax or indexMap[x - 1][y - 1][z + 1] == 0 then
                        normals[index][1] = normals[index][1] + length
                        normals[index][2] = normals[index][2] + length
                        normals[index][3] = normals[index][3] - length
                    end
                    if x - 1 < 1 or y + 1 > yMax or z + 1 > zMax or indexMap[x - 1][y + 1][z + 1] == 0 then
                        normals[index][1] = normals[index][1] + length
                        normals[index][2] = normals[index][2] - length
                        normals[index][3] = normals[index][3] - length
                    end

                    if x + 1 > xMax or y - 1 < 1 or z - 1 < 1 or indexMap[x + 1][y - 1][z - 1] == 0 then
                        normals[index][1] = normals[index][1] - length
                        normals[index][2] = normals[index][2] + length
                        normals[index][3] = normals[index][3] + length
                    end
                    if x + 1 > xMax or y + 1 > yMax or z - 1 < 1 or indexMap[x + 1][y + 1][z - 1] == 0 then
                        normals[index][1] = normals[index][1] - length
                        normals[index][2] = normals[index][2] - length
                        normals[index][3] = normals[index][3] + length
                    end
                    if x + 1 > xMax or y - 1 < 1 or z + 1 > zMax or indexMap[x + 1][y - 1][z + 1] == 0 then
                        normals[index][1] = normals[index][1] - length
                        normals[index][2] = normals[index][2] + length
                        normals[index][3] = normals[index][3] - length
                    end
                    if x + 1 > xMax or y + 1 > yMax or z + 1 > zMax or indexMap[x + 1][y + 1][z + 1] == 0 then
                        normals[index][1] = normals[index][1] - length
                        normals[index][2] = normals[index][2] - length
                        normals[index][3] = normals[index][3] - length
                    end

                    local length2 = 1 / 1.414;
                    if y - 1 < 1 or z - 1 < 1 or indexMap[x][y - 1][z - 1] == 0 then
                        normals[index][2] = normals[index][2] + length2
                        normals[index][3] = normals[index][3] + length2
                    end
                    if y + 1 > yMax or z - 1 < 1 or indexMap[x][y + 1][z - 1] == 0 then
                        normals[index][2] = normals[index][2] - length2
                        normals[index][3] = normals[index][3] + length2
                    end
                    if y - 1 < 1 or z + 1 > zMax or indexMap[x][y - 1][z + 1] == 0 then
                        normals[index][2] = normals[index][2] + length2
                        normals[index][3] = normals[index][3] - length2
                    end
                    if y + 1 > yMax or z + 1 > zMax or indexMap[x][y + 1][z + 1] == 0 then
                        normals[index][2] = normals[index][2] - length2
                        normals[index][3] = normals[index][3] - length2
                    end

                    if x - 1 < 1 or z - 1 < 1 or indexMap[x - 1][y][z - 1] == 0 then
                        normals[index][1] = normals[index][1] + length2
                        normals[index][3] = normals[index][3] + length2
                    end
                    if x - 1 < 1 or z + 1 > zMax or indexMap[x - 1][y][z + 1] == 0 then
                        normals[index][1] = normals[index][1] + length2
                        normals[index][3] = normals[index][3] - length2
                    end
                    if x + 1 > xMax or z - 1 < 1 or indexMap[x + 1][y][z - 1] == 0 then
                        normals[index][1] = normals[index][1] - length2
                        normals[index][3] = normals[index][3] + length2
                    end
                    if x + 1 > xMax or z + 1 > zMax or indexMap[x + 1][y][z + 1] == 0 then
                        normals[index][1] = normals[index][1] - length2
                        normals[index][3] = normals[index][3] - length2
                    end

                    if x - 1 < 1 or y - 1 < 1 or indexMap[x - 1][y - 1][z] == 0 then
                        normals[index][1] = normals[index][1] + length2
                        normals[index][2] = normals[index][2] + length2
                    end
                    if x - 1 < 1 or y + 1 > yMax or indexMap[x - 1][y + 1][z] == 0 then
                        normals[index][1] = normals[index][1] + length2
                        normals[index][2] = normals[index][2] - length2
                    end
                    if x + 1 > xMax or y - 1 < 1 or indexMap[x + 1][y - 1][z] == 0 then
                        normals[index][1] = normals[index][1] - length2
                        normals[index][2] = normals[index][2] + length2
                    end
                    if x + 1 > xMax or y + 1 > yMax or indexMap[x + 1][y + 1][z] == 0 then
                        normals[index][1] = normals[index][1] - length2
                        normals[index][2] = normals[index][2] - length2
                    end
                end
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

local cameraPosition = { 0, 0, 90 }
local targetPosition = { 0, 0, 0 }
local a = 0;

function gameState.Update()
    print("Lua Update")
    a = a + 0.001
    local distance = gameMath.PingPong(100, 200, a)
    cameraPosition[1] = -distance * math.sin(a)
    cameraPosition[2] = distance * math.cos(a)

    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
