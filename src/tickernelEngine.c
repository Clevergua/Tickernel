#include <tickernelEngine.h>

#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000

static void TickernelStart(TickernelEngine *pTickernelEngine)
{
    printf("Tickernel Start!\n");

    pTickernelEngine->pGraphicEngine = TickernelMalloc(sizeof(GraphicEngine));
    GraphicEngine *pGraphicEngine = pTickernelEngine->pGraphicEngine;
    pGraphicEngine->enableValidationLayers = true;
    pGraphicEngine->name = "Tickernel Engine";
    pGraphicEngine->targetPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    pGraphicEngine->windowWidth = 256;
    pGraphicEngine->windowHeight = 256;
    pGraphicEngine->targetSwapchainImageCount = 2;
    pGraphicEngine->shadersPath = TickernelMalloc(sizeof(char) * FILENAME_MAX);
    pGraphicEngine->canUpdateGlobalUniformBuffer = false;
    pGraphicEngine->canUpdateLightsUniformBuffer = false;
    strcpy(pGraphicEngine->shadersPath, pTickernelEngine->assetsPath);
    TickernelCombinePaths(pGraphicEngine->shadersPath, FILENAME_MAX, "shaders");
    StartGraphicEngine(pGraphicEngine);

    pTickernelEngine->pLuaEngine = TickernelMalloc(sizeof(char) * sizeof(LuaEngine));
    LuaEngine *pLuaEngine = pTickernelEngine->pLuaEngine;
    pLuaEngine->pGraphicEngine = pTickernelEngine->pGraphicEngine;

    pLuaEngine->shadersPath = TickernelMalloc(FILENAME_MAX);
    strcpy(pLuaEngine->shadersPath, pTickernelEngine->assetsPath);
    StartLua(pLuaEngine);
}

static void TickernelUpdate(TickernelEngine *pTickernelEngine, bool *pCanUpdate)
{
    printf("Tickernel Update!\n");
    struct timespec startTime, endTime;
    timespec_get(&startTime, TIME_UTC);
    UpdateLua(pTickernelEngine->pLuaEngine);
    timespec_get(&endTime, TIME_UTC);
    uint32_t luaDeltaTime = (uint32_t)(endTime.tv_sec - startTime.tv_sec) * MILLISECONDS_PER_SECOND + (endTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    printf("Lua Cost Time: %u ms\n", luaDeltaTime);

    timespec_get(&startTime, TIME_UTC);
    UpdateGraphicEngine(pTickernelEngine->pGraphicEngine, pCanUpdate);
    timespec_get(&endTime, TIME_UTC);
    uint32_t graphicDeltaTime = (uint32_t)(endTime.tv_sec - startTime.tv_sec) * MILLISECONDS_PER_SECOND + (endTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    printf("Graphic Cost Time: %u ms\n", graphicDeltaTime);
}

static void TickernelEnd(TickernelEngine *pTickernelEngine)
{
    LuaEngine *pLuaEngine = pTickernelEngine->pLuaEngine;
    EndLua(pLuaEngine);
    TickernelFree(pLuaEngine->shadersPath);
    TickernelFree(pLuaEngine);
    GraphicEngine *pGraphicEngine = pTickernelEngine->pGraphicEngine;
    EndGraphicEngine(pGraphicEngine);
    TickernelFree(pGraphicEngine->shadersPath);
    TickernelFree(pGraphicEngine);
    printf("Tickernel End!\n");
}

void RunTickernelEngine(TickernelEngine *pTickernelEngine)
{
    TickernelStart(pTickernelEngine);
    while (pTickernelEngine->canUpdate && pTickernelEngine->frameCount < UINT32_MAX)
    {
        uint32_t millisecondsPerFrame = MILLISECONDS_PER_SECOND / pTickernelEngine->targetFrameRate;
        struct timespec frameStartTime;
        timespec_get(&frameStartTime, TIME_UTC);
        TickernelUpdate(pTickernelEngine, &pTickernelEngine->canUpdate);
        struct timespec frameEndTime;
        timespec_get(&frameEndTime, TIME_UTC);
        uint32_t deltaMilliseconds = (uint32_t)(frameEndTime.tv_sec - frameStartTime.tv_sec) * MILLISECONDS_PER_SECOND + (frameEndTime.tv_nsec - frameStartTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
        uint32_t sleepMilliseconds;
        if (deltaMilliseconds < millisecondsPerFrame)
        {
            sleepMilliseconds = millisecondsPerFrame - deltaMilliseconds;
        }
        else
        {
            sleepMilliseconds = 0;
        }
        printf("Tickernel Tick!\n    Framecount: %u\n    TimeCost/TimePerFrame: %ums/%ums\n", pTickernelEngine->frameCount, deltaMilliseconds, millisecondsPerFrame);
        TickernelSleep(sleepMilliseconds);
        pTickernelEngine->frameCount++;
    }

    TickernelEnd(pTickernelEngine);
}
