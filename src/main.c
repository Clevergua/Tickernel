#include <tickernelEngine.h>
#include <assert.h>
int main()
{
    getchar();
    GFXEngine gfxEngine = {
        // Config:
        .enableValidationLayers = true,
        .name = "Tickernel Engine",
        .targetPresentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR,
        .width = 1920,
        .height = 1080,
        .targetSwapchainImageCount = 3,
        .targetWaitFrameCount = 2,
        .maxCommandBufferListCount = 512,
        // Runtime:
        .pGLFWWindow = NULL,
        .vkInstance = NULL,
        .vkSurface = NULL,
        .vkPhysicalDevice = NULL,
        .graphicQueueFamilyIndex = -1,
        .presentQueueFamilyIndex = -1,
        .vkDevice = NULL,
        .vkGraphicQueue = NULL,
        .vkPresentQueue = NULL,
        .vkSwapchain = NULL,
        .surfaceFormat = 0,
        .swapchainExtent = 0,
        .swapchainImageCount = -1,
        .swapchainImages = NULL,
        .swapchainImageViews = NULL,
        // .vkRenderPass = NULL,

        .depthImage = NULL,
        .depthFormat = 0,
        .depthImageView = NULL,
        .depthImageMemory = NULL,
        // .vkFramebuffers = NULL,
        .vkCommandPools = NULL,
        .vkCommandBufferCount = 0,
        .vkCommandBuffers = NULL,

        .acquiredImageIndex = -1,
        .imageAvailableSemaphores = NULL,
        .renderFinishedSemaphores = NULL,
        .renderFinishedFences = NULL,
        .frameCount = 0,
        .frameIndex = -1,
    };

    LuaEngine luaEngine = {
        .pLuaState = NULL,
        .luaAssetsPath = NULL,
    };

    TickernelEngine tickernelEngine = {
        .targetFrameRate = 1,
        .frameCount = 4294967290,
        // .frameCount = 0,
        .canTick = true,
        .assetsPath = NULL,
        .pGFXEngine = &gfxEngine,
        .pLuaEngine = &luaEngine,
    };
    RunTickernelEngine(&tickernelEngine);
    getchar();

    return EXIT_SUCCESS;
}