#pragma once
#include <postProcessSubpass.h>

typedef struct PostProcessRenderPassStruct
{
    char *shadersPath;
    VkRenderPass vkRenderPass;
    VkFramebuffer vkFramebuffer;
    Subpass postProcessSubpass;
} PostProcessRenderPass;

void CreatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, VkViewport viewport, VkRect2D scissor);
void DestroyPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice);
void UpdatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage,  uint32_t width, uint32_t height);
void RecordPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice);
