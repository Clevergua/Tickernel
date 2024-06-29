#include <tickernelEngine.h>

int main()
{
    getchar();
    GFXDevice gfxDevice = {
        .enableValidationLayers = true,
        .name = "Tickernel Engine",
        .targetPresentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR,
        .width = 1920,
        .height = 1080,
        .targetSwapchainImageCount = 3,
        .targetWaitFrameCount = 2,
        .maxCommandBufferListCount = 512,

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
        .commandBufferCount = 0,
        .commandBufferList = NULL,

        .acquiredImageIndex = -1,
        .imageAvailableSemaphores = NULL,
        .renderFinishedSemaphores = NULL,
        .renderFinishedFences = NULL,
        .waitFrameCount = 0,
        .frameCount = 0,
        .frameIndex = -1,
        .hasRecreateSwapchain = false,
    };
    TickernelEngine tickernelEngine = {
        .targetFrameRate = 1,
        .frameCount = 4294967295,
        // .frameCount = 0,
        .canTick = true,
        .assetsPath = "",
        .pGFXDevice = &gfxDevice,
    };
    // void *a = malloc(400000);
    RunTickernelEngine(&tickernelEngine);
    getchar();

    return EXIT_SUCCESS;
}