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
    while (pPipeline->drawCallPtrDynamicArray.count > 0)
    {
        uint32_t lastIndex = pPipeline->drawCallPtrDynamicArray.count - 1;
        DrawCall *pDrawCall = *(DrawCall **)tknGetFromDynamicArray(&pPipeline->drawCallPtrDynamicArray, lastIndex);
        removeDrawCall(pGfxContext, pDrawCall);
    }
}