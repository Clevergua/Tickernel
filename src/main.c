#include <tickernelEngine.h>
#include <assert.h>
void TestFunction(int *a)
{
    int *b = a;
    printf("%p, %p", a, b);
}
int main(int argc, const char *argv[])
{
    TickernelEngine *pTickernelEngine = TickernelMalloc(sizeof(TickernelEngine));
    *pTickernelEngine = (TickernelEngine){
        .targetFrameRate = 1,
        .frameCount = 4294967290,
        .canTick = true,
        .assetsPath = NULL,
        .pGraphicEngine = NULL,
        .pLuaEngine = NULL,
    };

    RunTickernelEngine(pTickernelEngine);
    TickernelFree(pTickernelEngine);

    getchar();
    return EXIT_SUCCESS;
}