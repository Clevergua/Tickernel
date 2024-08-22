#pragma once
#include <graphicEngineCore.h>

typedef struct DeferredRenderPipelineStruct
{
    VkRenderPass vkRenderPass;
} DeferredRenderPipeline;
void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, DeferredRenderPipeline *pDeferredRenderPipeline);
void DestroyDeferredRenderPipeline(GraphicEngine *pGraphicEngine, DeferredRenderPipeline *pDeferredRenderPipeline);