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
    pGraphicEngine->windowWidth = 512;
    pGraphicEngine->windowHeight = 512;
    pGraphicEngine->targetSwapchainImageCount = 2;
    pGraphicEngine->assetsPath = TickernelMalloc(sizeof(char) * FILENAME_MAX);
    strcpy(pGraphicEngine->assetsPath, pTickernelEngine->assetsPath);
    
    StartGraphicEngine(pGraphicEngine);

    pTickernelEngine->pLuaEngine = TickernelMalloc(sizeof(char) * sizeof(LuaEngine));
    LuaEngine *pLuaEngine = pTickernelEngine->pLuaEngine;
    pLuaEngine->pGraphicEngine = pTickernelEngine->pGraphicEngine;

    pLuaEngine->assetsPath = TickernelMalloc(FILENAME_MAX);
    strcpy(pLuaEngine->assetsPath, pTickernelEngine->assetsPath);
    StartLua(pLuaEngine);
}

static void TickernelUpdate(TickernelEngine *pTickernelEngine, bool *pCanUpdate)
{
    printf("Tickernel Update!\n");
    UpdateLua(pTickernelEngine->pLuaEngine);
    UpdateGraphicEngine(pTickernelEngine->pGraphicEngine, pCanUpdate);
}

static void TickernelEnd(TickernelEngine *pTickernelEngine)
{
    LuaEngine *pLuaEngine = pTickernelEngine->pLuaEngine;
    EndLua(pLuaEngine);
    TickernelFree(pLuaEngine->assetsPath);
    TickernelFree(pLuaEngine);
    GraphicEngine *pGraphicEngine = pTickernelEngine->pGraphicEngine;
    EndGraphicEngine(pGraphicEngine);
    TickernelFree(pGraphicEngine->assetsPath);
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
        uint32_t deltaMilliseconds = (frameEndTime.tv_sec - frameStartTime.tv_sec) * MILLISECONDS_PER_SECOND + (frameEndTime.tv_nsec - frameStartTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
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