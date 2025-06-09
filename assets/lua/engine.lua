require("vulkan")
table.empty = {}

local engine = {
    assetsPath = "",
    frameCount = 0,
    keyCodes = nil,
    lastFrameInput = {},
    input = {},
}

engine.keyCodeState = {
    IDLE = 0,
    DOWN = 1,
    UP = 2,
}

engine.keyCodes = {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7,
    I = 8,
    J = 9,
    K = 10,
    L = 11,
    M = 12,
    N = 13,
    O = 14,
    P = 15,
    Q = 16,
    R = 17,
    S = 18,
    T = 19,
    U = 20,
    V = 21,
    W = 22,
    X = 23,
    Y = 24,
    Z = 25,
    Num0 = 26,
    Num1 = 27,
    Num2 = 28,
    Num3 = 29,
    Num4 = 30,
    Num5 = 31,
    Num6 = 32,
    Num7 = 33,
    Num8 = 34,
    Num9 = 35,
    Enter = 36,
    Escape = 37,
    Backspace = 38,
    Tab = 39,
    Space = 40,
    Minus = 41,
    Equal = 42,
    LeftBracket = 43,
    RightBracket = 44,
    Backslash = 45,
    Semicolon = 46,
    Apostrophe = 47,
    Grave = 48,
    Comma = 49,
    Period = 50,
    Slash = 51,
    CapsLock = 52,
    F1 = 53,
    F2 = 54,
    F3 = 55,
    F4 = 56,
    F5 = 57,
    F6 = 58,
    F7 = 59,
    F8 = 60,
    F9 = 61,
    F10 = 62,
    F11 = 63,
    F12 = 64,
    PrintScreen = 65,
    ScrollLock = 66,
    Pause = 67,
    Insert = 68,
    Home = 69,
    PageUp = 70,
    Delete = 71,
    End = 72,
    PageDown = 73,
    Right = 74,
    Left = 75,
    Down = 76,
    Up = 77,
    NumLock = 78,
    Numpad0 = 79,
    Numpad1 = 80,
    Numpad2 = 81,
    Numpad3 = 82,
    Numpad4 = 83,
    Numpad5 = 84,
    Numpad6 = 85,
    Numpad7 = 86,
    Numpad8 = 87,
    Numpad9 = 88,
    NumpadDivide = 89,
    NumpadMultiply = 90,
    NumpadSubtract = 91,
    NumpadAdd = 92,
    NumpadEnter = 93,
    NumpadDecimal = 94,
    MaxEnum = 95,
}


-- VkAttachmentDescription = {
--     format = VkFormat,
--     samples = VkSampleCountFlagBits,
--     loadOp = VkAttachmentLoadOp,
--     storeOp = VkAttachmentStoreOp,
--     stencilLoadOp = VkAttachmentLoadOp,
--     stencilStoreOp = VkAttachmentStoreOp,
--     initialLayout = VkImageLayout,
--     finalLayout = VkImageLayout,
-- }

-- VkAttachmentReference = {
--     attachment = 0,
--     layout = VkImageLayout,
-- }
-- VkSubpassDescription = {
--     pipelineBindPoint = VkPipelineBindPoint,
--     inputAttachmentCount = 0,
--     pInputAttachments = { VkAttachmentReference },
--     colorAttachmentCount = 0,
--     pColorAttachments = { VkAttachmentReference },
--     pResolveAttachments = { VkAttachmentReference },
--     pDepthStencilAttachment = { VkAttachmentReference },
--     preserveAttachmentCount = 0,
--     pPreserveAttachments = { 0 },
-- }

-- VkSubpassDependency = {
--     srcSubpass = 0,
--     dstSubpass = 0,
--     srcStageMask = VkPipelineStageFlagBits,
--     dstStageMask = VkPipelineStageFlagBits,
--     srcAccessMask = VkAccessFlagBits,
--     dstAccessMask = VkAccessFlagBits,
--     dependencyFlags = VkDependencyFlagBits,
-- }

function engine.createRenderPass(vkAttachmentDescriptions, pAttachments, vkSubpassDescriptions, vkSubpassDependencies,
                                 renderPassIndex)
    local pRenderPass = 0
    return pRenderPass
end

function engine.destroyRenderPass(pRenderPass)
    return
end

function engine.setNormals(vertices, normals, voxelMap)
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

        -- local length = 1 / 1.414 / 1.414
        -- if voxelMap[x - 1][y - 1][z - 1] ~= nil and voxelMap[x - 1][y - 1][z - 1] == centerValue then
        --     normals[i][1] = normals[i][1] + length
        --     normals[i][2] = normals[i][2] + length
        --     normals[i][3] = normals[i][3] + length
        -- end
        -- if voxelMap[x - 1][y + 1][z - 1] ~= nil and voxelMap[x - 1][y + 1][z - 1] == centerValue then
        --     normals[i][1] = normals[i][1] + length
        --     normals[i][2] = normals[i][2] - length
        --     normals[i][3] = normals[i][3] + length
        -- end
        -- if voxelMap[x - 1][y - 1][z + 1] ~= nil and voxelMap[x - 1][y - 1][z + 1] == centerValue then
        --     normals[i][1] = normals[i][1] + length
        --     normals[i][2] = normals[i][2] + length
        --     normals[i][3] = normals[i][3] - length
        -- end
        -- if voxelMap[x - 1][y + 1][z + 1] ~= nil and voxelMap[x - 1][y + 1][z + 1] == centerValue then
        --     normals[i][1] = normals[i][1] + length
        --     normals[i][2] = normals[i][2] - length
        --     normals[i][3] = normals[i][3] - length
        -- end

        -- if voxelMap[x + 1][y - 1][z - 1] ~= nil and voxelMap[x + 1][y - 1][z - 1] == centerValue then
        --     normals[i][1] = normals[i][1] - length
        --     normals[i][2] = normals[i][2] + length
        --     normals[i][3] = normals[i][3] + length
        -- end
        -- if voxelMap[x + 1][y + 1][z - 1] ~= nil and voxelMap[x + 1][y + 1][z - 1] == centerValue then
        --     normals[i][1] = normals[i][1] - length
        --     normals[i][2] = normals[i][2] - length
        --     normals[i][3] = normals[i][3] + length
        -- end
        -- if voxelMap[x + 1][y - 1][z + 1] ~= nil and voxelMap[x + 1][y - 1][z + 1] == centerValue then
        --     normals[i][1] = normals[i][1] - length
        --     normals[i][2] = normals[i][2] + length
        --     normals[i][3] = normals[i][3] - length
        -- end
        -- if voxelMap[x + 1][y + 1][z + 1] ~= nil and voxelMap[x + 1][y + 1][z + 1] == centerValue then
        --     normals[i][1] = normals[i][1] - length
        --     normals[i][2] = normals[i][2] - length
        --     normals[i][3] = normals[i][3] - length
        -- end

        -- length = 1 / 1.414
        -- if voxelMap[x - 1][y - 1][z] ~= nil and voxelMap[x - 1][y - 1][z] == centerValue then
        --     normals[i][1] = normals[i][1] + length
        --     normals[i][2] = normals[i][2] + length
        -- end
        -- if voxelMap[x - 1][y + 1][z] ~= nil and voxelMap[x - 1][y + 1][z] == centerValue then
        --     normals[i][1] = normals[i][1] + length
        --     normals[i][2] = normals[i][2] - length
        -- end
        -- if voxelMap[x - 1][y][z - 1] ~= nil and voxelMap[x - 1][y][z - 1] == centerValue then
        --     normals[i][1] = normals[i][1] + length
        --     normals[i][3] = normals[i][3] + length
        -- end
        -- if voxelMap[x - 1][y][z + 1] ~= nil and voxelMap[x - 1][y][z + 1] == centerValue then
        --     normals[i][1] = normals[i][1] + length
        --     normals[i][3] = normals[i][3] - length
        -- end
        -- if voxelMap[x + 1][y - 1][z] ~= nil and voxelMap[x + 1][y - 1][z] == centerValue then
        --     normals[i][1] = normals[i][1] - length
        --     normals[i][2] = normals[i][2] + length
        -- end
        -- if voxelMap[x + 1][y + 1][z] ~= nil and voxelMap[x + 1][y + 1][z] == centerValue then
        --     normals[i][1] = normals[i][1] - length
        --     normals[i][2] = normals[i][2] - length
        -- end
        -- if voxelMap[x + 1][y][z - 1] ~= nil and voxelMap[x + 1][y][z - 1] == centerValue then
        --     normals[i][1] = normals[i][1] - length
        --     normals[i][3] = normals[i][3] + length
        -- end
        -- if voxelMap[x + 1][y][z + 1] ~= nil and voxelMap[x + 1][y][z + 1] == centerValue then
        --     normals[i][1] = normals[i][1] - length
        --     normals[i][3] = normals[i][3] - length
        -- end
        -- if voxelMap[x][y - 1][z - 1] ~= nil and voxelMap[x][y - 1][z - 1] == centerValue then
        --     normals[i][2] = normals[i][2] + length
        --     normals[i][3] = normals[i][3] + length
        -- end
        -- if voxelMap[x][y - 1][z + 1] ~= nil and voxelMap[x][y - 1][z + 1] == centerValue then
        --     normals[i][2] = normals[i][2] + length
        --     normals[i][3] = normals[i][3] - length
        -- end
        -- if voxelMap[x][y + 1][z - 1] ~= nil and voxelMap[x][y + 1][z - 1] == centerValue then
        --     normals[i][2] = normals[i][2] - length
        --     normals[i][3] = normals[i][3] + length
        -- end
        -- if voxelMap[x][y + 1][z + 1] ~= nil and voxelMap[x][y + 1][z + 1] == centerValue then
        --     normals[i][2] = normals[i][2] - length
        --     normals[i][3] = normals[i][3] - length
        -- end

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
            normals[i][3] = 1
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

function engine.drawModel(instances, model)
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
    local model = engine.addModelToGeometrySubpass(vertices, colors, normals)
    engine.updateInstancesInGeometrySubpass(model, instances)
    return model
end

function engine.createFixedAttachment(vkExtent3D, vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, width, height)
    local pAttachment
    return pAttachment
end
function engine.destroyFixedAttachment(pAttachment)
end

function engine.createDynamicAttachment(vkExtent3D, vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, scaler)
    local pAttachment
    return pAttachment
end
function engine.destroyDynamicAttachment(pAttachment)
end



return engine
