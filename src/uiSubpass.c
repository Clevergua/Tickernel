#include "uiSubpass.h"

void createUISubpass(Subpass *pUISubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t uiSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    createVkPipeline(pUISubpass, shadersPath, vkRenderPass, uiSubpassIndex, vkDevice, viewport, scissor);

    pUISubpass->vkDescriptorPoolSizeCount = 1;
    pUISubpass->vkDescriptorPoolSizes = tickernelMalloc(sizeof(VkDescriptorPoolSize) * pUISubpass->vkDescriptorPoolSizeCount);
    pUISubpass->vkDescriptorPoolSizes[0] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
    };

    tickernelCreateDynamicArray(&pUISubpass->modelDynamicArray, 1, sizeof(SubpassModel));
}

void destroyUISubpass(Subpass *pPostProcessSubpass, VkDevice vkDevice)
{
    destroyPostProcessSubpassModel(pPostProcessSubpass, vkDevice, 0);
    tickernelDestroyDynamicArray(&pPostProcessSubpass->modelDynamicArray);
    tickernelFree(pPostProcessSubpass->vkDescriptorPoolSizes);

    destroyVkPipeline(pPostProcessSubpass, vkDevice);
}
