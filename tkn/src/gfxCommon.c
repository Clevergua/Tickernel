#include "gfxCommon.h"

void assertVkResult(VkResult vkResult)
{
    tknAssert(vkResult == VK_SUCCESS, "Vulkan error: %d", vkResult);
}

VkFormat getSupportedFormat(GfxContext *pGfxContext, uint32_t candidateCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (uint32_t i = 0; i < candidateCount; i++)
    {
        VkFormat format = candidates[i];
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(pGfxContext->vkPhysicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR)
        {
            if ((props.linearTilingFeatures & features) == features)
            {
                return format;
            }
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL)
        {
            if ((props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
    }
    fprintf(stderr, "Error: No supported format found for the given requirements\n");
    return VK_FORMAT_MAX_ENUM;
}