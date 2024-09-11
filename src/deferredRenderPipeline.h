#pragma once
#include <graphicEngineCore.h>
typedef struct
{
    vec3 position;
    vec3 color;
} DeferredRenderPipelineVertex;

typedef struct
{
    mat4 model;
} ObjectUniformBufferObject;

typedef struct
{
    uint32_t vertexCount;
    DeferredRenderPipelineVertex *vertices;
} DeferredRenderPipelineObject;

void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void DestroyDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void RecordDeferredRenderPipeline(GraphicEngine *pGraphicEngine);