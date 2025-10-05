require("gfx")
local uiRenderPass = require("uiRenderPass")
local image = require("image")
local text = require("text")
local ui = {
    drawables = {},
}

local fullScreenRect = {
    left = -1,
    right = 1,
    bottom = -1,
    top = 1,
}

local nodeTemplate = {
    name = "", -- node name
    children = {}, -- children array
    parent = nil, -- parent node (node or nil)
    component = nil, -- component (image or text ..)
    layout = {
        rect = {
            left = 0,
            right = 0,
            bottom = 0,
            top = 0,
        }, -- rectangle area in MDC coordinates
        dirty = true, -- whether layout is dirty
        horizontal = { -- horizontal layout
            type = "anchored", -- "anchored" or "relative"
            -- for anchored: anchor (0-1), pivot (0-1), width (pixels)
            anchor = 0.5,
            pivot = 0.5,
            width = 100,
            -- for relative: left, right (pixels or 0-1 ratio)
            -- left = 0, right = 0,
        },
        vertical = { -- vertical layout (similar to horizontal)
            type = "anchored",
            anchor = 0.5,
            pivot = 0.5,
            height = 100,
            -- bottom = 0, top = 0,
        },
    }, -- layout configuration (see layoutTemplate)
}

function ui.updateRect(pGfxContext, screenWidth, screenHeight, node, parentDirty)
    if node.layout.dirty or parentDirty then
        local parentRect = node == ui.rootNode and fullScreenRect or node.parent.layout.rect
        local layout = node.layout
        -- Handle horizontal layout
        if layout.horizontal.type == "anchored" then
            local anchor = layout.horizontal.anchor
            local pivot = layout.horizontal.pivot
            local width = layout.horizontal.width
            local x = parentRect.left + (parentRect.right - parentRect.left) * anchor - width * pivot
            node.layout.rect.left = x
            node.layout.rect.right = x + width / screenWidth * 2
        elseif layout.horizontal.type == "relative" then
            if math.type(layout.horizontal.left) == "integer" then
                node.layout.rect.left = parentRect.left + layout.horizontal.left / screenWidth * 2
            else
                node.layout.rect.left = parentRect.left + (parentRect.right - parentRect.left) * layout.horizontal.left
            end
            if math.type(layout.horizontal.right) == "integer" then
                node.layout.rect.right = parentRect.right - layout.horizontal.right / screenWidth * 2
            else
                node.layout.rect.right = parentRect.right - (parentRect.right - parentRect.left) * layout.horizontal.right
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
            node.layout.rect.bottom = y
            node.layout.rect.top = y + height / screenHeight * 2
        elseif layout.vertical.type == "relative" then
            if math.type(layout.vertical.bottom) == "integer" then
                node.layout.rect.bottom = parentRect.bottom + layout.vertical.bottom / screenHeight * 2
            else
                node.layout.rect.bottom = parentRect.bottom + (parentRect.top - parentRect.bottom) * layout.vertical.bottom
            end
            if math.type(layout.vertical.top) == "integer" then
                node.layout.rect.top = parentRect.top - layout.vertical.top / screenHeight * 2
            else
                node.layout.rect.top = parentRect.top - (parentRect.top - parentRect.bottom) * layout.vertical.top
            end
        else
            error("ui.calculateRect: unknown vertical layout type " .. tostring(layout.vertical.type))
        end

        if ui.isRenderable(node) then
            ui.updateMeshPtr(pGfxContext, node)
        end
        node.layout.dirty = false
        for i, child in ipairs(node.children) do
            ui.updateRect(pGfxContext, screenWidth, screenHeight, child, true)
        end
    else
        for i, child in ipairs(node.children) do
            ui.updateRect(pGfxContext, screenWidth, screenHeight, child, false)
        end
    end
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
            dirty = true,
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
        ui.rootNode.layout.dirty = true
    end
    ui.updateRect(pGfxContext, screenWidth, screenHeight, ui.rootNode, false)
end

function ui.isRenderable(node)
    if node.component then
        if node.component.type == "image" then
            return image.isRenderable(node.component)
        elseif node.component.type == "text" then
            return text.isRenderable(node.component)
        end
    else
        return false
    end
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

function ui.createMeshPtr(node)
    if node.component.type == "image" then
        image.createMeshPtr(ui.pGfxContext, node.component, node.layout.rect)
    elseif node.component.type == "text" then
        text.createMeshPtr(ui.pGfxContext, node.component, node.layout.rect)
    end

end

function ui.addNode(pGfxContext, parent, index, name, layout)
    assert(parent ~= nil, "ui.addNode: parent is nil")
    assert(index >= 1 and index <= #parent.children + 1, "ui.addNode: index out of bounds")
    local node = nil
    if #ui.nodePool > 0 then
        node = table.remove(ui.nodePool)
        node.name = name
        assert(#node.children == 0, "ui.addNode: node from pool has children")
        node.parent = parent
        node.component = nil
        node.layout = layout
    else
        node = {
            name = name,
            children = {},
            parent = parent,
            component = nil,
            layout = layout,
        }
    end
    table.insert(parent.children, index, node)
    return node
end

function ui.removeNode(pGfxContext, node)
    for i = #node.children, 1, -1 do
        ui.removeNode(pGfxContext, node.children[i])
    end
    if node.component then
        image.destroyImageComponent(pGfxContext, node.component)
    end
    if node.component.pDrawCall then
        gfx.removeDrawCallPtr(node.component.pDrawCall)
        gfx.destroyDrawCallPtr(pGfxContext, node.component.pDrawCall)
        node.component.pDrawCall = nil
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
    node.component = nil
    node.layout = nil
    table.insert(ui.nodePool, node)
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
        if ui.isRenderable(child) then
            table.insert(drawCalls, child.component.pDrawCall)
        end
    end)

    if #drawCalls == 0 then
        local originalIndex = ui.getNodeIndex(node)
        table.remove(node.parent.children, originalIndex)
        table.insert(parent.children, index, node)
        node.parent = parent
        node.layout.dirty = true
        return true
    else
        local drawCallStartIndex = 0
        ui.traverseNode(ui.rootNode, function(child)
            if child == node then
                return true
            else
                if ui.isRenderable(child) then
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
                if ui.isRenderable(child) then
                    drawCallStartIndex = drawCallStartIndex + 1
                end
                return false
            end
        end)

        for i, dc in ipairs(drawCalls) do
            local insertIndex = drawCallStartIndex + i - 1
            gfx.insertDrawCallPtr(dc, insertIndex)
        end
        node.layout.dirty = true
        return true
    end
end

return ui
