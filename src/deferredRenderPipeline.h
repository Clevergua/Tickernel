#pragma once
#include <graphicEngineCore.h>
typedef struct DeferredRenderPipelineVertexStruct
{
    vec3 position;
    vec3 color;
} DeferredRenderPipelineVertex;
void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void DestroyDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void RecordDeferredRenderPipeline(GraphicEngine *pGraphicEngine);