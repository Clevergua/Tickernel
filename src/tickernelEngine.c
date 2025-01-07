#include <tickernelEngine.h>

#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000

static void TickernelStart(TickernelEngine *pTickernelEngine)
{
    printf("Tickernel Start!\n");

    pTickernelEngine->pGraphicContext = TickernelMalloc(sizeof(GraphicContext));
    GraphicContext *pGraphicContext = pTickernelEngine->pGraphicContext;
    pGraphicContext->enableValidationLayers = pTickernelEngine->enableValidationLayers;
    pGraphicContext->applicationName = pTickernelEngine->applicationName;
    pGraphicContext->targetPresentMode = pTickernelEngine->targetPresentMode;
    pGraphicContext->windowWidth = pTickernelEngine->windowWidth;
    pGraphicContext->windowHeight = pTickernelEngine->windowHeight;
    pGraphicContext->targetSwapchainImageCount = pTickernelEngine->targetSwapchainImageCount;
    pGraphicContext->shadersPath = TickernelMalloc(sizeof(char) * FILENAME_MAX);
    strcpy(pGraphicContext->shadersPath, pTickernelEngine->assetsPath);
    TickernelCombinePaths(pGraphicContext->shadersPath, FILENAME_MAX, "shaders");
    StartGraphic(pGraphicContext);

    pTickernelEngine->pLuaContext = TickernelMalloc(sizeof(char) * sizeof(LuaContext));
    LuaContext *pLuaContext = pTickernelEngine->pLuaContext;
    pLuaContext->pGraphicContext = pTickernelEngine->pGraphicContext;

    pLuaContext->assetPath = TickernelMalloc(FILENAME_MAX);
    strcpy(pLuaContext->assetPath, pTickernelEngine->assetsPath);
    StartLua(pLuaContext);
}

static void TickernelUpdate(TickernelEngine *pTickernelEngine, bool *pCanUpdate)
{
    printf("Tickernel Update!\n");
    struct timespec startTime, endTime;
    timespec_get(&startTime, TIME_UTC);
    UpdateLua(pTickernelEngine->pLuaContext);
    timespec_get(&endTime, TIME_UTC);
    uint32_t luaDeltaTime = (uint32_t)(endTime.tv_sec - startTime.tv_sec) * MILLISECONDS_PER_SECOND + (endTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    printf("Lua Cost Time: %u ms\n", luaDeltaTime);

    timespec_get(&startTime, TIME_UTC);
    UpdateGraphic(pTickernelEngine->pGraphicContext, pCanUpdate);
    timespec_get(&endTime, TIME_UTC);
    uint32_t graphicDeltaTime = (uint32_t)(endTime.tv_sec - startTime.tv_sec) * MILLISECONDS_PER_SECOND + (endTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    printf("Graphic Cost Time: %u ms\n", graphicDeltaTime);
}

static void TickernelEnd(TickernelEngine *pTickernelEngine)
{
    LuaContext *pLuaContext = pTickernelEngine->pLuaContext;
    EndLua(pLuaContext);
    TickernelFree(pLuaContext->assetPath);
    TickernelFree(pLuaContext);
    GraphicContext *pGraphicContext = pTickernelEngine->pGraphicContext;
    EndGraphic(pGraphicContext);
    TickernelFree(pGraphicContext->shadersPath);
    TickernelFree(pGraphicContext);
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
