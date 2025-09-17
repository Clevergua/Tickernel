require("gfx")
local ui = {}

function ui.setup()
    ui.rootNode = {
        name = "root",
        children = {},
    }
    ui.currentNode = ui.rootNode
    ui.nodePool = {}
    ui.drawCalls = {}
end

function ui.teardown()
    ui.removeNode(ui.rootNode)
    ui.rootNode = nil
    ui.currentNode = nil
    ui.nodePool = nil
    ui.drawCalls = nil
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

return ui
