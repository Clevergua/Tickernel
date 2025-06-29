#include <graphics.h>

void CreateVertexInputState(uint32_t inputVariableCount, SpvReflectInterfaceVariable *inputVariables, MeshLayout meshLayout)
{

    uint32_t vertexAttributeDescriptionCount = 0;

    // for (uint32_t i = 0; i < inputVariableCount; i++)
    // {

    //     if (inputVariable.type_description->type_flags == SPV_REFLECT_TYPE_FLAG_MATRIX)
    //     {
    //         uint32_t columnCount = inputVariable.type_description->traits.numeric.matrix.column_count;
    //         uint32_t rowCount = inputVariable.type_description->traits.numeric.matrix.row_count;
    //         for (uint32_t i = 0; i < rowCount; i++)
    //         {
    //             VkVertexInputAttributeDescription vertexAttributeDescription = {
    //                 .location = inputVariable.location + i,
    //                 .binding = binding,
    //                 .format = (VkFormat)inputVariable.format,
    //                 .offset = layout.offset + i * inputVariable.numeric.scalar.width * columnCount / 8,
    //             };
    //         }
    //     }
    //     else if (inputVariable.type_description->type_flags == SPV_REFLECT_TYPE_FLAG_VECTOR)
    //     {
    //         VkVertexInputAttributeDescription vertexAttributeDescription = {
    //             .location = inputVariable.location,
    //             .binding = binding,
    //             .format = (VkFormat)inputVariable.format,
    //             .offset = layout.offset,
    //         };
    //     }
    //     else
    //     {
    //         tickernelError("Unsupported input variable type: %s", inputVariable.name);
    //     }
    // }
    // VkVertexInputAttributeDescription *vertexAttributeDescriptions = tickernelMalloc(sizeof(VkVertexInputAttributeDescription) * vertexAttributeDescriptionCount);
    // VkPipelineVertexInputStateCreateInfo vertexInputState = {
    //     .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    //     .pNext = NULL,
    //     .flags = 0,
    //     .vertexBindingDescriptionCount = vertexBindingDescriptionCount,
    //     .pVertexBindingDescriptions = vertexBindingDescriptions,
    //     .vertexAttributeDescriptionCount = vertexAttributeDescriptionCount,
    //     .pVertexAttributeDescriptions = vertexAttributeDescriptions,
    // };

    // tickernelFree(vertexBindingDescriptions);
    // tickernelFree(vertexAttributeDescriptions);
}

int main()
{
    VkResult result = VK_SUCCESS;
    uint32_t stageCount = 4; // Example stage count
    const char *shaderPaths[] = {
        "/Users/forgemastergua/Documents/GitHub/Tickernel/assets/shaders/opaqueGeometry.vert.spv",
        "/Users/forgemastergua/Documents/GitHub/Tickernel/assets/shaders/opaqueGeometry.frag.spv",
        "/Users/forgemastergua/Documents/GitHub/Tickernel/assets/shaders/opaqueLighting.vert.spv",
        "/Users/forgemastergua/Documents/GitHub/Tickernel/assets/shaders/opaqueLighting.frag.spv",
    };
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

    // const char *requiredExtensions[] = {
    //     "VK_KHR_portability_enumeration",
    // };
    // VkApplicationInfo vkApplicationInfo = {
    //     .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    //     .pNext = NULL,
    //     .pApplicationName = "Tickernel",
    //     .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    //     .pEngineName = "Tickernel Engine",
    //     .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    //     .apiVersion = VK_API_VERSION_1_3,
    // };
    // VkInstanceCreateInfo vkInstanceCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    //     .pNext = NULL,
    //     .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
    //     .pApplicationInfo = &vkApplicationInfo,
    //     .enabledLayerCount = 0,
    //     .ppEnabledLayerNames = NULL,
    //     .enabledExtensionCount = 1,
    //     .ppEnabledExtensionNames = requiredExtensions,
    // };

    // VkInstance vkInstance;
    // result = vkCreateInstance(&vkInstanceCreateInfo, NULL, &vkInstance);
    // tickernelAssert(result == VK_SUCCESS, "Failed to create instance: %d", result);
    // GraphicsContext *pGraphicsContext = createGraphicsContext(2, VK_PRESENT_MODE_IMMEDIATE_KHR, vkInstance, NULL, 800, 600);
    // destroyGraphicsContext(pGraphicsContext);
    // VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    //     .pNext = NULL,
    //     .flags = 0,
    //     .stageCount = stageCount,
    //     .pStages = stages,
    //     .pVertexInputState = &vertexInputState,
    //     .pInputAssemblyState = &inputAssemblyState,
    //     .pTessellationState = NULL,
    //     .pViewportState = &viewportState,
    //     .pRasterizationState = &rasterizationState,
    //     .pMultisampleState = &multisampleState,
    //     .pDepthStencilState = &depthStencilState,
    //     .pColorBlendState = &colorBlendState,
    //     .pDynamicState = &dynamicState,
    //     .layout = vkPipelineLayout,
    //     .renderPass = pRenderPass->vkRenderPass,
    //     .subpass = subpassIndex,
    //     .basePipelineHandle = VK_NULL_HANDLE,
    //     .basePipelineIndex = -1,
    // };
    // tickernelFree(stages);
    return 0;
}