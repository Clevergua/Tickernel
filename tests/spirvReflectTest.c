#include <graphics.h>

// void test(SpvReflectInterfaceVariable **aaa)
// {
//     SpvReflectInterfaceVariable* a =  malloc(sizeof(SpvReflectInterfaceVariable));
//     SpvReflectInterfaceVariable* b =  malloc(sizeof(SpvReflectInterfaceVariable));
//     SpvReflectInterfaceVariable* c =  malloc(sizeof(SpvReflectInterfaceVariable));
//     aaa[0] = a;
//     aaa[1] = b;
//     aaa[2] = c;
// }
void CreateVertexInputState(uint32_t inputVariableCount, SpvReflectInterfaceVariable **pSpvReflectInterfaceVariables)
{
    // Example vertex input state creation
    VkVertexInputBindingDescription vertexBindingDescription = {
        .binding = 0,
        .stride = sizeof(float) * 3, // Assuming 3 floats per vertex (x, y, z)
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkVertexInputAttributeDescription vertexAttributeDescription = {
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT, // Assuming float3 format
        .offset = 0,
    };

    VkPipelineVertexInputStateCreateInfo vertexInputState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexBindingDescription,
        .vertexAttributeDescriptionCount = 1,
        .pVertexAttributeDescriptions = &vertexAttributeDescription,
    };
}

int main()
{
    VkResult result = VK_SUCCESS;
    uint32_t stageCount = 2; // Example stage count
    const char *shaderPaths[] = {
        "/Users/forgemastergua/Documents/GitHub/Tickernel/assets/shaders/opaqueGeometry.vert.spv",
        "/Users/forgemastergua/Documents/GitHub/Tickernel/assets/shaders/opaqueGeometry.frag.spv"};
    SpvReflectShaderModule spvReflectShaderModules[stageCount];
    for (uint32_t i = 0; i < stageCount; i++)
    {
        const char *filePath = shaderPaths[i];
        FILE *file = fopen(filePath, "rb");
        if (!file)
        {
            printf("Failed to open file: %s\n", filePath);
            tickernelError("Failed to open SPIR-V file");
        }
        fseek(file, 0, SEEK_END);
        size_t shaderSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (shaderSize % 4 != 0)
        {
            printf("Invalid SPIR-V file size: %s\n", filePath);
            fclose(file);
            tickernelError("Invalid SPIR-V file size");
        }
        void *shaderCode = tickernelMalloc(shaderSize);
        size_t bytesRead = fread(shaderCode, 1, shaderSize, file);

        fclose(file);

        if (bytesRead != shaderSize)
        {
            printf("Failed to read entire file: %s\n", filePath);
            tickernelError("Failed to read entire SPIR-V file");
        }

        SpvReflectResult spvReflectResult = spvReflectCreateShaderModule(shaderSize, shaderCode, &spvReflectShaderModules[i]);
        tickernelAssert(spvReflectResult == SPV_REFLECT_RESULT_SUCCESS, "Failed to reflect shader module: %s", shaderPaths[i]);
        tickernelFree(shaderCode);
    }
    // Create stages
    VkPipelineShaderStageCreateInfo *stages = tickernelMalloc(sizeof(VkPipelineShaderStageCreateInfo) * stageCount);
    for (uint32_t i = 0; i < stageCount; i++)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .codeSize = spvReflectGetCodeSize(&spvReflectShaderModules[i]),
            .pCode = spvReflectGetCode(&spvReflectShaderModules[i]),
        };
    }

    // Create descriptor set layouts
    uint32_t setLayoutCount = 0;

    for (uint32_t i = 0; i < stageCount; i++)
    {
        SpvReflectShaderModule spvReflectShaderModule = spvReflectShaderModules[i];
        uint32_t descriptorSetCount = 0;
        spvReflectEnumerateDescriptorSets(&spvReflectShaderModule, &descriptorSetCount, NULL);

        SpvReflectDescriptorSet **pSpvReflectDescriptorSets = tickernelMalloc(sizeof(SpvReflectDescriptorSet *) * descriptorSetCount);

        spvReflectEnumerateDescriptorSets(&spvReflectShaderModule, &descriptorSetCount, pSpvReflectDescriptorSets);
        for (size_t i = 0; i < descriptorSetCount; i++)
        {
            SpvReflectDescriptorSet *pSpvReflectDescriptorSet = pSpvReflectDescriptorSets[i];
            if (pSpvReflectDescriptorSet->set + 1 > setLayoutCount)
            {
                setLayoutCount = pSpvReflectDescriptorSet->set + 1;
            }
        }
        tickernelFree(pSpvReflectDescriptorSets);
    }
    VkDescriptorSetLayout *setLayouts = tickernelMalloc(sizeof(VkDescriptorSetLayout) * setLayoutCount);
    for (uint32_t i = 0; i < setLayoutCount; i++)
    {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .bindingCount = 0, // This should be set based on the actual bindings
            .pBindings = NULL, // This should be set based on the actual bindings
        };
        // result = vkCreateDescriptorSetLayout(vkDevice, &descriptorSetLayoutCreateInfo, NULL, &setLayouts[i]);
        // tickernelAssert(result == VK_SUCCESS, "Failed to create descriptor set layout %d", i);
    }
    printf("setLayoutCount: %d\n", setLayoutCount);
    tickernelFree(stages);
    return 0;
}
