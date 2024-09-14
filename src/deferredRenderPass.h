#pragma once
#include <geometrySubpass.h>
#include <lightingSubpass.h>

void CreateDeferredRenderPass(GraphicEngine *pGraphicEngine);
void DestroyDeferredRenderPass(GraphicEngine *pGraphicEngine);
void RecordDeferredRenderPass(GraphicEngine *pGraphicEngine);