#include <tickernelEngine.h>
#include <time.h>

#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000

static void TickernelStart(TickernelEngine *pTickernelEngine)
{
    printf("Tickernel Start!\n");
    pTickernelEngine->assetsPath = TKNMalloc(FILENAME_MAX);
    TKNGetCurrentDirectory(pTickernelEngine->assetsPath, FILENAME_MAX);
    TKNCombinePaths(pTickernelEngine->assetsPath, FILENAME_MAX, "assets", NULL);

    StartGFXEngine(pTickernelEngine->pGFXEngine);
    StartLua(pTickernelEngine->pLuaEngine, pTickernelEngine->assetsPath);
}

static void TickernelUpdate(TickernelEngine *pTickernelEngine)
{
    printf("Tickernel Update!\n");
    UpdateLua(pTickernelEngine->pLuaEngine);
    UpdateGFXEngine(pTickernelEngine->pGFXEngine);
}

static void TickernelEnd(TickernelEngine *pTickernelEngine)
{
    EndLua(pTickernelEngine->pLuaEngine);
    EndGFXEngine(pTickernelEngine->pGFXEngine);
    TKNFree(pTickernelEngine->assetsPath);
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
        printf("Tickernel Tick!\n    Framecount: %u\n    TimeCost/Total: %ums/%ums\n", pTickernelEngine->frameCount, deltaMilliseconds, millisecondsPerFrame);
        TKNSleep(sleepMilliseconds);
        pTickernelEngine->frameCount++;
    }
    TickernelEnd(pTickernelEngine);
}