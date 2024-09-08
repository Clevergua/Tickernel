#pragma once
#include <graphicEngineCore.h>
typedef struct DeferredRenderPipelineVertexStruct
{
    vec3 position;
    vec3 color;
} DeferredRenderPipelineVertex;

typedef struct ObjectUniformBufferObjectStruct
{
    mat4 model;
} ObjectUniformBufferObject;

void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void DestroyDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void RecordDeferredRenderPipeline(GraphicEngine *pGraphicEngine);