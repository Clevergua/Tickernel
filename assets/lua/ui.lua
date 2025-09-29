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
    if node.layout.border then
        -- 九宫格：生成9个Quad
        local border = node.layout.border
        local texW = node.textureSize[1]
        local texH = node.textureSize[2]
        local leftPx = border.left * texW
        local rightPx = border.right * texW
        local topPx = border.top * texH
        local bottomPx = border.bottom * texH
        
        -- 纹理UV区域
        local uLeft = leftPx / texW
        local uRight = 1 - rightPx / texW
        local vBottom = bottomPx / texH
        local vTop = 1 - topPx / texH
        
        -- 9个Quad的vertices (position, uv, color)
        local vertices = {}
        local indices = {}
        local quadIndex = 0
        
        -- 辅助函数：添加Quad
        local function addQuad(x1, y1, x2, y2, u1, v1, u2, v2)
            local base = #vertices / 5  -- 每个顶点5个float: pos2 + uv2 + color1(uint but as float?)
            -- 实际color是uint，但这里简化
            local color = node.color
            table.insert(vertices, x1) table.insert(vertices, y1) table.insert(vertices, u1) table.insert(vertices, v1) table.insert(vertices, color)
            table.insert(vertices, x2) table.insert(vertices, y1) table.insert(vertices, u2) table.insert(vertices, v1) table.insert(vertices, color)
            table.insert(vertices, x2) table.insert(vertices, y2) table.insert(vertices, u2) table.insert(vertices, v2) table.insert(vertices, color)
            table.insert(vertices, x1) table.insert(vertices, y2) table.insert(vertices, u1) table.insert(vertices, v2) table.insert(vertices, color)
            local idxBase = quadIndex * 4
            table.insert(indices, idxBase) table.insert(indices, idxBase+1) table.insert(indices, idxBase+2)
            table.insert(indices, idxBase+2) table.insert(indices, idxBase+3) table.insert(indices, idxBase)
            quadIndex = quadIndex + 1   
        end
        
        addQuad(0, 0, leftPx, bottomPx, 0, 0, uLeft, vBottom)
        addQuad(leftPx, 0, texW - rightPx, bottomPx, uLeft, 0, uRight, vBottom)
        addQuad(texW - rightPx, 0, texW, bottomPx, uRight, 0, 1, vBottom)
        addQuad(0, bottomPx, leftPx, texH - topPx, 0, vBottom, uLeft, vTop)
        addQuad(leftPx, bottomPx, texW - rightPx, texH - topPx, uLeft, vBottom, uRight, vTop)
        addQuad(texW - rightPx, bottomPx, texW, texH - topPx, uRight, vBottom, 1, vTop)
        addQuad(0, texH - topPx, leftPx, texH, 0, vTop, uLeft, 1)
        addQuad(leftPx, texH - topPx, texW - rightPx, texH, uLeft, vTop, uRight, 1)
        addQuad(texW - rightPx, texH - topPx, texW, texH, uRight, vTop, 1, 1)
        
        node.pMesh = gfx.createMeshPtrWithData(pGfxContext, ui.pUIVertexInputLayout, ui.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
    else
        local vertices = {
            position = {0, 0, 0, 0, 0, 0, 0, 0},
            uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
            color = {node.color, node.color, node.color, node.color},
        }
        local indices = {0, 1, 2, 2, 3, 0}
        node.pMesh = gfx.createMeshPtrWithData(pGfxContext, ui.pUIVertexInputLayout, ui.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
    end
end

function ui.destroyMeshPtr(pGfxContext, node)
    gfx.destroyMeshPtr(pGfxContext, node.pMesh)
    node.pMesh = nil
end

function ui.updateMeshPtr(pGfxContext, node)
    if node.layout.border then
        -- 九宫格更新
        local border = node.layout.border
        local texW = node.textureSize[1]
        local texH = node.textureSize[2]
        local leftPx = border.left * texW
        local rightPx = border.right * texW
        local topPx = border.top * texH
        local bottomPx = border.bottom * texH
        
        local rect = node.rect
        local rectW = rect.right - rect.left
        local rectH = rect.top - rect.bottom
        
        -- 计算拉伸后的位置
        local leftStretch = leftPx
        local rightStretch = rectW - (texW - rightPx)
        local bottomStretch = bottomPx
        local topStretch = rectH - (texH - topPx)
        
        -- 9个Quad的位置更新 (position only, UV不变)
        local vertices = {}
        local indices = {0,1,2,2,3,0, 4,5,6,6,7,0, 8,9,10,10,11,0, 12,13,14,14,15,0, 16,17,18,18,19,0, 20,21,22,22,23,0, 24,25,26,26,27,0, 28,29,30,30,31,0, 32,33,34,34,35,0}  -- 9个Quad的indices
        
        -- 辅助函数：添加Quad position
        local function addQuadPos(x1, y1, x2, y2)
            table.insert(vertices, rect.left + x1 / texW * rectW)
            table.insert(vertices, rect.bottom + y1 / texH * rectH)
            table.insert(vertices, rect.left + x2 / texW * rectW)
            table.insert(vertices, rect.bottom + y1 / texH * rectH)
            table.insert(vertices, rect.left + x2 / texW * rectW)
            table.insert(vertices, rect.bottom + y2 / texH * rectH)
            table.insert(vertices, rect.left + x1 / texW * rectW)
            table.insert(vertices, rect.bottom + y2 / texH * rectH)
        end
        
        -- 左下角 (固定)
        addQuadPos(0, 0, leftPx, bottomPx)
        -- 下边 (水平拉伸)
        addQuadPos(leftPx, 0, texW - rightPx, bottomPx)
        -- 右下角 (固定)
        addQuadPos(texW - rightPx, 0, texW, bottomPx)
        -- 左边 (垂直拉伸)
        addQuadPos(0, bottomPx, leftPx, texH - topPx)
        -- 中间 (双向拉伸)
        addQuadPos(leftPx, bottomPx, texW - rightPx, texH - topPx)
        -- 右边 (垂直拉伸)
        addQuadPos(texW - rightPx, bottomPx, texW, texH - topPx)
        -- 左上角 (固定)
        addQuadPos(0, texH - topPx, leftPx, texH)
        -- 上边 (水平拉伸)
        addQuadPos(leftPx, texH - topPx, texW - rightPx, texH)
        -- 右上角 (固定)
        addQuadPos(texW - rightPx, texH - topPx, texW, texH)
        
        -- UV不变，从createMeshPtr
        local uvs = {0,0, uLeft,0, uLeft,vBottom, 0,vBottom,  uLeft,0, uRight,0, uRight,vBottom, uLeft,vBottom,  uRight,0, 1,0, 1,vBottom, uRight,vBottom,  0,vBottom, uLeft,vBottom, uLeft,vTop, 0,vTop,  uLeft,vBottom, uRight,vBottom, uRight,vTop, uLeft,vTop,  uRight,vBottom, 1,vBottom, 1,vTop, uRight,vTop,  0,vTop, uLeft,vTop, uLeft,1, 0,1,  uLeft,vTop, uRight,vTop, uRight,1, uLeft,1,  uRight,vTop, 1,vTop, 1,1, uRight,1}
        -- 合并vertices: pos + uv + color
        local fullVertices = {}
        for i = 1, #vertices, 2 do
            table.insert(fullVertices, vertices[i])
            table.insert(fullVertices, vertices[i+1])
            table.insert(fullVertices, uvs[i])
            table.insert(fullVertices, uvs[i+1])
            table.insert(fullVertices, node.color)
        end
        
        gfx.updateMeshPtr(pGfxContext, node.pMesh, ui.uiVertexFormat, fullVertices, VK_INDEX_TYPE_UINT16, indices)
    else
        -- 普通更新
        local rect = node.rect
        local vertices = {
            position = {rect.left, rect.bottom, rect.right, rect.bottom, rect.right, rect.top, rect.left, rect.top},
            uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
            color = {node.color, node.color, node.color, node.color},
        }
        local indices = {0, 1, 2, 2, 3, 0}
        gfx.updateMeshPtr(pGfxContext, node.pMesh, ui.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
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

function ui.addNode(pGfxContext, parent, index, name, layout, pMaterial, color, textureSize)
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
            node.textureSize = textureSize  -- 纹理大小 {width, height}
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
                textureSize = textureSize,
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
    node.textureSize = nil
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
