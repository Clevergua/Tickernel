#include <tickernelEngine.h>
#include <tickernelCore.h>

#define MILLISECONDS_PER_SECOND 1000

void RunTickernelEngine()
{
    printf("Tickernel Engine Start!\n");
    uint32_t tickernelFrameCount = 0;
    bool canTick = true;
    uint32_t targetFrameRate = 60;
    uint32_t millisecondsPerFrame = MILLISECONDS_PER_SECOND / targetFrameRate;
    printf("%d\n", millisecondsPerFrame);
    while (canTick && tickernelFrameCount < UINT32_MAX)
    {
        TickernelSleep(1000);
        tickernelFrameCount++;
        printf("Tickernel Engine Tick! Framecount: %u\n", tickernelFrameCount);
    }
    printf("Tickernel Engine End!\n");
}