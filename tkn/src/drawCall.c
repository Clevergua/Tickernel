#include "gfxCore.h"

DrawCall *addDrawCallPtr(GfxContext *pGfxContext, Pipeline *pPipeline, Material *pMaterial, Mesh *pMesh, Instance *pInstance)
{
    DrawCall *pDrawCall = tknMalloc(sizeof(DrawCall));
    *pDrawCall = (DrawCall){
        .pPipeline = pPipeline,
        .pMaterial = pMaterial,
        .pInstance = pInstance,
        .pMesh = pMesh,
    };
    if (pMaterial != NULL)
        tknAddToHashSet(&pMaterial->drawCallPtrHashSet, &pDrawCall);
    if (pInstance != NULL)
        tknAddToHashSet(&pInstance->drawCallPtrHashSet, &pDrawCall);
    if (pMesh != NULL)
        tknAddToHashSet(&pMesh->drawCallPtrHashSet, &pDrawCall);
    tknAddToDynamicArray(&pPipeline->drawCallPtrDynamicArray, &pDrawCall);
    return pDrawCall;
}
void removeDrawCallPtr(GfxContext *pGfxContext, DrawCall *pDrawCall)
{
    if (pDrawCall->pMaterial != NULL)
        tknRemoveFromHashSet(&pDrawCall->pMaterial->drawCallPtrHashSet, &pDrawCall);
    if (pDrawCall->pInstance != NULL)
        tknRemoveFromHashSet(&pDrawCall->pInstance->drawCallPtrHashSet, &pDrawCall);
    if (pDrawCall->pMesh != NULL)
        tknRemoveFromHashSet(&pDrawCall->pMesh->drawCallPtrHashSet, &pDrawCall);
    tknRemoveFromDynamicArray(&pDrawCall->pPipeline->drawCallPtrDynamicArray, &pDrawCall);
    *pDrawCall = (DrawCall){0};
    tknFree(pDrawCall);
}
void clearDrawCalls(GfxContext *pGfxContext, Pipeline *pPipeline)
{
    while (pPipeline->drawCallPtrDynamicArray.count > 0)
    {
        uint32_t lastIndex = pPipeline->drawCallPtrDynamicArray.count - 1;
        DrawCall *pDrawCall = *(DrawCall **)tknGetFromDynamicArray(&pPipeline->drawCallPtrDynamicArray, lastIndex);
        removeDrawCallPtr(pGfxContext, pDrawCall);
    }
}
