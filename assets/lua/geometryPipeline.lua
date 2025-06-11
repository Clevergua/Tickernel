local geometryPipeline = {}
local engine = require("engine")

geometryPipeline.createPipeline = function(pRenderPass, pVertexShader, pFragmentShader)

    -- typedef struct VkPipelineShaderStageCreateInfo {
    --     VkStructureType                     sType;
    --     const void*                         pNext;
    --     VkPipelineShaderStageCreateFlags    flags;
    --     VkShaderStageFlagBits               stage;
    --     VkShaderModule                      module;
    --     const char*                         pName;
    --     const VkSpecializationInfo*         pSpecializationInfo;
    -- } VkPipelineShaderStageCreateInfo;
    local stages = {
        {
            stage = VK_SHADER_STAGE_VERTEX_BIT,
            shaderPath = "assets/shaders/geometry.vert",
            pName = "main",
        },
        {
            stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            shaderPath = "assets/shaders/geometry.frag",
            pName = "main",
        }
    }
    local pPipeline = engine.createPipeline(stages, )
    return pPipeline
end

return geometryPipeline