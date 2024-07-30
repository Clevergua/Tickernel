#include <tickernelEngine.h>
#include <assert.h>
void TestFunction(int *a)
{
    int *b = a;
    printf("%p, %p", a, b);
}
int main(int argc, const char *argv[])
{
    TickernelEngine *pTickernelEngine = TKNMalloc(sizeof(TickernelEngine));
    *pTickernelEngine = (TickernelEngine){
        .targetFrameRate = 1,
        .frameCount = 4294967290,
        .canTick = true,
        .assetsPath = NULL,
        .pGFXEngine = NULL,
        .pLuaEngine = NULL,
    };

    RunTickernelEngine(pTickernelEngine);
    TKNFree(pTickernelEngine);

    getchar();
    return EXIT_SUCCESS;
}