table.empty = {};

local gameState = {
    pGraphicEngine = nil,
};

local integer = 0;
---Add model to geometry subpass
---@param vertices table
---@param colors table
---@return integer
function gameState.AddModel(vertices, colors)
    return integer;
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

return gameState;
