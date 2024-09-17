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
void AddModelToLightingSubpass(GraphicEngine *pGraphicEngine, uint32_t vertexCount, LightingSubpassVertex *lightingSubpassVertices, uint32_t *pGroupIndex, uint32_t *pModelIndex);
void RemoveModelFromLightingSubpass(GraphicEngine *pGraphicEngine, uint32_t groupIndex, uint32_t modelIndex);