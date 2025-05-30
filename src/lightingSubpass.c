#include "lightingSubpass.h"

void createLightingSubpass(Subpass *pLightingSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t lightingSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);
void destroyLightingSubpass(Subpass *pLightingSubpass, VkDevice vkDevice);
void recreateLightingSubpassModel(Subpass *pLightingSubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);

void recordLightingSubpass(Subpass *pLightingSubpass, VkCommandBuffer vkCommandBuffer)
{
    Pipeline *pLightingPipeline = &pLightingSubpass->pipelines[0];
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pLightingPipeline->vkPipeline);
    Material *pMaterial = pLightingPipeline->materialDynamicArray.array[0];
    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pLightingPipeline->vkPipelineLayout, 0, 1, &pMaterial->vkDescriptorSet, 0, NULL);
    vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0);
}