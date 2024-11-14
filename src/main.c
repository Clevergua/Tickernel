#include <tickernelEngine.h>

int main(int argc, const char *argv[])
{
    const char *pathseparator = TickernelGetPathSeparator();
    char *assetsPath = TickernelMalloc(sizeof(char) * FILENAME_MAX);
    strcpy(assetsPath, argv[0]);
    size_t fileNameLength = strlen(assetsPath);
    for (size_t i = fileNameLength - 1; i >= 0; i--)
    {
        if (assetsPath[i] == pathseparator[0])
        {
            assetsPath[i] = '\0';
            break;
        }
        else
        {
            // continue;
        }
    }
    TickernelCombinePaths(assetsPath, FILENAME_MAX, "assets");

    TickernelEngine *pTickernelEngine = TickernelMalloc(sizeof(TickernelEngine));
    *pTickernelEngine = (TickernelEngine){
        // .targetFrameRate = 60,
        .targetFrameRate = 1,
        // .frameCount = 4294967294,
        .frameCount = 0,
        .canUpdate = true,
        .pGraphicEngine = NULL,
        .pLuaEngine = NULL,
        .assetsPath = assetsPath,
    };
    RunTickernelEngine(pTickernelEngine);
    TickernelFree(pTickernelEngine);
    TickernelFree(assetsPath);
    return EXIT_SUCCESS;
}