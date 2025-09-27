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

function ui.hasDrawCall(node)
    return node.rect ~= nil and node.pMaterial ~= nil
end

function ui.updateNodeLayout(screenWidth, screenHeight, dirtyNode)
    ui.traverseNode(dirtyNode, function(node)
        if node.layoutDirty then
            local parentRect = node == ui.rootNode and ui.fullScreenRect or node.parent.rect
            local layout = node.layout
            if layout.type == "anchored" then
                -- Anchored positioning with anchor and pivot
                local anchor = layout.anchor
                local pivot = layout.pivot
                local width = layout.width
                local height = layout.height
                local x = parentRect.left + (parentRect.right - parentRect.left) * anchor.x - width * pivot.x
                local y = parentRect.bottom + (parentRect.top - parentRect.bottom) * anchor.y - height * pivot.y
                node.rect.left = x
                node.rect.bottom = y
                node.rect.right = x + width / screenWidth * 2
                node.rect.top = y + height / screenHeight * 2
            elseif layout.type == "relative" then
                -- Relative positioning with pixel or percentage offsets
                if math.type(layout.left) == "integer" then
                    node.rect.left = parentRect.left + layout.left / screenWidth * 2
                else
                    node.rect.left = parentRect.left + (parentRect.right - parentRect.left) * layout.left
                end
                if math.type(layout.bottom) == "integer" then
                    node.rect.bottom = parentRect.bottom + layout.bottom / screenHeight * 2
                else
                    node.rect.bottom = parentRect.bottom + (parentRect.top - parentRect.bottom) * layout.bottom
                end
                if math.type(layout.right) == "integer" then
                    node.rect.right = parentRect.right - layout.right / screenWidth * 2
                else
                    node.rect.right = parentRect.right - (parentRect.right - parentRect.left) * layout.right
                end
                if math.type(layout.top) == "integer" then
                    node.rect.top = parentRect.top - layout.top / screenHeight * 2
                else
                    node.rect.top = parentRect.top - (parentRect.top - parentRect.bottom) * layout.top
                end
            else
                error("ui.updateLayout: unknown layout type " .. tostring(layout.type))
            end
        end
        -- if ui.hasDrawCall(node) then
        --     local drawCallIndex = 0
        --     for i, child in ipairs(ui.rootNode.children) do
        --         if ui.hasDrawCall(child) then
        --             drawCallIndex = drawCallIndex + 1
        --             if child == node then
        --                 break
        --             end
        --         end
        --     end
        --     node.pMesh = ui.createMeshPtr(pGfxContext, node)
        --     node.drawCall = gfx.createDrawCallPtr(pGfxContext, uiRenderPass.pUIPipeline, node.pMaterial, node.pMesh, nil)
        --     gfx.insertDrawCallPtr(node.drawCall, drawCallIndex)
        -- end
        if node.mesh then
            
        else

        end
        node.layoutDirty = false
        return false
    end)
end

function ui.createMeshPtr(pGfxContext, node)
    local rect = node.rect
    local vertices = {
        position = {rect.left, rect.bottom, rect.right, rect.bottom, rect.right, rect.top, rect.left, rect.top},
        uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
        color = {node.color, node.color, node.color, node.color},
    }
    local indices = {0, 1, 2, 2, 3, 0}
    return gfx.createMeshPtrWithData(pGfxContext, ui.pUIVertexInputLayout, ui.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
end

function ui.destroyMeshPtr(pGfxContext, pMesh)
    gfx.destroyMeshPtr(pGfxContext, pMesh)
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
        layout = {
            type = "relative",
            left = 0,
            bottom = 0,
            right = 0,
            top = 0,
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
function ui.update()
    -- update layout
    ui.traverseNode(ui.rootNode, function(node)
        if node.layoutDirty then
            ui.updateRect(node)
        end
        return false
    end)
end

function ui.addNode(pGfxContext, name, pMaterial, color, rect, parent, index, layout)
    if not parent then
        return nil
    end

    local node = nil
    if #ui.nodePool > 0 then
        node = table.remove(ui.nodePool)
        node.name = name
        node.parent = parent
        node.rect = rect
        node.color = color
        node.pMaterial = pMaterial
        node.pMesh = nil
        node.drawCall = nil
        node.layout = layout
        node.layoutDirty = true
        assert(#node.children == 0, "ui.addNode: node from pool has children")
    else
        node = {
            name = name,
            children = {},
            parent = parent,
            rect = rect,
            color = color,
            pMaterial = pMaterial,
            pMesh = nil,
            drawCall = nil,
            layout = layout,
            layoutDirty = true,
        }
    end
    table.insert(parent.children, index, node)

    return node
end

function ui.removeNode(pGfxContext, node)
    for i = #node.children, 1, -1 do
        ui.removeNode(pGfxContext, node.children[i])
    end

    if node.drawCall then
        gfx.removeDrawCallPtr(node.drawCall)
        gfx.destroyDrawCallPtr(pGfxContext, node.drawCall)
        node.drawCall = nil
    end

    if node.pMesh then
        ui.destroyMeshPtr(pGfxContext, node.pMesh)
        node.pMesh = nil
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
    node.color = nil
    node.pMaterial = nil
    node.pMesh = nil
    node.drawCall = nil
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
    ui.traverseNode(node, function(n)
        if ui.hasDrawCall(n) then
            table.insert(drawCalls, n.drawCall)
        end
    end)

    if #drawCalls == 0 then
        local originalIndex = ui.getNodeIndex(node)
        table.remove(node.parent.children, originalIndex)
        table.insert(parent.children, index, node)
        node.parent = parent
        if node.rect and node.rect.anchorMin then
            ui.updateRect(node)
        end
        node.layoutDirty = true
        return true
    else
        local drawCallStartIndex = 0
        ui.traverseNode(ui.rootNode, function(n)
            if n == node then
                return true
            else
                if ui.hasDrawCall(n) then
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
        ui.traverseNode(ui.rootNode, function(n)
            if n == node then
                return true
            else
                if ui.hasDrawCall(n) then
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
