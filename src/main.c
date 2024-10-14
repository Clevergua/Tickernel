#include <tickernelEngine.h>
#include <assert.h>
#include <plySerializer.h>
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

    // char *plyPath = TickernelMalloc(sizeof(char) * FILENAME_MAX);
    // strcpy(plyPath, assetsPath);
    // TickernelCombinePaths(plyPath, FILENAME_MAX, "models");
    // TickernelCombinePaths(plyPath, FILENAME_MAX, "LargeBuilding01.ply");
    // PLYModel* pPLYModel = TickernelMalloc(sizeof(PLYModel));
    // DeserializePLYModel(plyPath, pPLYModel, TickernelMalloc);

    TickernelEngine *pTickernelEngine = TickernelMalloc(sizeof(TickernelEngine));
    *pTickernelEngine = (TickernelEngine){
        .targetFrameRate = 60,
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