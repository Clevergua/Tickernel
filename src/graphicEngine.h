#pragma once
#include <deferredRenderPass.h>

void StartGraphicEngine(GraphicEngine *pGraphicEngine);
void UpdateGraphicEngine(GraphicEngine *pGraphicEngine, bool *pCanUpdate);
void EndGraphicEngine(GraphicEngine *pGraphicEngine);