local gfx = require("gfx")
local image = {
    pool = {},
}

function image.createImageComponent(pGfxContext, color, slice, pMaterial)
    local image
    if #image.pool > 0 then
        image = table.remove(image.pool)
        image.color = color
        image.slice = slice
        image.pMaterial = pMaterial
        image.pMesh = nil
        image.pDrawCall = nil
    else
        image = {
            type = "image",
            color = color,
            slice = slice,
            pMaterial = pMaterial,
            pMesh = nil,
            pDrawCall = nil,
        }
    end
    return image
end
function image.destroyImageComponent(pGfxContext, image)
    gfx.destroyDrawCallPtr(pGfxContext, image.pDrawCall)
    gfx.destroyMeshPtr(pGfxContext, image.pMesh)
    image.pMaterial = nil
    image.pMesh = nil
    image.pDrawCall = nil
    image.slice = nil
    image.color = 0xFFFFFFFF

    table.insert(image.pool, image)
end

function image.createMeshPtr(pGfxContext, image)
    local vertices = {
        position = {},
        uv = {},
        color = {},
    }
    local vertexCount = 16

    for i = 1, vertexCount do
        table.insert(vertices.position, 0.0)
        table.insert(vertices.position, 0.0)
        table.insert(vertices.uv, 0.0)
        table.insert(vertices.uv, 0.0)
        table.insert(vertices.color, image.color)
    end

    local indices = {}
    local indexCount = 54 -- 6 * 9
    for i = 1, indexCount do
        indices[i] = 0
    end
    image.pMesh = gfx.createMeshPtrWithData(pGfxContext, image.uiVertexFormat.pVertexInputLayout, image.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
end

function image.destroyMeshPtr(pGfxContext, image)
    gfx.destroyMeshPtr(pGfxContext, image.pMesh)
    image.pMesh = nil
end

function image.updateMeshPtr(pGfxContext, image, rect)
    if image.slice then
        -- Nine-slice: generate 4x4 grid with 16 vertices
        local vertices = {
            position = {},
            uv = {},
            color = {},
        }
        
        -- Simplified implementation: fill 16 vertices with default data
        for i = 1, 16 do
            table.insert(vertices.position, rect.left)
            table.insert(vertices.position, rect.bottom)
            table.insert(vertices.uv, 0.0)
            table.insert(vertices.uv, 0.0)
            table.insert(vertices.color, image.color)
        end
        
        -- Generate indices (9 quads)
        local indices = {}
        for quad = 1, 9 do
            local base = (quad - 1) * 4
            table.insert(indices, base)
            table.insert(indices, base + 1)
            table.insert(indices, base + 2)
            table.insert(indices, base + 2)
            table.insert(indices, base + 3)
            table.insert(indices, base)
        end
        
        gfx.updateMeshPtr(pGfxContext, image.pMesh, image.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
    else
        -- Regular quad: 4 vertices
        local vertices = {
            position = {rect.left, rect.bottom, rect.right, rect.bottom, rect.right, rect.top, rect.left, rect.top},
            uv = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0},
            color = {image.color, image.color, image.color, image.color},
        }
        local indices = {0, 1, 2, 2, 3, 0}
        gfx.updateMeshPtr(pGfxContext, image.pMesh, image.uiVertexFormat, vertices, VK_INDEX_TYPE_UINT16, indices)
    end
end

function image.isRenderable(image)
    return image and image.pMaterial ~= nil
end
return image
