//#include "tickernelEngine.h"
//
//int main(int argc, const char *argv[])
//{
//    const char *pathseparator = TickernelGetPathSeparator();
//    char *assetsPath = TickernelMalloc(sizeof(char) * FILENAME_MAX);
//    strcpy(assetsPath, argv[0]);
//    size_t fileNameLength = strlen(assetsPath);
//    for (size_t i = fileNameLength - 1; i >= 0; i--)
//    {
//        if (assetsPath[i] == pathseparator[0])
//        {
//            assetsPath[i] = '\0';
//            break;
//        }
//        else
//        {
//            // continue;
//        }
//    }
//    TickernelCombinePaths(assetsPath, FILENAME_MAX, "assets");
//
//    TickernelEngine *pTickernelEngine = TickernelMalloc(sizeof(TickernelEngine));
//    *pTickernelEngine = (TickernelEngine)
//    {
//        .targetFrameRate = 60,
//        .frameCount = 4294967294,
//        // .frameCount = 0,
//        .canUpdate = true,
//        .pGraphicContext = NULL,
//        .pLuaContext = NULL,
//        .assetsPath = assetsPath,
//        .enableValidationLayers = true,
//        .applicationName = "Tickernel Engine",
//        .targetPresentMode = VK_PRESENT_MODE_FIFO_KHR,
//        .windowWidth = 256,
//        .windowHeight = 256,
//        .targetSwapchainImageCount = 2,
//    };
//    TickernelStart(pTickernelEngine);
//    while (pTickernelEngine->canUpdate && pTickernelEngine->frameCount < UINT32_MAX)
//    {
//        uint32_t millisecondsPerFrame = MILLISECONDS_PER_SECOND / pTickernelEngine->targetFrameRate;
//        struct timespec frameStartTime;
//        timespec_get(&frameStartTime, TIME_UTC);
//        TickernelUpdate(pTickernelEngine, &pTickernelEngine->canUpdate);
//        struct timespec frameEndTime;
//        timespec_get(&frameEndTime, TIME_UTC);
//        uint32_t deltaMilliseconds = (uint32_t)(frameEndTime.tv_sec - frameStartTime.tv_sec) * MILLISECONDS_PER_SECOND + (uint32_t)(frameEndTime.tv_nsec - frameStartTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
//        uint32_t sleepMilliseconds;
//        if (deltaMilliseconds < millisecondsPerFrame)
//        {
//            sleepMilliseconds = millisecondsPerFrame - deltaMilliseconds;
//        }
//        else
//        {
//            sleepMilliseconds = 0;
//        }
//        printf("Tickernel Tick!\n    Framecount: %u\n    TimeCost/TimePerFrame: %ums/%ums\n", pTickernelEngine->frameCount, deltaMilliseconds, millisecondsPerFrame);
//        TickernelSleep(sleepMilliseconds);
//        pTickernelEngine->frameCount++;
//    }
//
//    TickernelEnd(pTickernelEngine);Ï
//    TickernelFree(pTickernelEngine);
//    TickernelFree(assetsPath);
//    return EXIT_SUCCESS;
//}
