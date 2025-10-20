#include "gfxCore.h"

Sampler *createSamplerPtr(GfxContext *pGfxContext, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressModeU, VkSamplerAddressMode addressModeV, VkSamplerAddressMode addressModeW, float mipLodBias, VkBool32 anisotropyEnable, float maxAnisotropy, float minLod, float maxLod, VkBorderColor borderColor)
{
    Sampler *pSampler = tknMalloc(sizeof(Sampler));
    // Initialize the hash set for binding references
    pSampler->bindingPtrHashSet = tknCreateHashSet(sizeof(Binding *));
    // Create sampler create info with provided parameters
    VkSamplerCreateInfo samplerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .magFilter = magFilter,
        .minFilter = minFilter,
        .mipmapMode = mipmapMode,
        .addressModeU = addressModeU,
        .addressModeV = addressModeV,
        .addressModeW = addressModeW,
        .mipLodBias = mipLodBias,
        .anisotropyEnable = anisotropyEnable,
        .maxAnisotropy = maxAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = minLod,
        .maxLod = maxLod,
        .borderColor = borderColor,
        .unnormalizedCoordinates = VK_FALSE,
    };

    // Create the Vulkan sampler
    VkResult result = vkCreateSampler(pGfxContext->vkDevice, &samplerCreateInfo, NULL, &pSampler->vkSampler);
    if (result != VK_SUCCESS)
    {
        tknError("Failed to create Vulkan sampler: %d", result);
        tknDestroyHashSet(pSampler->bindingPtrHashSet);
        tknFree(pSampler);
        return NULL;
    }
    
    return pSampler;
}

void destroySamplerPtr(GfxContext *pGfxContext, Sampler *pSampler)
{
    if (pSampler == NULL)
    {
        return;
    }
    
    // Clear all binding references
    clearBindingPtrHashSet(pGfxContext, pSampler->bindingPtrHashSet);
    
    // Destroy the hash set
    tknDestroyHashSet(pSampler->bindingPtrHashSet);
    
    // Destroy the Vulkan sampler
    if (pSampler->vkSampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(pGfxContext->vkDevice, pSampler->vkSampler, NULL);
    }
    
    // Free the sampler struct
    tknFree(pSampler);
}
