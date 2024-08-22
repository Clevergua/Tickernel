#pragma once
#include <graphicEngineCore.h>

void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage);
void DestroyDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void RecordDeferredRenderPipeline(GraphicEngine *pGraphicEngine);