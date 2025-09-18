#include "gfxCore.h"

DrawCall *createDrawCallPtr(GfxContext *pGfxContext, Pipeline *pPipeline, Material *pMaterial, Mesh *pMesh, Instance *pInstance)
{
    // Validate DrawCall compatibility with Pipeline
    if (pPipeline->pMeshVertexInputLayout != NULL)
    {
        tknAssert(pMesh != NULL, "DrawCall must have a Mesh when Pipeline requires mesh vertex input layout");
        tknAssert(pMesh->pVertexInputLayout == pPipeline->pMeshVertexInputLayout, "DrawCall Mesh vertex input layout must match Pipeline mesh vertex input layout");
    }
    else
    {
        tknAssert(pMesh == NULL, "DrawCall must not have a Mesh when Pipeline does not require mesh vertex input layout");
    }

    if (pPipeline->pInstanceVertexInputLayout != NULL)
    {
        tknAssert(pInstance != NULL, "DrawCall must have an Instance when Pipeline requires instance vertex input layout");
        tknAssert(pInstance->pVertexInputLayout == pPipeline->pInstanceVertexInputLayout, "DrawCall Instance vertex input layout must match Pipeline instance vertex input layout");
    }
    else
    {
        tknAssert(pInstance == NULL, "DrawCall must not have an Instance when Pipeline does not require instance vertex input layout");
    }

    // Validate Material compatibility
    tknAssert(pMaterial != NULL, "DrawCall must have a Material");
    tknAssert(pMaterial->pDescriptorSet == pPipeline->pPipelineDescriptorSet, "DrawCall Material descriptor set must match Pipeline descriptor set");

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

    // Add to pipeline hashset only (dynamic array management is handled by addDrawCallToPipeline)
    tknAddToHashSet(&pPipeline->drawCallPtrHashSet, &pDrawCall);

    return pDrawCall;
}

void destroyDrawCallPtr(GfxContext *pGfxContext, DrawCall *pDrawCall)
{
    // Remove from pipeline if associated
    if (pDrawCall->pPipeline != NULL)
    {
        tknRemoveFromDynamicArray(&pDrawCall->pPipeline->drawCallPtrDynamicArray, &pDrawCall);
        tknRemoveFromHashSet(&pDrawCall->pPipeline->drawCallPtrHashSet, &pDrawCall);
    }
    if (pDrawCall->pMaterial != NULL)
        tknRemoveFromHashSet(&pDrawCall->pMaterial->drawCallPtrHashSet, &pDrawCall);
    if (pDrawCall->pInstance != NULL)
        tknRemoveFromHashSet(&pDrawCall->pInstance->drawCallPtrHashSet, &pDrawCall);
    if (pDrawCall->pMesh != NULL)
        tknRemoveFromHashSet(&pDrawCall->pMesh->drawCallPtrHashSet, &pDrawCall);
    *pDrawCall = (DrawCall){0};
    tknFree(pDrawCall);
}

void insertDrawCallPtr(DrawCall *pDrawCall, uint32_t index)
{
    tknAssert(pDrawCall->pPipeline != NULL, "DrawCall must be associated with a Pipeline");
    // Remove from current position in dynamic array and insert at new position
    tknRemoveFromDynamicArray(&pDrawCall->pPipeline->drawCallPtrDynamicArray, &pDrawCall);
    tknInsertIntoDynamicArray(&pDrawCall->pPipeline->drawCallPtrDynamicArray, &pDrawCall, index);
}

void removeDrawCallPtr(DrawCall *pDrawCall)
{
    tknAssert(pDrawCall->pPipeline != NULL, "DrawCall must be associated with a Pipeline");
    tknRemoveFromDynamicArray(&pDrawCall->pPipeline->drawCallPtrDynamicArray, &pDrawCall);
    // Keep pipeline reference for hashset relationship tracking
}

void removeDrawCallAtIndex(Pipeline *pPipeline, uint32_t index)
{
    if (index >= pPipeline->drawCallPtrDynamicArray.count)
    {
        return;
    }
    DrawCall *pDrawCall = *(DrawCall **)tknGetFromDynamicArray(&pPipeline->drawCallPtrDynamicArray, index);
    destroyDrawCallPtr(NULL, pDrawCall);
}
DrawCall *getDrawCallAtIndex(Pipeline *pPipeline, uint32_t index)
{
    if (index >= pPipeline->drawCallPtrDynamicArray.count)
    {
        return NULL;
    }
    return *(DrawCall **)tknGetFromDynamicArray(&pPipeline->drawCallPtrDynamicArray, index);
}
uint32_t getDrawCallCount(Pipeline *pPipeline)
{
    return pPipeline->drawCallPtrDynamicArray.count;
}