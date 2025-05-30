#include "postProcessSubpass.h"

void createPostProcessSubpass(Subpass *pPostProcessSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t postProcessSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkImageView colorVkImageView);
void destroyPostProcessSubpass(Subpass *pPostProcessSubpass, VkDevice vkDevice);
void recreatePostProcessSubpassModel(Subpass *pPostProcessSubpass, VkDevice vkDevice, VkImageView colorVkImageView);

void recordPostProcessSubpass(Subpass *pPostProcessSubpass, VkCommandBuffer vkCommandBuffer)
{
    Pipeline *pPostProcessPipeline = &pPostProcessSubpass->pipelines[0];
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPostProcessPipeline->vkPipeline);
    Material * pMaterial = pPostProcessPipeline->materialDynamicArray.array[0];
    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPostProcessPipeline->vkPipelineLayout, 0, 1, &pMaterial->vkDescriptorSet, 0, NULL);
    vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0);
}