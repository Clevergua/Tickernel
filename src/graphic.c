#include "graphic.h"

static void hasAllRequiredExtensions(GraphicContext *pGraphicContext, VkPhysicalDevice vkPhysicalDevice, bool *pHasAllRequiredExtensions)
{
    VkResult result = VK_SUCCESS;
    char *requiredExtensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    uint32_t requiredExtensionCount = ARRAY_SIZE(requiredExtensionNames);

    uint32_t extensionCount = 0;
    result = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, NULL);
    tryThrowVulkanError(result);
    VkExtensionProperties *extensionProperties = tickernelMalloc(extensionCount * sizeof(VkExtensionProperties));
    result = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, extensionProperties);
    tryThrowVulkanError(result);

    uint32_t foundCount = 0;
    for (uint32_t i = 0; i < extensionCount; i++)
    {
        char *supportExtensionName = extensionProperties[i].extensionName;
        for (uint32_t j = 0; j < requiredExtensionCount; j++)
        {
            char *requiredExtensionName = requiredExtensionNames[j];
            if (0 == strcmp(supportExtensionName, requiredExtensionName))
            {
                foundCount++;
                break;
            }
            else
            {
                // continue;
            }
        }
    }
    *pHasAllRequiredExtensions = foundCount == requiredExtensionCount;
    tickernelFree(extensionProperties);
}

static void hasGraphicsAndPresentQueueFamilies(GraphicContext *pGraphicContext, VkPhysicalDevice vkPhysicalDevice, bool *pHasGraphicsAndPresentQueueFamilies, uint32_t *pGraphicsQueueFamilyIndex, uint32_t *pPresentQueueFamilyIndex)
{

    VkSurfaceKHR vkSurface = pGraphicContext->vkSurface;
    uint32_t *pQueueFamilyPropertyCount = &pGraphicContext->queueFamilyPropertyCount;

    VkResult result = VK_SUCCESS;

    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, pQueueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *vkQueueFamilyPropertiesList = tickernelMalloc(*pQueueFamilyPropertyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, pQueueFamilyPropertyCount, vkQueueFamilyPropertiesList);

    int graphicIndexCount = 0;
    int presentIndexCount = 0;
    *pHasGraphicsAndPresentQueueFamilies = false;
    for (uint32_t i = 0; i < *pQueueFamilyPropertyCount; i++)
    {
        VkQueueFamilyProperties vkQueueFamilyProperties = vkQueueFamilyPropertiesList[i];
        if (vkQueueFamilyProperties.queueCount > 0 && vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            *pGraphicsQueueFamilyIndex = i;
            graphicIndexCount++;
        }
        else
        {
            // continue;
        }
        VkBool32 presentSupport = VK_FALSE;
        result = vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurface, &presentSupport);
        tryThrowVulkanError(result);
        if (vkQueueFamilyProperties.queueCount > 0 && presentSupport)
        {
            *pPresentQueueFamilyIndex = i;
            presentIndexCount++;
        }
        else
        {
            // continue;
        }

        if (graphicIndexCount > 0 && presentIndexCount > 0)
        {
            *pHasGraphicsAndPresentQueueFamilies = true;
            break;
        }
    }
    tickernelFree(vkQueueFamilyPropertiesList);
}

static void pickPhysicalDevice(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    uint32_t deviceCount = -1;
    result = vkEnumeratePhysicalDevices(pGraphicContext->vkInstance, &deviceCount, NULL);
    tryThrowVulkanError(result);

    if (deviceCount <= 0)
    {
        printf("failed to find GPUs with Vulkan support!");
    }
    else
    {
        VkPhysicalDevice *devices = tickernelMalloc(deviceCount * sizeof(VkPhysicalDevice));
        result = vkEnumeratePhysicalDevices(pGraphicContext->vkInstance, &deviceCount, devices);
        tryThrowVulkanError(result);

        uint32_t graphicQueueFamilyIndex = -1;
        uint32_t presentQueueFamilyIndex = -1;
        VkPhysicalDevice targetDevice = NULL;
        uint32_t maxScore = 0;
        char *targetDeviceName = NULL;

        VkSurfaceKHR vkSurface = pGraphicContext->vkSurface;
        for (uint32_t i = 0; i < deviceCount; i++)
        {
            VkPhysicalDevice vkPhysicalDevice = devices[i];

            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

            bool hasExtensions;
            hasAllRequiredExtensions(pGraphicContext, vkPhysicalDevice, &hasExtensions);
            tryThrowVulkanError(result);
            bool hasQueueFamilies;
            hasGraphicsAndPresentQueueFamilies(pGraphicContext, vkPhysicalDevice, &hasQueueFamilies, &graphicQueueFamilyIndex, &presentQueueFamilyIndex);

            uint32_t surfaceFormatCount;
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, NULL);
            tryThrowVulkanError(result);
            bool hasSurfaceFormat = surfaceFormatCount > 0;
            uint32_t presentModeCount;
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, NULL);
            tryThrowVulkanError(result);
            bool hasPresentMode = presentModeCount > 0;

            VkFormatProperties formatProps;
            vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, VK_FORMAT_ASTC_4x4_UNORM_BLOCK, &formatProps);
            if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
            {
                // 设备不支持ASTC格式
            }
            if (hasExtensions && hasQueueFamilies && hasSurfaceFormat && hasPresentMode)
            {
                uint32_t formatCount = 0;
                uint32_t modeCount = 0;
                result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &formatCount, NULL);
                tryThrowVulkanError(result);
                result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &modeCount, NULL);
                tryThrowVulkanError(result);

                if (formatCount > 0 && modeCount > 0)
                {
                    uint32_t score = 0;
                    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                    {
                        // nothing
                    }
                    else
                    {
                        score += 10;
                    }
                    if (score >= maxScore)
                    {
                        maxScore = score;
                        targetDevice = vkPhysicalDevice;
                        targetDeviceName = deviceProperties.deviceName;
                    }
                    else
                    {
                        // continue
                    }
                }
                else
                {
                    // continue;
                }
            }
            else
            {
                // Dont select this device
            }
        }
        tickernelFree(devices);
        if (targetDevice != NULL)
        {
            pGraphicContext->vkPhysicalDevice = targetDevice;
            pGraphicContext->graphicQueueFamilyIndex = graphicQueueFamilyIndex;
            pGraphicContext->presentQueueFamilyIndex = presentQueueFamilyIndex;
            printf("Selected target physical device named %s\n", targetDeviceName);
        }
        else
        {
            printf("failed to find GPUs with Vulkan support!");
        }
    }
}

static void createLogicalDevice(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;

    VkPhysicalDevice vkPhysicalDevice = pGraphicContext->vkPhysicalDevice;
    uint32_t graphicQueueFamilyIndex = pGraphicContext->graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGraphicContext->presentQueueFamilyIndex;
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo *queueCreateInfos;
    uint32_t queueCount;
    if (graphicQueueFamilyIndex == presentQueueFamilyIndex)
    {
        queueCount = 1;
        queueCreateInfos = tickernelMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
        VkDeviceQueueCreateInfo graphicCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = graphicQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
        queueCreateInfos[0] = graphicCreateInfo;
    }
    else
    {
        queueCount = 2;
        queueCreateInfos = tickernelMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
        VkDeviceQueueCreateInfo graphicCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = graphicQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
        queueCreateInfos[0] = graphicCreateInfo;

        VkDeviceQueueCreateInfo presentCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .queueFamilyIndex = presentQueueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        };
        queueCreateInfos[1] = presentCreateInfo;
    }

    VkPhysicalDeviceFeatures deviceFeatures =
        {
            .fillModeNonSolid = VK_TRUE,
            .sampleRateShading = VK_TRUE,
        };
    char **enabledLayerNames = NULL;
    uint32_t enabledLayerCount = 0;

    char *extensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_KHR_portability_subset",
        //        "VK_EXT_debug_utils"
    };
    uint32_t extensionCount = ARRAY_SIZE(extensionNames);
    VkDeviceCreateInfo vkDeviceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .queueCreateInfoCount = queueCount,
            .pQueueCreateInfos = queueCreateInfos,
            .enabledLayerCount = enabledLayerCount,
            .ppEnabledLayerNames = (const char *const *)enabledLayerNames,
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = (const char *const *)extensionNames,
            .pEnabledFeatures = &deviceFeatures,
        };
    result = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, NULL, &pGraphicContext->vkDevice);
    tryThrowVulkanError(result);
    vkGetDeviceQueue(pGraphicContext->vkDevice, graphicQueueFamilyIndex, 0, &pGraphicContext->vkGraphicQueue);
    vkGetDeviceQueue(pGraphicContext->vkDevice, presentQueueFamilyIndex, 0, &pGraphicContext->vkPresentQueue);
    tickernelFree(queueCreateInfos);
}

static void destroyLogicalDevice(GraphicContext *pGraphicContext)
{
    vkDestroyDevice(pGraphicContext->vkDevice, NULL);
}

static void chooseSurfaceFormat(VkSurfaceFormatKHR *surfaceFormats, uint32_t surfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormat)
{
    for (uint32_t i = 0; i < surfaceFormatCount; i++)
    {
        // Return srgb & nonlinears
        VkSurfaceFormatKHR surfaceFormat = surfaceFormats[i];
        if (VK_FORMAT_B8G8R8A8_SRGB == surfaceFormat.format && VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == surfaceFormat.colorSpace)
        {
            *pSurfaceFormat = surfaceFormat;
            return;
        }
        else
        {
            // continue;s
        }
    }
    tickernelError("Format not found!");
}
static void choosePresentMode(VkPresentModeKHR *supportPresentModes, uint32_t supportPresentModeCount, VkPresentModeKHR targetPresentMode, VkPresentModeKHR *pPresentMode)
{
    *pPresentMode = supportPresentModes[0];
    for (uint32_t i = 0; i < supportPresentModeCount; i++)
    {
        if (supportPresentModes[i] == targetPresentMode)
        {
            *pPresentMode = targetPresentMode;
            break;
        }
        else
        {
            // continue;
        }
    }
}

static void createSwapchain(GraphicContext *pGraphicContext, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    VkResult result = VK_SUCCESS;
    pGraphicContext->swapchainWidth = swapchainWidth;
    pGraphicContext->swapchainHeight = swapchainHeight;

    VkPhysicalDevice vkPhysicalDevice = pGraphicContext->vkPhysicalDevice;
    VkSurfaceKHR vkSurface = pGraphicContext->vkSurface;
    VkDevice vkDevice = pGraphicContext->vkDevice;
    uint32_t graphicQueueFamilyIndex = pGraphicContext->graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGraphicContext->presentQueueFamilyIndex;

    VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
    uint32_t supportSurfaceFormatCount;
    uint32_t supportPresentModeCount;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurface, &vkSurfaceCapabilities);
    tryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &supportSurfaceFormatCount, NULL);
    tryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &supportPresentModeCount, NULL);
    tryThrowVulkanError(result);
    VkSurfaceFormatKHR *supportSurfaceFormats = tickernelMalloc(supportSurfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    VkPresentModeKHR *supportPresentModes = tickernelMalloc(supportPresentModeCount * sizeof(VkPresentModeKHR));

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &supportSurfaceFormatCount, supportSurfaceFormats);
    tryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &supportPresentModeCount, supportPresentModes);
    tryThrowVulkanError(result);

    VkPresentModeKHR presentMode;
    chooseSurfaceFormat(supportSurfaceFormats, supportSurfaceFormatCount, &pGraphicContext->surfaceFormat);
    choosePresentMode(supportPresentModes, supportPresentModeCount, pGraphicContext->targetPresentMode, &presentMode);
    uint32_t swapchainImageCount = pGraphicContext->targetSwapchainImageCount;
    if (swapchainImageCount < vkSurfaceCapabilities.minImageCount)
    {
        swapchainImageCount = vkSurfaceCapabilities.minImageCount;
    }
    else if (swapchainImageCount > vkSurfaceCapabilities.maxImageCount)
    {
        swapchainImageCount = vkSurfaceCapabilities.maxImageCount;
    }
    else
    {
        // Do nothing.
    }

    VkExtent2D swapchainExtent;
    if (pGraphicContext->swapchainWidth > vkSurfaceCapabilities.maxImageExtent.width)
    {
        swapchainExtent.width = vkSurfaceCapabilities.maxImageExtent.width;
    }
    else
    {
        if (pGraphicContext->swapchainWidth < vkSurfaceCapabilities.minImageExtent.width)
        {
            swapchainExtent.width = vkSurfaceCapabilities.minImageExtent.width;
        }
        else
        {
            swapchainExtent.width = pGraphicContext->swapchainWidth;
        }
    }

    if (pGraphicContext->swapchainHeight > vkSurfaceCapabilities.maxImageExtent.height)
    {
        swapchainExtent.height = vkSurfaceCapabilities.maxImageExtent.height;
    }
    else
    {
        if (pGraphicContext->swapchainHeight < vkSurfaceCapabilities.minImageExtent.height)
        {
            swapchainExtent.height = vkSurfaceCapabilities.minImageExtent.height;
        }
        else
        {
            swapchainExtent.height = pGraphicContext->swapchainHeight;
        }
    }
    VkSharingMode imageSharingMode;
    uint32_t queueFamilyIndexCount;
    uint32_t *pQueueFamilyIndices;

    if (graphicQueueFamilyIndex != presentQueueFamilyIndex)
    {
        imageSharingMode = VK_SHARING_MODE_CONCURRENT;

        uint32_t queueFamilyIndices[] = {graphicQueueFamilyIndex, presentQueueFamilyIndex};
        pQueueFamilyIndices = queueFamilyIndices;
        queueFamilyIndexCount = ARRAY_SIZE(queueFamilyIndices);
    }
    else
    {
        imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        queueFamilyIndexCount = 0;
        pQueueFamilyIndices = NULL;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0,
            .surface = vkSurface,
            .minImageCount = swapchainImageCount,
            .imageFormat = pGraphicContext->surfaceFormat.format,
            .imageColorSpace = pGraphicContext->surfaceFormat.colorSpace,
            .imageExtent = swapchainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = imageSharingMode,
            .queueFamilyIndexCount = queueFamilyIndexCount,
            .pQueueFamilyIndices = pQueueFamilyIndices,
            .preTransform = vkSurfaceCapabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };

    result = vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &pGraphicContext->vkSwapchain);
    tryThrowVulkanError(result);

    tickernelFree(supportSurfaceFormats);
    tickernelFree(supportPresentModes);

    result = vkGetSwapchainImagesKHR(vkDevice, pGraphicContext->vkSwapchain, &pGraphicContext->swapchainImageCount, NULL);
    tryThrowVulkanError(result);
    pGraphicContext->swapchainImages = tickernelMalloc(swapchainImageCount * sizeof(VkImage));
    result = vkGetSwapchainImagesKHR(vkDevice, pGraphicContext->vkSwapchain, &pGraphicContext->swapchainImageCount, pGraphicContext->swapchainImages);
    tryThrowVulkanError(result);
    pGraphicContext->swapchainImageViews = tickernelMalloc(swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        createImageView(pGraphicContext->vkDevice, pGraphicContext->swapchainImages[i], pGraphicContext->surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicContext->swapchainImageViews[i]);
    }
}
static void destroySwapchain(GraphicContext *pGraphicContext)
{
    for (uint32_t i = 0; i < pGraphicContext->swapchainImageCount; i++)
    {
        vkDestroyImageView(pGraphicContext->vkDevice, pGraphicContext->swapchainImageViews[i], NULL);
    }
    tickernelFree(pGraphicContext->swapchainImageViews);
    tickernelFree(pGraphicContext->swapchainImages);
    vkDestroySwapchainKHR(pGraphicContext->vkDevice, pGraphicContext->vkSwapchain, NULL);
}

static void createSignals(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };
    VkDevice vkDevice = pGraphicContext->vkDevice;

    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicContext->imageAvailableSemaphore);
    tryThrowVulkanError(result);
    result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicContext->renderFinishedSemaphore);
    tryThrowVulkanError(result);
    result = vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGraphicContext->renderFinishedFence);
    tryThrowVulkanError(result);
}

static void destroySignals(GraphicContext *pGraphicContext)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    vkDestroySemaphore(vkDevice, pGraphicContext->imageAvailableSemaphore, NULL);
    vkDestroySemaphore(vkDevice, pGraphicContext->renderFinishedSemaphore, NULL);
    vkDestroyFence(vkDevice, pGraphicContext->renderFinishedFence, NULL);
}

static void recreateSwapchain(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    uint32_t swapchainWidth = pGraphicContext->swapchainWidth;
    uint32_t swapchainHeight = pGraphicContext->swapchainHeight;

    result = vkDeviceWaitIdle(pGraphicContext->vkDevice);
    tryThrowVulkanError(result);

    destroySwapchain(pGraphicContext);
    createSwapchain(pGraphicContext, swapchainWidth, swapchainHeight);
}

static void createCommandPools(GraphicContext *pGraphicContext)
{
    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = pGraphicContext->graphicQueueFamilyIndex,
    };
    VkResult result = vkCreateCommandPool(pGraphicContext->vkDevice, &vkCommandPoolCreateInfo, NULL, &pGraphicContext->graphicVkCommandPool);
    tryThrowVulkanError(result);
}

static void destroyCommandPools(GraphicContext *pGraphicContext)
{
    vkDestroyCommandPool(pGraphicContext->vkDevice, pGraphicContext->graphicVkCommandPool, NULL);
}

static void submitCommandBuffer(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    // Submit workflow...
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGraphicContext->imageAvailableSemaphore},
        .pWaitDstStageMask = (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        .commandBufferCount = 1,
        .pCommandBuffers = &pGraphicContext->graphicVkCommandBuffers[pGraphicContext->swapchainIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = (VkSemaphore[]){pGraphicContext->renderFinishedSemaphore},
    };

    result = vkQueueSubmit(pGraphicContext->vkGraphicQueue, 1, &submitInfo, pGraphicContext->renderFinishedFence);
    tryThrowVulkanError(result);
}

static void present(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGraphicContext->renderFinishedSemaphore},
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]){pGraphicContext->vkSwapchain},
        .pImageIndices = &pGraphicContext->swapchainIndex,
        .pResults = NULL,
    };
    result = vkQueuePresentKHR(pGraphicContext->vkPresentQueue, &presentInfo);
    if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
    {
        printf("Recreate swapchain because of the result: %d when presenting.\n", result);
        recreateSwapchain(pGraphicContext);
    }
    else
    {
        tryThrowVulkanError(result);
    }
}

static void createVkCommandBuffers(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    pGraphicContext->graphicVkCommandBuffers = tickernelMalloc(sizeof(VkCommandBuffer) * pGraphicContext->swapchainImageCount);
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pGraphicContext->graphicVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = pGraphicContext->swapchainImageCount,
    };
    result = vkAllocateCommandBuffers(pGraphicContext->vkDevice, &vkCommandBufferAllocateInfo, pGraphicContext->graphicVkCommandBuffers);
    tryThrowVulkanError(result);
}

static void destroyVkCommandBuffers(GraphicContext *pGraphicContext)
{
    vkFreeCommandBuffers(pGraphicContext->vkDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->swapchainImageCount, pGraphicContext->graphicVkCommandBuffers);
    tickernelFree(pGraphicContext->graphicVkCommandBuffers);
}

static void recordCommandBuffer(GraphicContext *pGraphicContext)
{
    VkCommandBuffer vkCommandBuffer = pGraphicContext->graphicVkCommandBuffers[pGraphicContext->swapchainIndex];
    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = NULL,
        };
    VkResult result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    tryThrowVulkanError(result);

    for (uint32_t i = 0; i < pGraphicContext->RenderPasseDynamicArray.length; i++)
    {
        RenderPass *pRenderPass = pGraphicContext->RenderPasseDynamicArray.array[i];
        for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
        {
            Subpass *pSubpass = &pRenderPass->subpasses[i];

            for (uint32_t j = 0; j < pSubpass->pipelineDynamicArray.length; j++)
            {
                Pipeline *pPipeline = pSubpass->pipelineDynamicArray.array[j];
                vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipeline);
                for (uint32_t modelIndex = 0; modelIndex < pPipeline->materialDynamicArray.length; modelIndex++)
                {
                    Material *pMaterial = pPipeline->materialDynamicArray.array[modelIndex];
                    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipelineLayout, 0, 1, &pMaterial->vkDescriptorSet, 0, NULL);
                    for (uint32_t meshIndex = 0; meshIndex < pMaterial->meshDynamicArray.length; meshIndex++)
                    {
                        Mesh *pMesh = pMaterial->meshDynamicArray.array[meshIndex];
                        if (pMesh->vertexCount > 0)
                        {
                            if (pMesh->indexCount > 0)
                            {
                                if (pMesh->instanceCount > 0)
                                {
                                    VkBuffer vertexBuffers[] = {pMesh->vertexBuffer.vkBuffer, pMesh->instanceMappedBuffer.buffer.vkBuffer};
                                    VkDeviceSize offsets[] = {0, 0};
                                    vkCmdBindVertexBuffers(vkCommandBuffer, 0, 2, vertexBuffers, offsets);
                                    vkCmdBindIndexBuffer(vkCommandBuffer, pMesh->indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);
                                    vkCmdDrawIndexed(vkCommandBuffer, pMesh->indexCount, pMesh->instanceCount, 0, 0, 0);
                                }
                                else
                                {
                                    VkBuffer vertexBuffers[] = {pMesh->vertexBuffer.vkBuffer};
                                    VkDeviceSize offsets[] = {0};
                                    vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
                                    vkCmdBindIndexBuffer(vkCommandBuffer, pMesh->indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);
                                    vkCmdDrawIndexed(vkCommandBuffer, pMesh->indexCount, 1, 0, 0, 0);
                                }
                            }
                            else
                            {
                                if (pMesh->instanceCount > 0)
                                {
                                    VkBuffer vertexBuffers[] = {pMesh->vertexBuffer.vkBuffer, pMesh->instanceMappedBuffer.buffer.vkBuffer};
                                    VkDeviceSize offsets[] = {0, 0};
                                    vkCmdBindVertexBuffers(vkCommandBuffer, 0, 2, vertexBuffers, offsets);
                                    vkCmdDraw(vkCommandBuffer, pMesh->vertexCount, pMesh->instanceCount, 0, 0);
                                }
                                else
                                {
                                    VkBuffer vertexBuffers[] = {pMesh->vertexBuffer.vkBuffer};
                                    VkDeviceSize offsets[] = {0};
                                    vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
                                    vkCmdDraw(vkCommandBuffer, pMesh->vertexCount, 1, 0, 0);
                                }
                            }
                        }
                        else
                        {
                            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipelineLayout, 0, 1, &pMaterial->vkDescriptorSet, 0, NULL);
                            vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0);
                        }
                    }
                }
            }
        }
    }

    result = vkEndCommandBuffer(vkCommandBuffer);
    tryThrowVulkanError(result);
}

static void createFramebuffer(GraphicContext *pGraphicContext, uint32_t attachmentCount, Attachment *attachments, RenderPass *pRenderPass, VkFramebuffer *pVkFramebuffer)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    VkImageView attachmentVkImageViews[attachmentCount];
    uint32_t width, height;
    for (uint32_t j = 0; j < attachmentCount; j++)
    {
        Attachment attachment = attachments[j];
        if (ATTACHMENT_TYPE_SWAPCHAIN == attachment.attachmentType)
        {
            attachmentVkImageViews[j] = pGraphicContext->swapchainImageViews[j];
            width = pGraphicContext->swapchainWidth;
            height = pGraphicContext->swapchainHeight;
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == attachment.attachmentType)
        {
            DynamicAttachment dynamicAttachment = attachment.attachmentContent.dynamicAttachment;
            attachmentVkImageViews[j] = dynamicAttachment.graphicImage.vkImageView;
            width = pGraphicContext->swapchainWidth * dynamicAttachment.scaler;
            height = pGraphicContext->swapchainHeight * dynamicAttachment.scaler;
        }
        else
        {
            FixedAttachment fixedAttachment = attachment.attachmentContent.fixedAttachment;
            attachmentVkImageViews[j] = fixedAttachment.graphicImage.vkImageView;
            width = fixedAttachment.width;
            height = fixedAttachment.height;
        }
    }
    VkFramebufferCreateInfo vkFramebufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderPass = pRenderPass->vkRenderPass,
        .attachmentCount = attachmentCount,
        .pAttachments = attachmentVkImageViews,
        .width = width,
        .height = height,
        .layers = 1,
    };
    VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, pVkFramebuffer);
    tryThrowVulkanError(result);
}
static void destroyFramebuffer(GraphicContext *pGraphicContext, VkFramebuffer vkFramebuffer)
{
    vkDestroyFramebuffer(pGraphicContext->vkDevice, vkFramebuffer, NULL);
}

GraphicContext *startGraphic(const char *assetsPath, int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    GraphicContext *pGraphicContext = tickernelMalloc(sizeof(GraphicContext));
    pGraphicContext->assetsPath = assetsPath;
    pGraphicContext->targetSwapchainImageCount = targetSwapchainImageCount;
    pGraphicContext->targetPresentMode = targetPresentMode;
    pGraphicContext->vkInstance = vkInstance;
    pGraphicContext->vkSurface = vkSurface;

    pickPhysicalDevice(pGraphicContext);
    createLogicalDevice(pGraphicContext);
    createSwapchain(pGraphicContext, swapchainWidth, swapchainHeight);
    createSignals(pGraphicContext);
    createCommandPools(pGraphicContext);
    createVkCommandBuffers(pGraphicContext);

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = pGraphicContext->swapchainWidth,
        .height = pGraphicContext->swapchainHeight,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkOffset2D offset = {
        .x = 0,
        .y = 0,
    };
    VkExtent2D extent = {
        .width = pGraphicContext->swapchainWidth,
        .height = pGraphicContext->swapchainHeight,
    };
    VkRect2D scissor = {
        .offset = offset,
        .extent = extent,
    };

    return pGraphicContext;
}

void updateGraphic(GraphicContext *pGraphicContext, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    VkResult result = VK_SUCCESS;
    // Wait for gpu
    result = vkWaitForFences(pGraphicContext->vkDevice, 1, &pGraphicContext->renderFinishedFence, VK_TRUE, UINT64_MAX);
    tryThrowVulkanError(result);

    if (swapchainWidth != pGraphicContext->swapchainWidth || swapchainHeight != pGraphicContext->swapchainHeight)
    {
        printf("Recreate swapchain because of a size change: (%d, %d) to (%d, %d) \n",
               pGraphicContext->swapchainWidth,
               pGraphicContext->swapchainHeight,
               swapchainWidth,
               swapchainHeight);
        pGraphicContext->swapchainWidth = swapchainWidth;
        pGraphicContext->swapchainHeight = swapchainHeight;
        recreateSwapchain(pGraphicContext);
    }
    else
    {
        VkDevice vkDevice = pGraphicContext->vkDevice;
        result = vkAcquireNextImageKHR(vkDevice, pGraphicContext->vkSwapchain, UINT64_MAX, pGraphicContext->imageAvailableSemaphore, VK_NULL_HANDLE, &pGraphicContext->swapchainIndex);
        if (result != VK_SUCCESS)
        {
            if (VK_ERROR_OUT_OF_DATE_KHR == result)
            {
                printf("Recreate swapchain because of result: %d\n", result);
                recreateSwapchain(pGraphicContext);
            }
            else if (VK_SUBOPTIMAL_KHR == result)
            {
                result = vkResetFences(pGraphicContext->vkDevice, 1, &pGraphicContext->renderFinishedFence);
                tryThrowVulkanError(result);
                recordCommandBuffer(pGraphicContext);
                submitCommandBuffer(pGraphicContext);
                present(pGraphicContext);
            }
            else
            {
                tryThrowVulkanError(result);
            }
        }
        else
        {
            result = vkResetFences(pGraphicContext->vkDevice, 1, &pGraphicContext->renderFinishedFence);
            tryThrowVulkanError(result);

            recordCommandBuffer(pGraphicContext);
            submitCommandBuffer(pGraphicContext);
            present(pGraphicContext);
        }
    }
}

void endGraphic(GraphicContext *pGraphicContext)
{
    VkResult result = vkDeviceWaitIdle(pGraphicContext->vkDevice);
    tryThrowVulkanError(result);

    destroyVkCommandBuffers(pGraphicContext);
    destroyCommandPools(pGraphicContext);
    destroySignals(pGraphicContext);
    destroySwapchain(pGraphicContext);
    destroyLogicalDevice(pGraphicContext);

    tickernelFree(pGraphicContext);
}

void createPipeline(GraphicContext *pGraphicContext, VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo, VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo, char **shaderPaths, uint32_t vkDescriptorPoolSizeCount, VkDescriptorPoolSize *vkDescriptorPoolSizes, Pipeline *pPipeline)
{
    VkPipelineCache pipelineCache = NULL;
    VkDevice vkDevice = pGraphicContext->vkDevice;
    for (uint32_t i = 0; i < vkGraphicsPipelineCreateInfo.stageCount; i++)
    {
        VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo = vkGraphicsPipelineCreateInfo.pStages[i];
        createVkShaderModule(vkDevice, shaderPaths[i], &vkPipelineShaderStageCreateInfo.module);
    }

    VkResult result = vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &pPipeline->descriptorSetLayout);
    tryThrowVulkanError(result);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &pPipeline->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    result = vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, NULL, &pPipeline->vkPipelineLayout);
    tryThrowVulkanError(result);

    vkGraphicsPipelineCreateInfo.layout = pPipeline->vkPipelineLayout;
    result = vkCreateGraphicsPipelines(vkDevice, pipelineCache, 1, &vkGraphicsPipelineCreateInfo, NULL, &pPipeline->vkPipeline);
    tryThrowVulkanError(result);

    for (uint32_t i = 0; i < vkGraphicsPipelineCreateInfo.stageCount; i++)
    {
        VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo = vkGraphicsPipelineCreateInfo.pStages[i];
        destroyVkShaderModule(vkDevice, vkPipelineShaderStageCreateInfo.module);
    }

    pPipeline->vkDescriptorPoolSizeCount = vkDescriptorPoolSizeCount;
    pPipeline->vkDescriptorPoolSizes = tickernelMalloc(sizeof(VkDescriptorPoolSize) * pPipeline->vkDescriptorPoolSizeCount);
    memcpy(pPipeline->vkDescriptorPoolSizes, vkDescriptorPoolSizes, sizeof(VkDescriptorPoolSize) * pPipeline->vkDescriptorPoolSizeCount);
    tickernelCreateDynamicArray(&pPipeline->materialDynamicArray, 1, sizeof(Material));
}

void destroyPipeline(Pipeline pipeline, VkDevice vkDevice)
{
    for (uint32_t i = 0; i < pipeline.materialDynamicArray.length; i++)
    {
        Material *pMaterial = pipeline.materialDynamicArray.array[i];
        destroyMaterial(*pMaterial, vkDevice);
        tickernelDestroyDynamicArray(&pMaterial->meshDynamicArray);
    }
    tickernelDestroyDynamicArray(&pipeline.materialDynamicArray);

    tickernelFree(pipeline.vkDescriptorPoolSizes);

    vkDestroyPipeline(vkDevice, pipeline.vkPipeline, NULL);
    vkDestroyPipelineLayout(vkDevice, pipeline.vkPipelineLayout, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pipeline.descriptorSetLayout, NULL);
}

void createMaterial(VkDevice vkDevice, Pipeline pipeline, size_t meshSize, VkWriteDescriptorSet *vkWriteDescriptorSets, uint32_t vkWriteDescriptorSetCount, Material *pMaterial)
{
    tickernelCreateDynamicArray(&pMaterial->meshDynamicArray, 1, meshSize);
    pMaterial->vkDescriptorSet = VK_NULL_HANDLE;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = pipeline.vkDescriptorPoolSizeCount,
        .pPoolSizes = pipeline.vkDescriptorPoolSizes,
    };
    VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &pMaterial->vkDescriptorPool);
    tryThrowVulkanError(result);

    // Create vkDescriptorSet
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = pMaterial->vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pipeline.descriptorSetLayout,
    };
    result = vkAllocateDescriptorSets(vkDevice, &descriptorSetAllocateInfo, &pMaterial->vkDescriptorSet);
    tryThrowVulkanError(result);

    vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetCount, vkWriteDescriptorSets, 0, NULL);
}

void destroyMaterial(Material material, VkDevice vkDevice)
{
    vkFreeDescriptorSets(vkDevice, material.vkDescriptorPool, 1, &material.vkDescriptorSet);
    vkDestroyDescriptorPool(vkDevice, material.vkDescriptorPool, NULL);

    for (size_t i = material.meshDynamicArray.length - 1; i > -1; i--)
    {
        Mesh *pMesh = material.meshDynamicArray.array[i];
        if (pMesh)
        {
            destroyMesh(pMesh, vkDevice);
        }
    }
    tickernelDestroyDynamicArray(&material.meshDynamicArray);
}

void createMesh(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, uint32_t vertexCount, VkDeviceSize vertexBufferSize, void *vertexBufferData, uint32_t indexCount, VkDeviceSize indexBufferSize, void *indexBufferData, uint32_t instanceCount, VkDeviceSize instanceBufferSize, void *instanceBufferData, Mesh *pMesh)
{
    pMesh = tickernelMalloc(sizeof(Mesh));
    pMesh->vertexCount = vertexCount;
    if (vertexCount > 0)
    {
        createBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pMesh->vertexBuffer);

        updateBufferWithStagingBuffer(vkDevice, vkPhysicalDevice, 0, vertexBufferSize, vertexBufferData, graphicVkCommandPool, vkGraphicQueue, pMesh->vertexBuffer.vkBuffer);

        pMesh->indexCount = indexCount;
        if (indexCount > 0)
        {
            createBuffer(vkDevice, vkPhysicalDevice, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pMesh->indexBuffer);

            updateBufferWithStagingBuffer(vkDevice, vkPhysicalDevice, 0, indexBufferSize, indexBufferData, graphicVkCommandPool, vkGraphicQueue, pMesh->indexBuffer.vkBuffer);
            pMesh->indexCount = indexCount;
        }
        else
        {
            // continue;
        }

        pMesh->maxInstanceCount = instanceCount;
        pMesh->instanceCount = instanceCount;
        if (instanceCount > 0)
        {
            createBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pMesh->instanceMappedBuffer.buffer);
            vkMapMemory(vkDevice, pMesh->instanceMappedBuffer.buffer.vkBufferMemory, 0, instanceBufferSize, 0, pMesh->instanceMappedBuffer.mapped);

            memcpy(pMesh->instanceMappedBuffer.mapped, instanceBufferData, instanceBufferSize);
        }
        else
        {
            // continue;
        }
    }
    else
    {
        // continue;
    }
}

void destroyMesh(Mesh *pMesh, VkDevice vkDevice)
{
    if (pMesh->vertexCount > 0)
    {
        destroyBuffer(vkDevice, pMesh->vertexBuffer);
    }
    else
    {
        // continue;
    }
    if (pMesh->indexCount > 0)
    {
        destroyBuffer(vkDevice, pMesh->indexBuffer);
    }
    else
    {
        // continue;
    }

    if (pMesh->instanceCount > 0)
    {
        vkUnmapMemory(vkDevice, pMesh->instanceMappedBuffer.buffer.vkBufferMemory);
        destroyBuffer(vkDevice, pMesh->instanceMappedBuffer.buffer);
    }
    else
    {
        // continue;
    }

    tickernelFree(pMesh);
}

void updateMeshInstanceBuffer(Mesh *pMesh, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, VkDeviceSize instanceBufferSize, void *instanceBufferData, uint32_t instanceCount)
{
    if (0 == pMesh->maxInstanceCount)
    {
        pMesh->maxInstanceCount = instanceCount;
        pMesh->instanceCount = instanceCount;

        createMappedBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pMesh->instanceMappedBuffer);
        updateMappedBuffer(pMesh->instanceMappedBuffer.mapped, instanceBufferData, instanceBufferSize);
    }
    else if (instanceCount <= pMesh->maxInstanceCount)
    {
        pMesh->instanceCount = instanceCount;
        updateMappedBuffer(pMesh->instanceMappedBuffer.mapped, instanceBufferData, instanceBufferSize);
    }
    else
    {
        destroyMappedBuffer(vkDevice, pMesh->instanceMappedBuffer);
        pMesh->maxInstanceCount = instanceCount;
        pMesh->instanceCount = instanceCount;
        createMappedBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pMesh->instanceMappedBuffer);
        updateMappedBuffer(pMesh->instanceMappedBuffer.mapped, instanceBufferData, instanceBufferSize);
    }
}

void createRenderPass(GraphicContext *pGraphicContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment *attachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    bool useSwapchain = false;
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        Attachment attachment = attachments[i];
        if (ATTACHMENT_TYPE_SWAPCHAIN == attachment.attachmentType)
        {
            useSwapchain = true;
            vkAttachmentDescriptions[i].format = pGraphicContext->surfaceFormat.format;
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == attachment.attachmentType)
        {
            vkAttachmentDescriptions[i].format = attachment.attachmentContent.dynamicAttachment.graphicImage.vkFormat;
        }
        else
        {
            vkAttachmentDescriptions[i].format = attachment.attachmentContent.fixedAttachment.graphicImage.vkFormat;
        }
    }

    VkRenderPassCreateInfo vkRenderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .attachmentCount = attachmentCount,
        .pAttachments = vkAttachmentDescriptions,
        .subpassCount = subpassCount,
        .pSubpasses = vkSubpassDescriptions,
        .dependencyCount = 0,
        .pDependencies = NULL,
    };
    VkResult result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pRenderPass->vkRenderPass);
    tryThrowVulkanError(result);

    if (useSwapchain)
    {
        pRenderPass->vkFramebufferCount = pGraphicContext->swapchainImageCount;
        pRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer) * pRenderPass->vkFramebufferCount);
        for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
        {
            createFramebuffer(pGraphicContext, pRenderPass->vkFramebufferCount, attachments, pRenderPass, &pRenderPass->vkFramebuffers[i]);
        }
    }
    else
    {
        pRenderPass->vkFramebufferCount = 1;
        pRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer));
        createFramebuffer(pGraphicContext, pRenderPass->vkFramebufferCount, attachments, pRenderPass, &pRenderPass->vkFramebuffers[0]);
    }

    pRenderPass->subpassCount = vkRenderPassCreateInfo.subpassCount;
    pRenderPass->subpasses = tickernelMalloc(sizeof(Subpass) * pRenderPass->subpassCount);
    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        tickernelCreateDynamicArray(&pSubpass->pipelineDynamicArray, 0, sizeof(Pipeline));
    }
}

void destroyRenderPass(GraphicContext *pGraphicContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment *attachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, RenderPass *pRenderPass)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        for (uint32_t i = 0; i < pSubpass->pipelineDynamicArray.length; i++)
        {
            Pipeline *pPipeline = pSubpass->pipelineDynamicArray.array[i];
            destroyPipeline(*pPipeline, vkDevice);
        }
        tickernelDestroyDynamicArray(&pSubpass->pipelineDynamicArray);
    }
}