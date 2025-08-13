#include "gfxCore.h"

void assertVkResult(VkResult vkResult)
{
    tknAssert(vkResult == VK_SUCCESS, "Vulkan error: %d", vkResult);
}

SpvReflectShaderModule createSpvReflectShaderModule(const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        tknError("Failed to open file: %s\n", filePath);
    }
    fseek(file, 0, SEEK_END);
    size_t shaderSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (shaderSize % 4 != 0)
    {
        fclose(file);
        tknError("Invalid SPIR-V file size: %s\n", filePath);
    }
    void *shaderCode = tknMalloc(shaderSize);
    size_t bytesRead = fread(shaderCode, 1, shaderSize, file);

    fclose(file);

    if (bytesRead != shaderSize)
    {
        tknError("Failed to read entire file: %s\n", filePath);
    }
    SpvReflectShaderModule spvReflectShaderModule;
    SpvReflectResult spvReflectResult = spvReflectCreateShaderModule(shaderSize, shaderCode, &spvReflectShaderModule);
    tknAssert(spvReflectResult == SPV_REFLECT_RESULT_SUCCESS, "Failed to reflect shader module: %s", filePath);
    tknFree(shaderCode);

    return spvReflectShaderModule;
}
void destroySpvReflectShaderModule(SpvReflectShaderModule *pSpvReflectShaderModule)
{
    spvReflectDestroyShaderModule(pSpvReflectShaderModule);
}