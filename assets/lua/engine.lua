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
function engine.AddModelToOpaqueGeometrySubpass(vertices, colors, normals)
    return integer
end

---Add model to opaqueGeometry subpass
---@param index integer
function engine.RemoveModelFromOpaqueGeometrySubpass(index)
end

---Update model unifrom buffer
---@param modelIndex integer
---@param instances table
function engine.UpdateInstancesInOpaqueGeometrySubpass(modelIndex, instances)
end

---UpdateGlobalUniformBuffer
---@param cameraPosition table
---@param targetPosition table
---@param time number
function engine.UpdateGlobalUniformBuffer(cameraPosition, targetPosition, time)
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

function engine.SetNormals(vertices, normals, voxelMap)
    local vertexCount = #vertices
    for i = 1, vertexCount do
        local x = vertices[i][1]
        local y = vertices[i][2]
        local z = vertices[i][3]

        local centerValue = voxelMap[x][y][z]

        if voxelMap[x - 1][y][z] ~= nil and voxelMap[x - 1][y][z] == centerValue then
            normals[i][1] = normals[i][1] + 1
        end
        if voxelMap[x + 1][y][z] ~= nil and voxelMap[x + 1][y][z] == centerValue then
            normals[i][1] = normals[i][1] - 1
        end
        if voxelMap[x][y - 1][z] ~= nil and voxelMap[x][y - 1][z] == centerValue then
            normals[i][2] = normals[i][2] + 1
        end
        if voxelMap[x][y + 1][z] ~= nil and voxelMap[x][y + 1][z] == centerValue then
            normals[i][2] = normals[i][2] - 1
        end
        if voxelMap[x][y][z - 1] ~= nil and voxelMap[x][y][z - 1] == centerValue then
            normals[i][3] = normals[i][3] + 1
        end
        if voxelMap[x][y][z + 1] ~= nil and voxelMap[x][y][z + 1] == centerValue then
            normals[i][3] = normals[i][3] - 1
        end

        local length = 1 / 1.414 / 1.414
        if voxelMap[x - 1][y - 1][z - 1] ~= nil and voxelMap[x - 1][y - 1][z - 1] == centerValue then
            normals[i][1] = normals[i][1] + length
            normals[i][2] = normals[i][2] + length
            normals[i][3] = normals[i][3] + length
        end
        if voxelMap[x - 1][y + 1][z - 1] ~= nil and voxelMap[x - 1][y + 1][z - 1] == centerValue then
            normals[i][1] = normals[i][1] + length
            normals[i][2] = normals[i][2] - length
            normals[i][3] = normals[i][3] + length
        end
        if voxelMap[x - 1][y - 1][z + 1] ~= nil and voxelMap[x - 1][y - 1][z + 1] == centerValue then
            normals[i][1] = normals[i][1] + length
            normals[i][2] = normals[i][2] + length
            normals[i][3] = normals[i][3] - length
        end
        if voxelMap[x - 1][y + 1][z + 1] ~= nil and voxelMap[x - 1][y + 1][z + 1] == centerValue then
            normals[i][1] = normals[i][1] + length
            normals[i][2] = normals[i][2] - length
            normals[i][3] = normals[i][3] - length
        end

        if voxelMap[x + 1][y - 1][z - 1] ~= nil and voxelMap[x + 1][y - 1][z - 1] == centerValue then
            normals[i][1] = normals[i][1] - length
            normals[i][2] = normals[i][2] + length
            normals[i][3] = normals[i][3] + length
        end
        if voxelMap[x + 1][y + 1][z - 1] ~= nil and voxelMap[x + 1][y + 1][z - 1] == centerValue then
            normals[i][1] = normals[i][1] - length
            normals[i][2] = normals[i][2] - length
            normals[i][3] = normals[i][3] + length
        end
        if voxelMap[x + 1][y - 1][z + 1] ~= nil and voxelMap[x + 1][y - 1][z + 1] == centerValue then
            normals[i][1] = normals[i][1] - length
            normals[i][2] = normals[i][2] + length
            normals[i][3] = normals[i][3] - length
        end
        if voxelMap[x + 1][y + 1][z + 1] ~= nil and voxelMap[x + 1][y + 1][z + 1] == centerValue then
            normals[i][1] = normals[i][1] - length
            normals[i][2] = normals[i][2] - length
            normals[i][3] = normals[i][3] - length
        end

        length = 1 / 1.414
        if voxelMap[x - 1][y - 1][z] ~= nil and voxelMap[x - 1][y - 1][z] == centerValue then
            normals[i][1] = normals[i][1] + length
            normals[i][2] = normals[i][2] + length
        end
        if voxelMap[x - 1][y + 1][z] ~= nil and voxelMap[x - 1][y + 1][z] == centerValue then
            normals[i][1] = normals[i][1] + length
            normals[i][2] = normals[i][2] - length
        end
        if voxelMap[x - 1][y][z - 1] ~= nil and voxelMap[x - 1][y][z - 1] == centerValue then
            normals[i][1] = normals[i][1] + length
            normals[i][3] = normals[i][3] + length
        end
        if voxelMap[x - 1][y][z + 1] ~= nil and voxelMap[x - 1][y][z + 1] == centerValue then
            normals[i][1] = normals[i][1] + length
            normals[i][3] = normals[i][3] - length
        end
        if voxelMap[x + 1][y - 1][z] ~= nil and voxelMap[x + 1][y - 1][z] == centerValue then
            normals[i][1] = normals[i][1] - length
            normals[i][2] = normals[i][2] + length
        end
        if voxelMap[x + 1][y + 1][z] ~= nil and voxelMap[x + 1][y + 1][z] == centerValue then
            normals[i][1] = normals[i][1] - length
            normals[i][2] = normals[i][2] - length
        end
        if voxelMap[x + 1][y][z - 1] ~= nil and voxelMap[x + 1][y][z - 1] == centerValue then
            normals[i][1] = normals[i][1] - length
            normals[i][3] = normals[i][3] + length
        end
        if voxelMap[x + 1][y][z + 1] ~= nil and voxelMap[x + 1][y][z + 1] == centerValue then
            normals[i][1] = normals[i][1] - length
            normals[i][3] = normals[i][3] - length
        end
        if voxelMap[x][y - 1][z - 1] ~= nil and voxelMap[x][y - 1][z - 1] == centerValue then
            normals[i][2] = normals[i][2] + length
            normals[i][3] = normals[i][3] + length
        end
        if voxelMap[x][y - 1][z + 1] ~= nil and voxelMap[x][y - 1][z + 1] == centerValue then
            normals[i][2] = normals[i][2] + length
            normals[i][3] = normals[i][3] - length
        end
        if voxelMap[x][y + 1][z - 1] ~= nil and voxelMap[x][y + 1][z - 1] == centerValue then
            normals[i][2] = normals[i][2] - length
            normals[i][3] = normals[i][3] + length
        end
        if voxelMap[x][y + 1][z + 1] ~= nil and voxelMap[x][y + 1][z + 1] == centerValue then
            normals[i][2] = normals[i][2] - length
            normals[i][3] = normals[i][3] - length
        end

        local nx = normals[i][1]
        local ny = normals[i][2]
        local nz = normals[i][3]
        local len = math.sqrt(nx * nx + ny * ny + nz * nz)
        if len > 0.001 then
            normals[i][1] = nx / len
            normals[i][2] = ny / len
            normals[i][3] = nz / len
        else
            normals[i][1] = 0
            normals[i][2] = 0
            normals[i][3] = 0
        end
    end
end

-- function engine.SetNormals(vertices, normals, voxelMap)
--     local vertexCount = #vertices
--     for i = 1, vertexCount do
--         local x = vertices[i][1]
--         local y = vertices[i][2]
--         local z = vertices[i][3]
--         if voxelMap[x - 1][y][z] ~= nil then
--             normals[i][1] = normals[i][1] + 1;
--         end
--         if voxelMap[x + 1][y][z] ~= nil then
--             normals[i][1] = normals[i][1] - 1;
--         end
--         if voxelMap[x][y - 1][z] ~= nil then
--             normals[i][2] = normals[i][2] + 1;
--         end
--         if voxelMap[x][y + 1][z] ~= nil then
--             normals[i][2] = normals[i][2] - 1;
--         end
--         if voxelMap[x][y][z - 1] ~= nil then
--             normals[i][3] = normals[i][3] + 1;
--         end
--         if voxelMap[x][y][z + 1] ~= nil then
--             normals[i][3] = normals[i][3] - 1;
--         end

--         local length = 1 / 1.414 / 1.414
--         if voxelMap[x - 1][y - 1][z - 1] ~= nil then
--             normals[i][1] = normals[i][1] + length;
--             normals[i][2] = normals[i][2] + length;
--             normals[i][3] = normals[i][3] + length;
--         end
--         if voxelMap[x - 1][y + 1][z - 1] ~= nil then
--             normals[i][1] = normals[i][1] + length;
--             normals[i][2] = normals[i][2] - length;
--             normals[i][3] = normals[i][3] + length;
--         end
--         if voxelMap[x - 1][y - 1][z + 1] ~= nil then
--             normals[i][1] = normals[i][1] + length;
--             normals[i][2] = normals[i][2] + length;
--             normals[i][3] = normals[i][3] - length;
--         end
--         if voxelMap[x - 1][y + 1][z + 1] ~= nil then
--             normals[i][1] = normals[i][1] + length;
--             normals[i][2] = normals[i][2] - length;
--             normals[i][3] = normals[i][3] - length;
--         end

--         if voxelMap[x + 1][y - 1][z - 1] ~= nil then
--             normals[i][1] = normals[i][1] - length;
--             normals[i][2] = normals[i][2] + length;
--             normals[i][3] = normals[i][3] + length;
--         end
--         if voxelMap[x + 1][y + 1][z - 1] ~= nil then
--             normals[i][1] = normals[i][1] - length;
--             normals[i][2] = normals[i][2] - length;
--             normals[i][3] = normals[i][3] + length;
--         end
--         if voxelMap[x + 1][y - 1][z + 1] ~= nil then
--             normals[i][1] = normals[i][1] - length;
--             normals[i][2] = normals[i][2] + length;
--             normals[i][3] = normals[i][3] - length;
--         end
--         if voxelMap[x + 1][y + 1][z + 1] ~= nil then
--             normals[i][1] = normals[i][1] - length;
--             normals[i][2] = normals[i][2] - length;
--             normals[i][3] = normals[i][3] - length;
--         end

--         length = 1 / 1.414;
--         if voxelMap[x - 1][y - 1][z] ~= nil then
--             normals[i][1] = normals[i][1] + length;
--             normals[i][2] = normals[i][2] + length;
--         end
--         if voxelMap[x - 1][y + 1][z] ~= nil then
--             normals[i][1] = normals[i][1] + length;
--             normals[i][2] = normals[i][2] - length;
--         end
--         if voxelMap[x - 1][y][z - 1] ~= nil then
--             normals[i][1] = normals[i][1] + length;
--             normals[i][3] = normals[i][3] + length;
--         end
--         if voxelMap[x - 1][y][z + 1] ~= nil then
--             normals[i][1] = normals[i][1] + length;
--             normals[i][3] = normals[i][3] - length;
--         end
--         if voxelMap[x + 1][y - 1][z] ~= nil then
--             normals[i][1] = normals[i][1] - length;
--             normals[i][2] = normals[i][2] + length;
--         end
--         if voxelMap[x + 1][y + 1][z] ~= nil then
--             normals[i][1] = normals[i][1] - length;
--             normals[i][2] = normals[i][2] - length;
--         end
--         if voxelMap[x + 1][y][z - 1] ~= nil then
--             normals[i][1] = normals[i][1] - length;
--             normals[i][3] = normals[i][3] + length;
--         end
--         if voxelMap[x + 1][y][z + 1] ~= nil then
--             normals[i][1] = normals[i][1] - length;
--             normals[i][3] = normals[i][3] - length;
--         end
--         if voxelMap[x][y - 1][z - 1] ~= nil then
--             normals[i][2] = normals[i][2] + length;
--             normals[i][3] = normals[i][3] + length;
--         end
--         if voxelMap[x][y - 1][z + 1] ~= nil then
--             normals[i][2] = normals[i][2] + length;
--             normals[i][3] = normals[i][3] - length;
--         end
--         if voxelMap[x][y + 1][z - 1] ~= nil then
--             normals[i][2] = normals[i][2] - length;
--             normals[i][3] = normals[i][3] + length;
--         end
--         if voxelMap[x][y + 1][z + 1] ~= nil then
--             normals[i][2] = normals[i][2] - length;
--             normals[i][3] = normals[i][3] - length;
--         end

--         local nx = normals[i][1]
--         local ny = normals[i][2]
--         local nz = normals[i][3]
--         local len = math.sqrt(nx * nx + ny * ny + nz * nz)
--         if len > 0.001 then
--             normals[i][1] = nx / len
--             normals[i][2] = ny / len
--             normals[i][3] = nz / len
--         else
--             normals[i][1] = 0
--             normals[i][2] = 0
--             normals[i][3] = 0
--         end
--     end
-- end

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
            if propertyName == "px" then
                vertices[i][1] = model.indexToProperties[j][i]
            elseif propertyName == "py" then
                vertices[i][2] = model.indexToProperties[j][i]
            elseif propertyName == "pz" then
                vertices[i][3] = model.indexToProperties[j][i]
            elseif propertyName == "r" then
                colors[i][1] = model.indexToProperties[j][i]
            elseif propertyName == "g" then
                colors[i][2] = model.indexToProperties[j][i]
            elseif propertyName == "b" then
                colors[i][3] = model.indexToProperties[j][i]
            elseif propertyName == "nx" then
                normals[i][1] = model.indexToProperties[j][i]
            elseif propertyName == "ny" then
                normals[i][2] = model.indexToProperties[j][i]
            elseif propertyName == "nz" then
                normals[i][3] = model.indexToProperties[j][i]
            else
                -- do nothing
            end
        end
    end
    local modelIndex = engine.AddModelToOpaqueGeometrySubpass(vertices, colors, normals)
    engine.UpdateInstancesInOpaqueGeometrySubpass(modelIndex, instances)
    return modelIndex
end

return engine
