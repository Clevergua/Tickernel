#pragma once
#include <geometrySubpass.h>
#include <lightingSubpass.h>

void CreateDeferredRenderPass(RenderPass *pDeferredRenderPass);
void DestroyDeferredRenderPass(RenderPass *pDeferredRenderPass);
void RecordDeferredRenderPass(RenderPass *pDeferredRenderPass, VkCommandBuffer vkCommandBuffer);