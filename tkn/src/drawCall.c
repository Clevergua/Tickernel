#include "gfxCore.h"

DrawCall *addDrawCall(GfxContext *pGfxContext, Pipeline *pPipeline, Material *pMaterial, Mesh *pMesh, Instance *pInstance)
{
    DrawCall *pDrawCall = tknMalloc(sizeof(DrawCall));
    *pDrawCall = (DrawCall){
        .pPipeline = pPipeline,
        .pMaterial = pMaterial,
        .pInstance = pInstance,
        .pMesh = pMesh,
    };
    tknAddToHashSet(&pMaterial->drawCallPtrHashSet, &pDrawCall);
    tknAddToHashSet(&pInstance->drawCallPtrHashSet, &pDrawCall);
    tknAddToHashSet(&pMesh->drawCallPtrHashSet, &pDrawCall);
    tknAddToDynamicArray(&pPipeline->drawCallPtrDynamicArray, &pDrawCall);
    return pDrawCall;
}
void removeDrawCall(GfxContext *pGfxContext, DrawCall *pDrawCall)
{
    tknRemoveFromHashSet(&pDrawCall->pMaterial->drawCallPtrHashSet, &pDrawCall);
    tknRemoveFromHashSet(&pDrawCall->pInstance->drawCallPtrHashSet, &pDrawCall);
    tknRemoveFromHashSet(&pDrawCall->pMesh->drawCallPtrHashSet, &pDrawCall);
    tknRemoveFromDynamicArray(&pDrawCall->pPipeline->drawCallPtrDynamicArray, &pDrawCall);
    *pDrawCall = (DrawCall){0};
    tknFree(pDrawCall);
}
void clearDrawCalls(GfxContext *pGfxContext, Pipeline *pPipeline)
{
    for (uint32_t drawCallIndex = pPipeline->drawCallPtrDynamicArray.count - 1; drawCallIndex > -1; drawCallIndex--)
    {
        DrawCall *pDrawCall = *(DrawCall **)tknGetFromDynamicArray(&pPipeline->drawCallPtrDynamicArray, drawCallIndex);
        removeDrawCall(pGfxContext, pDrawCall);
    }
}