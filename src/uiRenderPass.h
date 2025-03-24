#pragma once
#include "uiSubpass.h"

typedef struct UIRenderPass
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    Subpass uiSubpass;
} UIRenderPass;
