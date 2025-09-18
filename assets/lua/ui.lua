require("gfx")
local uiRenderPass = require("uiRenderPass")

local ui = {}

function ui.isNodeVisible(node)
    return node.rect ~= nil and node.pMaterial ~= nil
end

function ui.createMeshPtr(pGfxContext, node)
    local rect = node.rect
    local vertices = {
        position = {rect.x, rect.y, rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + rect.height, rect.x, rect.y + rect.height},
        uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
        color = {rect.color, rect.color, rect.color, rect.color},
    }
    local indices = {0, 1, 2, 2, 3, 0}

    return gfx.createMeshPtrWithData(pGfxContext, ui.pUIVertexInputLayout, ui.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
end

function ui.setup(pGfxContext, pSwapchainAttachment, assetsPath)
    ui.uiVertexFormat = {{
        name = "position",
        type = TYPE_FLOAT,
        count = 2,
    }, {
        name = "uv",
        type = TYPE_FLOAT,
        count = 2,
    }, {
        name = "color",
        type = TYPE_UINT32,
        count = 1,
    }}
    ui.pUIVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, ui.uiVertexFormat)
    uiRenderPass.setup(pGfxContext, pSwapchainAttachment, assetsPath, ui.pUIVertexInputLayout)

    ui.rootNode = {
        name = "root",
        children = {},
    }
    ui.nodePool = {}
    ui.dirty = false
end

function ui.teardown(pGfxContext)
    ui.removeNode(ui.rootNode)

    ui.dirty = nil
    ui.nodePool = nil
    ui.rootNode = nil

    gfx.destroyVertexInputLayoutPtr(pGfxContext, ui.pUIVertexInputLayout)
    ui.uiVertexFormat = nil
end

function ui.addNode(pGfxContext, name, pMaterial, rect, parent, index)
    local node = nil
    if parent then
        if #ui.nodePool > 0 then
            node = table.remove(ui.nodePool)
            node.name = name
            node.parent = parent
            node.pMaterial = pMaterial
            node.rect = rect
            assert(#node.children == 0, "ui.addNode: node from pool has children")
        else
            node = {
                name = name,
                children = {},
                parent = parent,
                pMaterial = pMaterial,
                rect = rect,
            }
        end
        table.insert(parent.children, index, node)
        if ui.isNodeVisible(node) then
            local drawCallIndex = 0
            for index, child in ipairs(ui.rootNode.children) do
                if ui.isNodeVisible(child) then
                    drawCallIndex = drawCallIndex + 1
                    if child == node then
                        break
                    end
                end
            end
            local pMesh = ui.createMeshPtr(node.rect)
            node.drawCall = gfx.addDrawCallPtr(pGfxContext, uiRenderPass.pUIPipeline, node.pMaterial, pMesh, 0, drawCallIndex)
        end

    end

    return node
end

function ui.removeNode(pGfxContext, node)
    for i = #node.children, 1, -1 do
        ui.removeNode(pGfxContext, node.children[i])
    end

    if node.drawCall then
        gfx.removeDrawCallPtr(pGfxContext, node.drawCall)
        node.drawCall = nil
    end
    
    if node.parent then
        for i, child in ipairs(node.parent.children) do
            if child == node then
                table.remove(node.parent.children, i)
                break
            end
        end
        node.parent = nil
    end

    node.name = nil
    node.children = {}
    node.rect = nil
    node.pMaterial = nil
    node.drawCall = nil
    table.insert(ui.nodePool, node)
end

return ui
