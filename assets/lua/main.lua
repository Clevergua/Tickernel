local gameState = require("gameState")
local gameMath = require("gameMath")

function gameState.DrawModel(px, py, pz, modelName)
    local vertices = {}
    local colors = {}
    local normals = {}
    local indexMap = {}
    local model = gameState.LoadModel(gameState.assetsPath ..
        gameState.pathSeparator .. "models" .. gameState.pathSeparator .. modelName)
    for i = 1, model.vertexCount do
        vertices[i] = {}
        colors[i] = {}
        normals[i] = {}
        for j = 1, model.propertyCount do
            local propertyName = model.names[j]
            colors[i][4] = 0
            normals[i] = { 0, 0, 0 }
            if propertyName == "x" then
                vertices[i][1] = model.indexToProperties[j][i]
            elseif propertyName == "y" then
                vertices[i][2] = model.indexToProperties[j][i]
            elseif propertyName == "z" then
                vertices[i][3] = model.indexToProperties[j][i]
            elseif propertyName == "red" then
                colors[i][1] = model.indexToProperties[j][i] / 255.0
            elseif propertyName == "green" then
                colors[i][2] = model.indexToProperties[j][i] / 255.0
            elseif propertyName == "blue" then
                colors[i][3] = model.indexToProperties[j][i] / 255.0
            else
                -- do nothing
            end
        end
    end

    for i = 1, model.vertexCount do
        local x = vertices[i][1]
        local y = vertices[i][2]
        local z = vertices[i][3]
        if indexMap[x] == nil then
            indexMap[x] = {}
        end
        if indexMap[x][y] == nil then
            indexMap[x][y] = {}
        end
        indexMap[x][y][z] = i
    end

    for i = 1, model.vertexCount do
        local x = vertices[i][1]
        local y = vertices[i][2]
        local z = vertices[i][3]
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y] ~= nil then
                if indexMap[x - 1][y][z] ~= nil then
                    normals[i][1] = normals[i][1] + 1;
                end
            end
        end
        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y] ~= nil then
                if indexMap[x + 1][y][z] ~= nil then
                    normals[i][1] = normals[i][1] - 1;
                end
            end
        end
        if indexMap[x] ~= nil then
            if indexMap[x][y - 1] ~= nil then
                if indexMap[x][y - 1][z] ~= nil then
                    normals[i][2] = normals[i][2] + 1;
                end
            end
        end
        if indexMap[x] ~= nil then
            if indexMap[x][y + 1] ~= nil then
                if indexMap[x][y + 1][z] ~= nil then
                    normals[i][2] = normals[i][2] - 1;
                end
            end
        end

        if indexMap[x] ~= nil then
            if indexMap[x][y] ~= nil then
                if indexMap[x][y][z - 1] ~= nil then
                    normals[i][3] = normals[i][3] + 1;
                end
            end
        end
        if indexMap[x] ~= nil then
            if indexMap[x][y] ~= nil then
                if indexMap[x][y][z + 1] ~= nil then
                    normals[i][3] = normals[i][3] - 1;
                end
            end
        end

        local length = 1 / 1.414 / 1.414
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y - 1] ~= nil then
                if indexMap[x - 1][y - 1][z - 1] ~= nil then
                    normals[i][1] = normals[i][1] + length;
                    normals[i][2] = normals[i][2] + length;
                    normals[i][3] = normals[i][3] + length;
                end
            end
        end
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y + 1] ~= nil then
                if indexMap[x - 1][y + 1][z - 1] ~= nil then
                    normals[i][1] = normals[i][1] + length;
                    normals[i][2] = normals[i][2] - length;
                    normals[i][3] = normals[i][3] + length;
                end
            end
        end
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y - 1] ~= nil then
                if indexMap[x - 1][y - 1][z + 1] ~= nil then
                    normals[i][1] = normals[i][1] + length;
                    normals[i][2] = normals[i][2] + length;
                    normals[i][3] = normals[i][3] - length;
                end
            end
        end
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y + 1] ~= nil then
                if indexMap[x - 1][y + 1][z + 1] ~= nil then
                    normals[i][1] = normals[i][1] + length;
                    normals[i][2] = normals[i][2] - length;
                    normals[i][3] = normals[i][3] - length;
                end
            end
        end

        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y - 1] ~= nil then
                if indexMap[x + 1][y - 1][z - 1] ~= nil then
                    normals[i][1] = normals[i][1] - length;
                    normals[i][2] = normals[i][2] + length;
                    normals[i][3] = normals[i][3] + length;
                end
            end
        end
        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y + 1] ~= nil then
                if indexMap[x + 1][y + 1][z - 1] ~= nil then
                    normals[i][1] = normals[i][1] - length;
                    normals[i][2] = normals[i][2] - length;
                    normals[i][3] = normals[i][3] + length;
                end
            end
        end
        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y - 1] ~= nil then
                if indexMap[x + 1][y - 1][z + 1] ~= nil then
                    normals[i][1] = normals[i][1] - length;
                    normals[i][2] = normals[i][2] + length;
                    normals[i][3] = normals[i][3] - length;
                end
            end
        end
        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y + 1] ~= nil then
                if indexMap[x + 1][y + 1][z + 1] ~= nil then
                    normals[i][1] = normals[i][1] - length;
                    normals[i][2] = normals[i][2] - length;
                    normals[i][3] = normals[i][3] - length;
                end
            end
        end

        length = 1 / 1.414;
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y - 1] ~= nil then
                if indexMap[x - 1][y - 1][z] ~= nil then
                    normals[i][1] = normals[i][1] + length;
                    normals[i][2] = normals[i][2] + length;
                end
            end
        end
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y + 1] ~= nil then
                if indexMap[x - 1][y + 1][z] ~= nil then
                    normals[i][1] = normals[i][1] + length;
                    normals[i][2] = normals[i][2] - length;
                end
            end
        end
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y] ~= nil then
                if indexMap[x - 1][y][z - 1] ~= nil then
                    normals[i][1] = normals[i][1] + length;
                    normals[i][3] = normals[i][3] + length;
                end
            end
        end
        if indexMap[x - 1] ~= nil then
            if indexMap[x - 1][y] ~= nil then
                if indexMap[x - 1][y][z + 1] ~= nil then
                    normals[i][1] = normals[i][1] + length;
                    normals[i][3] = normals[i][3] - length;
                end
            end
        end
        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y - 1] ~= nil then
                if indexMap[x + 1][y - 1][z] ~= nil then
                    normals[i][1] = normals[i][1] - length;
                    normals[i][2] = normals[i][2] + length;
                end
            end
        end
        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y + 1] ~= nil then
                if indexMap[x + 1][y + 1][z] ~= nil then
                    normals[i][1] = normals[i][1] - length;
                    normals[i][2] = normals[i][2] - length;
                end
            end
        end
        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y] ~= nil then
                if indexMap[x + 1][y][z - 1] ~= nil then
                    normals[i][1] = normals[i][1] - length;
                    normals[i][3] = normals[i][3] + length;
                end
            end
        end
        if indexMap[x + 1] ~= nil then
            if indexMap[x + 1][y] ~= nil then
                if indexMap[x + 1][y][z + 1] ~= nil then
                    normals[i][1] = normals[i][1] - length;
                    normals[i][3] = normals[i][3] - length;
                end
            end
        end
        if indexMap[x] ~= nil then
            if indexMap[x][y - 1] ~= nil then
                if indexMap[x][y - 1][z - 1] ~= nil then
                    normals[i][2] = normals[i][2] + length;
                    normals[i][3] = normals[i][3] + length;
                end
            end
        end
        if indexMap[x] ~= nil then
            if indexMap[x][y - 1] ~= nil then
                if indexMap[x][y - 1][z + 1] ~= nil then
                    normals[i][2] = normals[i][2] + length;
                    normals[i][3] = normals[i][3] - length;
                end
            end
        end
        if indexMap[x] ~= nil then
            if indexMap[x][y + 1] ~= nil then
                if indexMap[x][y + 1][z - 1] ~= nil then
                    normals[i][2] = normals[i][2] - length;
                    normals[i][3] = normals[i][3] + length;
                end
            end
        end
        if indexMap[x] ~= nil then
            if indexMap[x][y + 1] ~= nil then
                if indexMap[x][y + 1][z + 1] ~= nil then
                    normals[i][2] = normals[i][2] - length;
                    normals[i][3] = normals[i][3] - length;
                end
            end
        end
    end

    local index = gameState.AddModel(vertices, colors, normals)
    local model = {
        { 1,  0,  0,  0 },
        { 0,  1,  0,  0 },
        { 0,  0,  1,  0 },
        { px, py, pz, 1 },
    }
    gameState.UpdateModel(index, model)
end

function gameState.Start()
    print("Lua Start")
    gameState.DrawModel(-30, -30, 0, "LargeBuilding01.ply")
    gameState.DrawModel(30, 30, 0, "LargeBuilding01.ply")
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 100 }
local targetPosition = { 0, 0, 0 }
local t = 0;

function gameState.Update()
    print("Lua Update")
    t = t + 0.001
    local distance = gameMath.PingPong(100, 150, t)
    cameraPosition[1] = -distance * math.sin(t)
    cameraPosition[2] = distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
