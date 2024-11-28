table.empty = {}

local engine = {
    pGraphicEngine = nil,
    assetsPath = "",
    pathSeparator = "",
    frameCount = 0,
}

local integer = 0
---Add model to opaqueGeometry subpass
---@param vertices table
---@param colors table
---@return integer
function engine.AddModel(vertices, colors, normals)
    return integer
end

---Add model to opaqueGeometry subpass
---@param index integer
function engine.RemoveModel(index)
end

---Update model unifrom buffer
---@param modelIndex integer
---@param instances table
function engine.UpdateInstances(modelIndex, instances)
end

---UpdateGlobalUniformBuffer
---@param cameraPosition table
---@param targetPosition table
function engine.UpdateGlobalUniformBuffer(cameraPosition, targetPosition)
end

---UpdateGlobalUniformBuffer
---@param directionalLight table
---@param pointLights table
function engine.UpdateLightsUniformBuffer(directionalLight, pointLights)
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
function engine.LoadModel(path)
    return model
end

function engine.SetNormals(vertices, normals)
    local indexMap = {}
    local vertexCount = #vertices
    for i = 1, vertexCount do
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

    for i = 1, vertexCount do
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

        -- local length = 1 / 1.414 / 1.414
        -- if indexMap[x - 1] ~= nil then
        --     if indexMap[x - 1][y - 1] ~= nil then
        --         if indexMap[x - 1][y - 1][z - 1] ~= nil then
        --             normals[i][1] = normals[i][1] + length;
        --             normals[i][2] = normals[i][2] + length;
        --             normals[i][3] = normals[i][3] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x - 1] ~= nil then
        --     if indexMap[x - 1][y + 1] ~= nil then
        --         if indexMap[x - 1][y + 1][z - 1] ~= nil then
        --             normals[i][1] = normals[i][1] + length;
        --             normals[i][2] = normals[i][2] - length;
        --             normals[i][3] = normals[i][3] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x - 1] ~= nil then
        --     if indexMap[x - 1][y - 1] ~= nil then
        --         if indexMap[x - 1][y - 1][z + 1] ~= nil then
        --             normals[i][1] = normals[i][1] + length;
        --             normals[i][2] = normals[i][2] + length;
        --             normals[i][3] = normals[i][3] - length;
        --         end
        --     end
        -- end
        -- if indexMap[x - 1] ~= nil then
        --     if indexMap[x - 1][y + 1] ~= nil then
        --         if indexMap[x - 1][y + 1][z + 1] ~= nil then
        --             normals[i][1] = normals[i][1] + length;
        --             normals[i][2] = normals[i][2] - length;
        --             normals[i][3] = normals[i][3] - length;
        --         end
        --     end
        -- end

        -- if indexMap[x + 1] ~= nil then
        --     if indexMap[x + 1][y - 1] ~= nil then
        --         if indexMap[x + 1][y - 1][z - 1] ~= nil then
        --             normals[i][1] = normals[i][1] - length;
        --             normals[i][2] = normals[i][2] + length;
        --             normals[i][3] = normals[i][3] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x + 1] ~= nil then
        --     if indexMap[x + 1][y + 1] ~= nil then
        --         if indexMap[x + 1][y + 1][z - 1] ~= nil then
        --             normals[i][1] = normals[i][1] - length;
        --             normals[i][2] = normals[i][2] - length;
        --             normals[i][3] = normals[i][3] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x + 1] ~= nil then
        --     if indexMap[x + 1][y - 1] ~= nil then
        --         if indexMap[x + 1][y - 1][z + 1] ~= nil then
        --             normals[i][1] = normals[i][1] - length;
        --             normals[i][2] = normals[i][2] + length;
        --             normals[i][3] = normals[i][3] - length;
        --         end
        --     end
        -- end
        -- if indexMap[x + 1] ~= nil then
        --     if indexMap[x + 1][y + 1] ~= nil then
        --         if indexMap[x + 1][y + 1][z + 1] ~= nil then
        --             normals[i][1] = normals[i][1] - length;
        --             normals[i][2] = normals[i][2] - length;
        --             normals[i][3] = normals[i][3] - length;
        --         end
        --     end
        -- end

        -- length = 1 / 1.414;
        -- if indexMap[x - 1] ~= nil then
        --     if indexMap[x - 1][y - 1] ~= nil then
        --         if indexMap[x - 1][y - 1][z] ~= nil then
        --             normals[i][1] = normals[i][1] + length;
        --             normals[i][2] = normals[i][2] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x - 1] ~= nil then
        --     if indexMap[x - 1][y + 1] ~= nil then
        --         if indexMap[x - 1][y + 1][z] ~= nil then
        --             normals[i][1] = normals[i][1] + length;
        --             normals[i][2] = normals[i][2] - length;
        --         end
        --     end
        -- end
        -- if indexMap[x - 1] ~= nil then
        --     if indexMap[x - 1][y] ~= nil then
        --         if indexMap[x - 1][y][z - 1] ~= nil then
        --             normals[i][1] = normals[i][1] + length;
        --             normals[i][3] = normals[i][3] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x - 1] ~= nil then
        --     if indexMap[x - 1][y] ~= nil then
        --         if indexMap[x - 1][y][z + 1] ~= nil then
        --             normals[i][1] = normals[i][1] + length;
        --             normals[i][3] = normals[i][3] - length;
        --         end
        --     end
        -- end
        -- if indexMap[x + 1] ~= nil then
        --     if indexMap[x + 1][y - 1] ~= nil then
        --         if indexMap[x + 1][y - 1][z] ~= nil then
        --             normals[i][1] = normals[i][1] - length;
        --             normals[i][2] = normals[i][2] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x + 1] ~= nil then
        --     if indexMap[x + 1][y + 1] ~= nil then
        --         if indexMap[x + 1][y + 1][z] ~= nil then
        --             normals[i][1] = normals[i][1] - length;
        --             normals[i][2] = normals[i][2] - length;
        --         end
        --     end
        -- end
        -- if indexMap[x + 1] ~= nil then
        --     if indexMap[x + 1][y] ~= nil then
        --         if indexMap[x + 1][y][z - 1] ~= nil then
        --             normals[i][1] = normals[i][1] - length;
        --             normals[i][3] = normals[i][3] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x + 1] ~= nil then
        --     if indexMap[x + 1][y] ~= nil then
        --         if indexMap[x + 1][y][z + 1] ~= nil then
        --             normals[i][1] = normals[i][1] - length;
        --             normals[i][3] = normals[i][3] - length;
        --         end
        --     end
        -- end
        -- if indexMap[x] ~= nil then
        --     if indexMap[x][y - 1] ~= nil then
        --         if indexMap[x][y - 1][z - 1] ~= nil then
        --             normals[i][2] = normals[i][2] + length;
        --             normals[i][3] = normals[i][3] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x] ~= nil then
        --     if indexMap[x][y - 1] ~= nil then
        --         if indexMap[x][y - 1][z + 1] ~= nil then
        --             normals[i][2] = normals[i][2] + length;
        --             normals[i][3] = normals[i][3] - length;
        --         end
        --     end
        -- end
        -- if indexMap[x] ~= nil then
        --     if indexMap[x][y + 1] ~= nil then
        --         if indexMap[x][y + 1][z - 1] ~= nil then
        --             normals[i][2] = normals[i][2] - length;
        --             normals[i][3] = normals[i][3] + length;
        --         end
        --     end
        -- end
        -- if indexMap[x] ~= nil then
        --     if indexMap[x][y + 1] ~= nil then
        --         if indexMap[x][y + 1][z + 1] ~= nil then
        --             normals[i][2] = normals[i][2] - length;
        --             normals[i][3] = normals[i][3] - length;
        --         end
        --     end
        -- end
    end
end

function engine.DrawModel(instances, model)
    local vertices = {}
    local colors = {}
    local normals = {}
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
                colors[i][1] = model.indexToProperties[j][i]
            elseif propertyName == "g" then
                colors[i][2] = model.indexToProperties[j][i]
            elseif propertyName == "b" then
                colors[i][3] = model.indexToProperties[j][i]
            else
                -- do nothing
            end
        end
    end
    engine.SetNormals(vertices, normals)
    local modelIndex = engine.AddModel(vertices, colors, normals)
    engine.UpdateInstances(modelIndex, instances)
    return modelIndex
end

return engine
