#pragma once
#include <graphicEngineCore.h>

void CreateLightingSubpass(RenderPass *pDeferredRenderPass);
void DestroyLightingSubpass(RenderPass *pDeferredRenderPass);
void RecreateLightingSubpassModel(RenderPass *pDeferredRenderPass);