table.empty = {}

local gameState = {
    pGraphicEngine = nil,
    assetsPath = "",
    pathSeparator = "",
}

local integer = 0
---Add model to geometry subpass
---@param vertices table
---@param colors table
---@return integer
function gameState.AddModel(vertices, colors, normals)
    return integer
end

---Add model to geometry subpass
---@param index integer
function gameState.RemoveModel(index)
end

---Update model unifrom buffer
---@param index integer
---@param modelMatrix table
function gameState.UpdateModel(index, modelMatrix)
end

---SetCamera
---@param cameraPosition table
---@param targetPosition table
function gameState.SetCamera(cameraPosition, targetPosition)
end

local model = {
    vertexCount = 0,
    propertyCount = 0,
    names = {},
    types = {},
    indexToProperties = {},
}
---Deserialize .ply model
---@param path string
function gameState.LoadModel(path)
    return model
end

function gameState.DrawModel(px, py, pz, model)
    local vertices = {}
    local colors = {}
    local normals = {}
    local indexMap = {}
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
            elseif propertyName == "r" then
                colors[i][1] = model.indexToProperties[j][i] / 255.0
            elseif propertyName == "g" then
                colors[i][2] = model.indexToProperties[j][i] / 255.0
            elseif propertyName == "b" then
                colors[i][3] = model.indexToProperties[j][i] / 255.0
            else
                -- do nothing
            end
        end
        -- print("r:" ..
        -- model.indexToProperties[3][i] ..
        -- "g:" ..
        -- model.indexToProperties[4][i] .. "b:" .. model.indexToProperties[5][i] .. "a:" .. model.indexToProperties[6][i])
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
    -- local scale = 0.015625
    -- local scale = 1
    local scale = 0.03125
    local model = {
        { scale, 0,     0,     px },
        { 0,     scale, 0,     py },
        { 0,     0,     scale, pz },
        { 0,     0,     0,     1 },
    }
    gameState.UpdateModel(index, model)
end

return gameState
