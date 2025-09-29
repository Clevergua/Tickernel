require("gfx")
local uiRenderPass = require("uiRenderPass")

local ui = {
    fullScreenRect = {
        left = -1,
        right = 1,
        bottom = -1,
        top = 1,
    },
}

function ui.updateRect(pGfxContext, screenWidth, screenHeight, node, parentDirty)
    if node.layoutDirty or parentDirty then
        local parentRect = node == ui.rootNode and ui.fullScreenRect or node.parent.rect
        local layout = node.layout
        -- Handle horizontal layout
        if layout.horizontal.type == "anchored" then
            local anchor = layout.horizontal.anchor
            local pivot = layout.horizontal.pivot
            local width = layout.horizontal.width
            local x = parentRect.left + (parentRect.right - parentRect.left) * anchor - width * pivot
            node.rect.left = x
            node.rect.right = x + width / screenWidth * 2
        elseif layout.horizontal.type == "relative" then
            if math.type(layout.horizontal.left) == "integer" then
                node.rect.left = parentRect.left + layout.horizontal.left / screenWidth * 2
            else
                node.rect.left = parentRect.left + (parentRect.right - parentRect.left) * layout.horizontal.left
            end
            if math.type(layout.horizontal.right) == "integer" then
                node.rect.right = parentRect.right - layout.horizontal.right / screenWidth * 2
            else
                node.rect.right = parentRect.right - (parentRect.right - parentRect.left) * layout.horizontal.right
            end
        else
            error("ui.calculateRect: unknown horizontal layout type " .. tostring(layout.horizontal.type))
        end
        -- Handle vertical layout (assuming similar structure)
        if layout.vertical.type == "anchored" then
            local anchor = layout.vertical.anchor
            local pivot = layout.vertical.pivot
            local height = layout.vertical.height
            local y = parentRect.bottom + (parentRect.top - parentRect.bottom) * anchor - height * pivot
            node.rect.bottom = y
            node.rect.top = y + height / screenHeight * 2
        elseif layout.vertical.type == "relative" then
            if math.type(layout.vertical.bottom) == "integer" then
                node.rect.bottom = parentRect.bottom + layout.vertical.bottom / screenHeight * 2
            else
                node.rect.bottom = parentRect.bottom + (parentRect.top - parentRect.bottom) * layout.vertical.bottom
            end
            if math.type(layout.vertical.top) == "integer" then
                node.rect.top = parentRect.top - layout.vertical.top / screenHeight * 2
            else
                node.rect.top = parentRect.top - (parentRect.top - parentRect.bottom) * layout.vertical.top
            end
        else
            error("ui.calculateRect: unknown vertical layout type " .. tostring(layout.vertical.type))
        end

        if node.pMaterial then
            ui.updateMeshPtr(pGfxContext, node)
        end
        node.layoutDirty = false
        for i, child in ipairs(node.children) do
            ui.updateRect(pGfxContext, screenWidth, screenHeight, child, true)
        end
    else
        for i, child in ipairs(node.children) do
            ui.updateRect(pGfxContext, screenWidth, screenHeight, child, false)
        end
    end
end

function ui.createMeshPtr(pGfxContext, node)
    local vertices = {
        position = {0, 0, 0, 0, 0, 0, 0, 0},
        uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
        color = {node.color, node.color, node.color, node.color},
    }
    local indices = {0, 1, 2, 2, 3, 0}
    node.pMesh = gfx.createMeshPtrWithData(pGfxContext, ui.pUIVertexInputLayout, ui.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
end

function ui.destroyMeshPtr(pGfxContext, node)
    gfx.destroyMeshPtr(pGfxContext, node.pMesh)
    node.pMesh = nil
end

function ui.updateMeshPtr(pGfxContext, node)
    local rect = node.rect
    local vertices = {
        position = {rect.left, rect.bottom, rect.right, rect.bottom, rect.right, rect.top, rect.left, rect.top},
        uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
        color = {node.color, node.color, node.color, node.color},
    }
    local indices = {0, 1, 2, 2, 3, 0}
    gfx.updateMeshPtr(pGfxContext, node.pMesh, ui.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
end

function ui.setup(pGfxContext, pSwapchainAttachment, assetsPath)
    ui.pGfxContext = pGfxContext
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
        layout = {
            horizontal = {
                type = "relative",
                left = 0,
                right = 0,
            },
            vertical = {
                type = "relative",
                bottom = 0,
                top = 0,
            },
        },
        layoutDirty = true,
    }
    ui.nodePool = {}
end

function ui.teardown(pGfxContext)
    ui.removeNode(ui.rootNode)

    ui.nodePool = nil
    ui.rootNode = nil

    gfx.destroyVertexInputLayoutPtr(pGfxContext, ui.pUIVertexInputLayout)
    ui.uiVertexFormat = nil
end

function ui.update(pGfxContext, screenWidth, screenHeight)
    if ui.width ~= screenWidth or ui.height ~= screenHeight then
        ui.width = screenWidth
        ui.height = screenHeight
        ui.rootNode.layoutDirty = true
    end
    ui.updateRect(pGfxContext, screenWidth, screenHeight, ui.rootNode, false)
end

function ui.addNode(pGfxContext, parent, index, name, layout, pMaterial, color)
    if not parent and ui.rootNode then
        error("ui.rootNode is already set")
    else
        local node = nil
        if #ui.nodePool > 0 then
            node = table.remove(ui.nodePool)
            node.name = name
            assert(#node.children == 0, "ui.addNode: node from pool has children")
            node.parent = parent
            assert(node.rect ~= nil, "ui.addNode: node from pool has no rect")
            node.color = color
            node.pMaterial = pMaterial
            node.pMesh = nil
            node.pDrawCall = nil
            node.layout = layout
            node.layoutDirty = true
        else
            node = {
                name = name,
                children = {},
                parent = parent,
                rect = {},
                color = color,
                pMaterial = pMaterial,
                pMesh = nil,
                pDrawCall = nil,
                layout = layout,
                layoutDirty = true,
            }
        end
        if parent then
            table.insert(parent.children, index, node)
        else
            ui.rootNode = node
        end

        if pMaterial then
            ui.createMeshPtr(pGfxContext, node)
            node.pDrawCall = gfx.createDrawCallPtr(pGfxContext, uiRenderPass.pUIPipeline, node.pMaterial, node.pMesh, nil)
            if parent then
                local drawCallIndex = 0
                ui.traverseNode(ui.rootNode, function(child)
                    if child.pMaterial then
                        drawCallIndex = drawCallIndex + 1
                    end
                    if child == node then
                        return true
                    else
                        return false
                    end
                end)
                gfx.insertDrawCallPtr(node.pDrawCall, drawCallIndex)
            else
                gfx.insertDrawCallPtr(node.pDrawCall, 0)
            end
        end
        return node
    end

end

function ui.removeNode(pGfxContext, node)
    for i = #node.children, 1, -1 do
        ui.removeNode(pGfxContext, node.children[i])
    end

    if node.pDrawCall then
        gfx.removeDrawCallPtr(node.pDrawCall)
        gfx.destroyDrawCallPtr(pGfxContext, node.pDrawCall)
        node.pDrawCall = nil
    end

    ui.destroyMeshPtr(pGfxContext, node)

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
    node.color = nil
    node.pMaterial = nil
    node.pMesh = nil
    node.pDrawCall = nil
    node.layout = nil
    node.layoutDirty = false
    table.insert(ui.nodePool, node)
end

function ui.getNodeIndex(node)
    for i, child in ipairs(node.parent.children) do
        if child == node then
            return i
        end
    end
end

function ui.traverseNode(node, callback)
    local result = callback(node)
    if result then
        return result
    end
    for _, child in ipairs(node.children) do
        if ui.traverseNode(child, callback) then
            return true
        end
    end
    return false
end

function ui.moveNode(pGfxContext, node, parent, index)
    assert(node, "ui.moveNode: node is nil")
    assert(node ~= ui.rootNode, "ui.moveNode: cannot move root node")
    assert(parent, "ui.moveNode: parent is nil")

    if node.parent == parent and index == ui.getNodeIndex(node) then
        return true
    else
        assert(parent ~= node, "ui.moveNode: parent cannot be node itself")
        local current = parent
        while current do
            assert(current ~= node, "ui.moveNode: parent cannot be a descendant of node")
            current = current.parent
        end
    end

    local drawCalls = {}
    ui.traverseNode(node, function(child)
        if child.pMaterial then
            table.insert(drawCalls, child.pDrawCall)
        end
    end)

    if #drawCalls == 0 then
        local originalIndex = ui.getNodeIndex(node)
        table.remove(node.parent.children, originalIndex)
        table.insert(parent.children, index, node)
        node.parent = parent
        node.layoutDirty = true
        return true
    else
        local drawCallStartIndex = 0
        ui.traverseNode(ui.rootNode, function(child)
            if child == node then
                return true
            else
                if child.pMaterial then
                    drawCallStartIndex = drawCallStartIndex + 1
                end
                return false
            end
        end)

        for i = #drawCalls, 1, -1 do
            local removeIndex = drawCallStartIndex + i - 1
            gfx.removeDrawCallAt(removeIndex)
        end

        local originalIndex = ui.getNodeIndex(node)
        table.remove(node.parent.children, originalIndex)
        table.insert(parent.children, index, node)
        node.parent = parent

        drawCallStartIndex = 0
        ui.traverseNode(ui.rootNode, function(child)
            if child == node then
                return true
            else
                if child.pMaterial then
                    drawCallStartIndex = drawCallStartIndex + 1
                end
                return false
            end
        end)

        for i, dc in ipairs(drawCalls) do
            local insertIndex = drawCallStartIndex + i - 1
            gfx.insertDrawCallPtr(dc, insertIndex)
        end
        node.layoutDirty = true
        return true
    end
end

return ui
