#pragma once
#include "gfxResource.h"

void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void cleanupFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void repopulateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);