#pragma once
#include <renderPasses/subpasses/opaqueGeometrySubpass.h>
#include <renderPasses/subpasses/opaqueLightingSubpass.h>

typedef struct DeferredRenderPassStruct
{
    VkRenderPass vkRenderPass;
    VkFramebuffer vkFramebuffer;

    Subpass opaqueGeometrySubpass;
    Subpass opaqueLightingSubpass;
} DeferredRenderPass;

void CreateDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer);
void DestroyDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice);
void UpdateDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer);
void RecordDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice);
