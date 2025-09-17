require("gfx")
local format = {}

format.voxelVertexFormat = {{
    name = "position",
    type = TYPE_FLOAT,
    count = 3,
}, {
    name = "color",
    type = TYPE_UINT32,
    count = 1,
}, {
    name = "normal",
    type = TYPE_UINT32,
    count = 1,
}}


format.instanceFormat = {{
    name = "model",
    type = TYPE_FLOAT,
    count = 16,
}}

format.globalUniformBufferFormat = {{
    name = "view",
    type = TYPE_FLOAT,
    count = 16,
}, {
    name = "proj",
    type = TYPE_FLOAT,
    count = 16,
}, {
    name = "inv_view_proj",
    type = TYPE_FLOAT,
    count = 16,
}, {
    name = "pointSizeFactor",
    type = TYPE_FLOAT,
    count = 1,
}, {
    name = "time",
    type = TYPE_FLOAT,
    count = 1,
}, {
    name = "frameCount",
    type = TYPE_INT32,
    count = 1,
}, {
    name = "near",
    type = TYPE_FLOAT,
    count = 1,
}, {
    name = "far",
    type = TYPE_FLOAT,
    count = 1,
}, {
    name = "fov",
    type = TYPE_FLOAT,
    count = 1,
}}

format.lightsUniformBufferFormat = {{
    name = "directionalLightColor",
    type = TYPE_FLOAT,
    count = 4,
}, {
    name = "directionalLightDirection",
    type = TYPE_FLOAT,
    count = 4,
}, {
    -- PointLight array: 128 lights × (vec4 color + vec3 position + float range) = 128 × 8 floats
    name = "pointLights",
    type = TYPE_FLOAT,
    count = 128 * 8,
}, {
    name = "pointLightCount",
    type = TYPE_INT32,
    count = 1,
}}

function format.createLayouts(pGfxContext)
    format.pVoxelMeshVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, format.voxelVertexFormat)
    format.pInstanceVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, format.instanceFormat)
end

function format.destroyLayouts(pGfxContext)
    gfx.destroyVertexInputLayoutPtr(pGfxContext, format.pInstanceVertexInputLayout)
    gfx.destroyVertexInputLayoutPtr(pGfxContext, format.pVoxelMeshVertexInputLayout)
    format.pInstanceVertexInputLayout = nil
    format.pVoxelMeshVertexInputLayout = nil
end

return format
