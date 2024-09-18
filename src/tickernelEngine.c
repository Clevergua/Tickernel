#include <tickernelEngine.h>
#include <time.h>

#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000

static void TickernelStart(TickernelEngine *pTickernelEngine)
{
    printf("Tickernel Start!\n");
    pTickernelEngine->pGraphicEngine = TickernelMalloc(sizeof(GraphicEngine));
    pTickernelEngine->pGraphicEngine->enableValidationLayers = true;
    pTickernelEngine->pGraphicEngine->name = "Tickernel Engine";
    pTickernelEngine->pGraphicEngine->targetPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    pTickernelEngine->pGraphicEngine->width = 1920;
    pTickernelEngine->pGraphicEngine->height = 1080;
    pTickernelEngine->pGraphicEngine->targetSwapchainImageCount = 2;

    pTickernelEngine->pLuaEngine = TickernelMalloc(sizeof(LuaEngine));

    pTickernelEngine->assetsPath = TickernelMalloc(FILENAME_MAX);
    TickernelGetCurrentDirectory(pTickernelEngine->assetsPath, FILENAME_MAX);
    TickernelCombinePaths(pTickernelEngine->assetsPath, FILENAME_MAX, "assets");

    StartGraphicEngine(pTickernelEngine->pGraphicEngine);
    StartLua(pTickernelEngine->pLuaEngine, pTickernelEngine->assetsPath);
}

static void TickernelUpdate(TickernelEngine *pTickernelEngine)
{
    printf("Tickernel Update!\n");
    UpdateLua(pTickernelEngine->pLuaEngine);
    UpdateGraphicEngine(pTickernelEngine->pGraphicEngine);
}

static void TickernelEnd(TickernelEngine *pTickernelEngine)
{
    // EndLua(pTickernelEngine->pLuaEngine);
    EndGraphicEngine(pTickernelEngine->pGraphicEngine);

    TickernelFree(pTickernelEngine->assetsPath);
    TickernelFree(pTickernelEngine->pLuaEngine);
    TickernelFree(pTickernelEngine->pGraphicEngine);
    printf("Tickernel End!\n");
}

void RunTickernelEngine(TickernelEngine *pTickernelEngine)
{
    TickernelStart(pTickernelEngine);
    while (pTickernelEngine->canTick && pTickernelEngine->frameCount < UINT32_MAX)
    {
        uint32_t millisecondsPerFrame = MILLISECONDS_PER_SECOND / pTickernelEngine->targetFrameRate;
        struct timespec frameStartTime;
        timespec_get(&frameStartTime, TIME_UTC);
        TickernelUpdate(pTickernelEngine);
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