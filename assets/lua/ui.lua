-- UI
-- For Node's Layout, if we choose to update on real-time add/delete/modify operations, there might be cases where children are updated before parents, leading to redundant calculations. Therefore, we adopt a unified update approach during the update phase.
-- The creation, deletion, and modification of drawcalls and meshes depend on the add/delete/modify operations of nodes and components.
-- Updating mesh requires layout update first, so it always happens after the update layout phase.
local gfx = require("gfx")
local uiRenderPass = require("uiRenderPass")
local image = require("image")
local text = require("text")
local ui = {}

local fullScreenRect = {
    left = -1,
    right = 1,
    bottom = -1,
    top = 1,
}

local function updateRect(pGfxContext, screenWidth, screenHeight, node, parentDirty)
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

        -- Update mesh
        if node.component and node.component.pMesh then
            if node.component.type == "image" then
                image.updateMeshPtr(ui.pGfxContext, node.component, node.layout.rect, ui.vertexFormat)
            else
                error("ui.updateRect: unsupported component type " .. tostring(node.component.type))
            end
        end

        node.layout.dirty = false
        for i, child in ipairs(node.children) do
            updateRect(pGfxContext, screenWidth, screenHeight, child, true)
        end
    else
        for i, child in ipairs(node.children) do
            updateRect(pGfxContext, screenWidth, screenHeight, child, false)
        end
    end
end

local function addComponent(pGfxContext, node, component)
    if node.component then
        print("WARNING: ui.addComponent: node already has a component")
    else
        node.component = component
        if component.pDrawCall then
            local drawCallIndex = 0
            ui.traverseNode(ui.rootNode, function(child)
                if child == node then
                    return true
                else
                    if child.component and child.component.pDrawCall then
                        drawCallIndex = drawCallIndex + 1
                    end
                    return false
                end
            end)
            gfx.insertDrawCallPtr(node.component, drawCallIndex)
        end
    end
end

local function removeComponent(pGfxContext, node)
    local component = node.component
    if component then
        if component.pDrawCall then
            local drawCallIndex = 0
            ui.traverseNode(ui.rootNode, function(child)
                if child == node then
                    return true
                else
                    if child.component and child.component.pDrawCall then
                        drawCallIndex = drawCallIndex + 1
                    end
                    return false
                end
            end)
            gfx.removeDrawCallAt(drawCallIndex)
        end
    else
        print("WARNING: ui.removeComponent: node has no component")
        return
    end
end

function ui.setup(pGfxContext, pSwapchainAttachment, assetsPath)
    ui.pGfxContext = pGfxContext
    ui.vertexFormat = {
        {
            name = "position",
            type = gfx.TYPE_FLOAT,
            count = 2,
        },
        {
            name = "uv",
            type = gfx.TYPE_FLOAT,
            count = 2,
        },
        {
            name = "color",
            type = gfx.TYPE_UINT32,
            count = 1,
        },
        pVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, ui.vertexFormat),
    }
    uiRenderPass.setup(pGfxContext, pSwapchainAttachment, assetsPath, ui.vertexFormat.pVertexInputLayout)
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

    gfx.destroyVertexInputLayoutPtr(pGfxContext, ui.vertexFormat.pVertexInputLayout)
    ui.vertexFormat.pVertexInputLayout = nil
    ui.vertexFormat = nil
end

function ui.updateLayout(pGfxContext, screenWidth, screenHeight)
    if ui.width ~= screenWidth or ui.height ~= screenHeight then
        ui.width = screenWidth
        ui.height = screenHeight
        ui.rootNode.layout.dirty = true
    end
    updateRect(pGfxContext, screenWidth, screenHeight, ui.rootNode, false)
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
    removeComponent(pGfxContext, node)
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
        if child.component and child.component.pDrawCall then
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
                if child.component and child.component.pDrawCall then
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
                if child.component and child.component.pDrawCall then
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

function ui.addImageComponent(pGfxContext, color, slice, pMaterial, node)
    local component = image.createComponent(pGfxContext, color, slice, pMaterial, ui.vertexFormat, node)
    addComponent(pGfxContext, node, component)
    return component
end

function ui.removeImageComponent(pGfxContext, node)
    assert(node.component and node.component.type == "image", "ui.removeImageComponent: node has no image component")
    image.destroyComponent(pGfxContext, node.component)
    removeComponent(pGfxContext, node)
end

return ui
