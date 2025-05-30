#pragma once
#include "geometrySubpass.h"
#include "lightingSubpass.h"
#include "postProcessSubpass.h"

typedef struct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;

    Subpass geometrySubpass;
    Subpass lightingSubpass;
    Subpass postProcessSubpass;
} DeferredRenderPass;