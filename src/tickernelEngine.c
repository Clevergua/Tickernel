#include <tickernelEngine.h>
#include <windows.h>
#include <tickernelIO.h>
void TickTickernelEngine()
{
    uint32_t tickernelFrameCount = 0;

    while (tickernelFrameCount < UINT32_MAX)
    {
        Sleep(1000);
        tickernelFrameCount++;
        printf("TickTickernelEngine %u\n", tickernelFrameCount);
    }
    Log("TickTickernelEngine %u\n", tickernelFrameCount);
}
