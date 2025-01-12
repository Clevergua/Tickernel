#pragma once
#include "luaBinding.h"
typedef struct TickernelEngineStruct
{
    int targetFrameRate;
    uint32_t frameCount;
    bool canUpdate;
    char *assetsPath;

    bool enableValidationLayers;
    char *applicationName;
    int windowHeight;
    int windowWidth;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;

    GraphicContext *pGraphicContext;
    LuaContext *pLuaContext;
} TickernelEngine;

void TickernelStart(TickernelEngine *pTickernelEngine);
void TickernelUpdate(TickernelEngine *pTickernelEngine);
void TickernelEnd(TickernelEngine *pTickernelEngine);
