#pragma once
#include <graphicEngineCore.h>

typedef struct LightingSubpassModelStruct
{
    uint32_t count;
} LightSubpassModel;

void CreateLightingSubpass(GraphicEngine *pGraphicEngine);
void DestroyLightingSubpass(GraphicEngine *pGraphicEngine);