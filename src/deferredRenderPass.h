#pragma once
#include "graphicCore.h"

typedef struct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;

    Subpass geometrySubpass;
    Subpass lightingSubpass;
    Subpass postProcessSubpass;
} DeferredRenderPass;