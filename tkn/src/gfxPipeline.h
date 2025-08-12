#pragma once
#include "gfxRes.h"

bool hasSwapchain(RenderPass renderPass);
void populateFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void cleanupFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
