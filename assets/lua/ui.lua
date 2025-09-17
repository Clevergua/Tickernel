require("gfx")
local uiRenderPass = require("uiRenderPass")

local ui = {}

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
    ui.drawCalls = {}
    ui.dirty = false
end

function ui.teardown(pGfxContext)
    ui.removeNode(ui.rootNode)

    ui.dirty = nil
    ui.drawCalls = nil
    ui.nodePool = nil
    ui.rootNode = nil

    gfx.destroyVertexInputLayoutPtr(pGfxContext, ui.pUIVertexInputLayout)
    ui.uiVertexFormat = nil
end

function ui.addNode(name, material, rect, parent)
    local node = nil
    if parent then
        if #ui.nodePool > 0 then
            node = table.remove(ui.nodePool)
            node.name = name
            node.parent = parent
            node.material = material
            node.rect = rect
            assert(#node.children == 0, "ui.addNode: node from pool has children")
        else
            node = {
                name = name,
                children = {},
                parent = parent,
                material = material,
                rect = rect,
            }
        end
        table.insert(parent.children, node)
    end

    return node
end

function ui.removeNode(node)
    if node.parent then
        node.parent = nil
        for i, child in ipairs(node.parent.children) do
            if child == node then
                table.remove(node.parent.children, i)
                break
            end
        end
    end

    for index, child in ipairs(node.children) do
        ui.removeNode(child)
    end
    node.name = nil
    table.insert(ui.nodePool, node)
end

function ui.traverseNodes(node, callback)
    callback(node)
    for index, child in ipairs(node.children) do
        ui.traverseNodes(child, callback)
    end
end

function ui.updateDrawCalls(pGfxContext)
    if ui.dirty then
        gfx.clearDrawCalls(pGfxContext, uiRenderPass.pUIPipeline)
        ui.drawCalls = {}

        ui.traverseNodes(ui.rootNode, function(node)
            if node.material then
                local drawCall = gfx.addDrawCallPtr(pGfxContext, uiRenderPass.pUIPipeline, uiRenderPass.pUIMaterial, node.rect)
                table.insert(ui.drawCalls, drawCall)
            end
        end)
        ui.dirty = false
    else
        -- nothing
    end

end
return ui
