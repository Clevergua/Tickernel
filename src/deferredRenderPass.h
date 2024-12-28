#pragma once
#include <opaqueGeometrySubpass.h>
#include <opaqueLightingSubpass.h>
#include <waterGeometrySubpass.h>
typedef struct DeferredRenderPassStruct
{
    VkRenderPass vkRenderPass;
    VkFramebuffer vkFramebuffer;

    Subpass opaqueGeometrySubpass;
    Subpass waterGeometrySubpass;
    Subpass opaqueLightingSubpass;
} DeferredRenderPass;

void CreateDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer);
void DestroyDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice);
void UpdateDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer);
void RecordDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice);
