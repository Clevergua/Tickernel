#pragma once
#include <graphicEngineCore.h>

typedef struct LightingSubpassVertexStruct
{
    vec3 position;
} LightingSubpassVertex;

typedef struct LightingSubpassModelStruct
{
    uint32_t count;
    LightingSubpassVertex *lightingSubpassVertices;

} LightSubpassModel;

void CreateLightingSubpass(GraphicEngine *pGraphicEngine);
void DestroyLightingSubpass(GraphicEngine *pGraphicEngine);