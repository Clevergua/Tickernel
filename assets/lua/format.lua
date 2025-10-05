local gfx = require("gfx")
local format = {}

function format.createLayouts(pGfxContext)
    format.voxelVertexFormat = {{
        name = "position",
        type = gfx.TYPE_FLOAT,
        count = 3,
    }, {
        name = "color",
        type = gfx.TYPE_UINT32,
        count = 1,
    }, {
        name = "normal",
        type = gfx.TYPE_UINT32,
        count = 1,
    }}

    format.instanceFormat = {{
        name = "model",
        type = gfx.TYPE_FLOAT,
        count = 16,
    }}

    format.globalUniformBufferFormat = {{
        name = "view",
        type = gfx.TYPE_FLOAT,
        count = 16,
    }, {
        name = "proj",
        type = gfx.TYPE_FLOAT,
        count = 16,
    }, {
        name = "inv_view_proj",
        type = gfx.TYPE_FLOAT,
        count = 16,
    }, {
        name = "pointSizeFactor",
        type = gfx.TYPE_FLOAT,
        count = 1,
    }, {
        name = "time",
        type = gfx.TYPE_FLOAT,
        count = 1,
    }, {
        name = "frameCount",
        type = gfx.TYPE_INT32,
        count = 1,
    }, {
        name = "near",
        type = gfx.TYPE_FLOAT,
        count = 1,
    }, {
        name = "far",
        type = gfx.TYPE_FLOAT,
        count = 1,
    }, {
        name = "fov",
        type = gfx.TYPE_FLOAT,
        count = 1,
    }}

    format.lightsUniformBufferFormat = {{
        name = "directionalLightColor",
        type = gfx.TYPE_FLOAT,
        count = 4,
    }, {
        name = "directionalLightDirection",
        type = gfx.TYPE_FLOAT,
        count = 4,
    }, {
        -- PointLight array: 128 lights × (vec4 color + vec3 position + float range) = 128 × 8 floats
        name = "pointLights",
        type = gfx.TYPE_FLOAT,
        count = 128 * 8,
    }, {
        name = "pointLightCount",
        type = gfx.TYPE_INT32,
        count = 1,
    }}
    format.voxelVertexFormat.pVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, format.voxelVertexFormat)
    format.instanceFormat.pVertexInputLayout = gfx.createVertexInputLayoutPtr(pGfxContext, format.instanceFormat)
end

function format.destroyLayouts(pGfxContext)
    gfx.destroyVertexInputLayoutPtr(pGfxContext, format.instanceFormat.pVertexInputLayout)
    gfx.destroyVertexInputLayoutPtr(pGfxContext, format.voxelVertexFormat.pVertexInputLayout)
    format.instanceFormat.pVertexInputLayout = nil
    format.voxelVertexFormat.pVertexInputLayout = nil
end

return format
