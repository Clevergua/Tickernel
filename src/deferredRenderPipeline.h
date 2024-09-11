#pragma once
#include <graphicEngineCore.h>
typedef struct DeferredRenderPipelineVertexStruct
{
    vec3 position;
    vec3 color;
} DeferredRenderPipelineVertex;

typedef struct DeferredRenderPipelineObjectUniformBufferStruct
{
    mat4 model;
} DeferredRenderPipelineObjectUniformBuffer;

typedef struct DeferredRenderPipelineObjectStruct
{
    uint32_t vertexCount;
    DeferredRenderPipelineVertex *vertices;
} DeferredRenderPipelineObject;

void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void DestroyDeferredRenderPipeline(GraphicEngine *pGraphicEngine);
void RecordDeferredRenderPipeline(GraphicEngine *pGraphicEngine);