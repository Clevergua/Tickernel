local image = {
    pool = {},
}

function image.createImageComponent(pGfxContext, color, slice, pMaterial, rect)
    local component
    if #image.pool > 0 then
        component = table.remove(image.pool)
        component.color = color
        component.slice = slice
        component.pMaterial = pMaterial
        component.pMesh = image.createMeshPtr(pGfxContext, component, rect)
        component.pDrawCall = gfx.createDrawCallPtr(pGfxContext, pMaterial.pPipeline, pMaterial, component.pMesh, nil)
    else
        component = {
            type = "image",
            color = color,
            slice = slice,
            pMaterial = pMaterial,
            pMesh = image.createMeshPtr(pGfxContext, component, rect),
            pDrawCall = gfx.createDrawCallPtr(pGfxContext, pMaterial.pPipeline, pMaterial, component.pMesh, nil),
        }
    end
    return component
end
function image.destroyImageComponent(pGfxContext, component)
    gfx.destroyDrawCallPtr(pGfxContext, component.pDrawCall)
    gfx.destroyMeshPtr(pGfxContext, component.pMesh)
    component.pMaterial = nil
    component.pMesh = nil
    component.pDrawCall = nil
    component.slice = nil
    component.color = 0xFFFFFFFF

    table.insert(image.pool, component)
end

local function buildNineSlice(component, rect)
    local slice = component.slice
    local texW = rect.right - rect.left
    local texH = rect.top - rect.bottom

    local leftPx = slice.left * texW
    local rightPx = slice.right * texW
    local topPx = slice.top * texH
    local bottomPx = slice.bottom * texH

    local uLeft = leftPx / texW
    local uRight = 1 - rightPx / texW
    local vBottom = bottomPx / texH
    local vTop = 1 - topPx / texH

    local rectW = texW
    local rectH = texH

    local vertices = {}
    local indices = {}
    local quadIndex = 0

    local function pushQuad(x1, y1, x2, y2, u1, v1, u2, v2)
        local color = component.color
        local px1 = rect.left + x1 / texW * rectW
        local py1 = rect.bottom + y1 / texH * rectH
        local px2 = rect.left + x2 / texW * rectW
        local py2 = rect.bottom + y2 / texH * rectH

        table.insert(vertices, px1);
        table.insert(vertices, py1);
        table.insert(vertices, u1);
        table.insert(vertices, v1);
        table.insert(vertices, color)
        table.insert(vertices, px2);
        table.insert(vertices, py1);
        table.insert(vertices, u2);
        table.insert(vertices, v1);
        table.insert(vertices, color)
        table.insert(vertices, px2);
        table.insert(vertices, py2);
        table.insert(vertices, u2);
        table.insert(vertices, v2);
        table.insert(vertices, color)
        table.insert(vertices, px1);
        table.insert(vertices, py2);
        table.insert(vertices, u1);
        table.insert(vertices, v2);
        table.insert(vertices, color)

        local base = quadIndex * 4
        indices[#indices + 1] = base
        indices[#indices + 1] = base + 1
        indices[#indices + 1] = base + 2
        indices[#indices + 1] = base + 2
        indices[#indices + 1] = base + 3
        indices[#indices + 1] = base
        quadIndex = quadIndex + 1
    end

    -- order: bottom-left, bottom-mid, bottom-right, mid-left, center, mid-right, top-left, top-mid, top-right
    pushQuad(0, 0, leftPx, bottomPx, 0, 0, uLeft, vBottom)
    pushQuad(leftPx, 0, texW - rightPx, bottomPx, uLeft, 0, uRight, vBottom)
    pushQuad(texW - rightPx, 0, texW, bottomPx, uRight, 0, 1, vBottom)
    pushQuad(0, bottomPx, leftPx, texH - topPx, 0, vBottom, uLeft, vTop)
    pushQuad(leftPx, bottomPx, texW - rightPx, texH - topPx, uLeft, vBottom, uRight, vTop)
    pushQuad(texW - rightPx, bottomPx, texW, texH - topPx, uRight, vBottom, 1, vTop)
    pushQuad(0, texH - topPx, leftPx, texH, 0, vTop, uLeft, 1)
    pushQuad(leftPx, texH - topPx, texW - rightPx, texH, uLeft, vTop, uRight, 1)
    pushQuad(texW - rightPx, texH - topPx, texW, texH, uRight, vTop, 1, 1)

    return vertices, indices
end

function image.createMeshPtr(pGfxContext, component, rect)
    if component.slice then
        local vertices, indices = buildNineSlice(component, rect)
        component.pMesh = gfx.createMeshPtrWithData(pGfxContext, image.pUIVertexInputLayout, image.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
    else
        local vertices = {
            position = {rect.left, rect.bottom, rect.right, rect.bottom, rect.right, rect.top, rect.left, rect.top},
            uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
            color = {component.color, component.color, component.color, component.color},
        }
        local indices = {0, 1, 2, 2, 3, 0}
        component.pMesh = gfx.createMeshPtrWithData(pGfxContext, image.pUIVertexInputLayout, image.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
    end
end

function image.destroyMeshPtr(pGfxContext, component)
    gfx.destroyMeshPtr(pGfxContext, component.pMesh)
    component.pMesh = nil
end

function image.updateMeshPtr(pGfxContext, component, rect)
    if component.slice then
        local vertices, indices = buildNineSlice(component, rect)
        gfx.updateMeshPtr(pGfxContext, component.pMesh, image.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
    else
        local vertices = {
            position = {rect.left, rect.bottom, rect.right, rect.bottom, rect.right, rect.top, rect.left, rect.top},
            uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
            color = {component.color, component.color, component.color, component.color},
        }
        local indices = {0, 1, 2, 2, 3, 0}
        gfx.updateMeshPtr(pGfxContext, component.pMesh, image.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
    end
end

function image.isRenderable(component)
    return component and component.pMaterial ~= nil
end
return image
