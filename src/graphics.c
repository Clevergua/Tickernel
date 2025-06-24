#include "graphics.h"

static void tryThrowVulkanError(VkResult vkResult)
{
    tickernelAssert(vkResult == VK_SUCCESS, "Vulkan error code: %d\n", vkResult);
}

static void hasAllRequiredExtensions(GraphicsContext *pGraphicsContext, VkPhysicalDevice vkPhysicalDevice, bool *pHasAllRequiredExtensions)
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

static void hasGraphicsAndPresentQueueFamilies(GraphicsContext *pGraphicsContext, VkPhysicalDevice vkPhysicalDevice, bool *pHasGraphicsAndPresentQueueFamilies, uint32_t *pGraphicsQueueFamilyIndex, uint32_t *pPresentQueueFamilyIndex)
{

    VkSurfaceKHR vkSurface = pGraphicsContext->vkSurface;
    uint32_t *pQueueFamilyPropertyCount = &pGraphicsContext->queueFamilyPropertyCount;

    VkResult result = VK_SUCCESS;

    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, pQueueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *vkQueueFamilyPropertiesList = tickernelMalloc(*pQueueFamilyPropertyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, pQueueFamilyPropertyCount, vkQueueFamilyPropertiesList);

    int graphicsIndexCount = 0;
    int presentIndexCount = 0;
    *pHasGraphicsAndPresentQueueFamilies = false;
    for (uint32_t i = 0; i < *pQueueFamilyPropertyCount; i++)
    {
        VkQueueFamilyProperties vkQueueFamilyProperties = vkQueueFamilyPropertiesList[i];
        if (vkQueueFamilyProperties.queueCount > 0 && vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            *pGraphicsQueueFamilyIndex = i;
            graphicsIndexCount++;
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

        if (graphicsIndexCount > 0 && presentIndexCount > 0)
        {
            *pHasGraphicsAndPresentQueueFamilies = true;
            break;
        }
    }
    tickernelFree(vkQueueFamilyPropertiesList);
}

static void pickPhysicalDevice(GraphicsContext *pGraphicsContext)
{
    VkResult result = VK_SUCCESS;
    uint32_t deviceCount = -1;
    result = vkEnumeratePhysicalDevices(pGraphicsContext->vkInstance, &deviceCount, NULL);
    tryThrowVulkanError(result);

    if (deviceCount <= 0)
    {
        printf("failed to find GPUs with Vulkan support!");
    }
    else
    {
        VkPhysicalDevice *devices = tickernelMalloc(deviceCount * sizeof(VkPhysicalDevice));
        result = vkEnumeratePhysicalDevices(pGraphicsContext->vkInstance, &deviceCount, devices);
        tryThrowVulkanError(result);

        uint32_t graphicsQueueFamilyIndex = -1;
        uint32_t presentQueueFamilyIndex = -1;
        uint32_t maxScore = 0;
        char *targetDeviceName = NULL;
        pGraphicsContext->vkPhysicalDevice = NULL;

        VkSurfaceKHR vkSurface = pGraphicsContext->vkSurface;
        for (uint32_t i = 0; i < deviceCount; i++)
        {
            VkPhysicalDevice vkPhysicalDevice = devices[i];

            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

            bool hasExtensions;
            hasAllRequiredExtensions(pGraphicsContext, vkPhysicalDevice, &hasExtensions);
            tryThrowVulkanError(result);
            bool hasQueueFamilies;
            hasGraphicsAndPresentQueueFamilies(pGraphicsContext, vkPhysicalDevice, &hasQueueFamilies, &graphicsQueueFamilyIndex, &presentQueueFamilyIndex);

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
                        targetDeviceName = deviceProperties.deviceName;
                        pGraphicsContext->vkPhysicalDevice = vkPhysicalDevice;
                        pGraphicsContext->graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
                        pGraphicsContext->presentQueueFamilyIndex = presentQueueFamilyIndex;
                        pGraphicsContext->vkPhysicalDeviceProperties = deviceProperties;
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
        if (pGraphicsContext->vkPhysicalDevice != NULL)
        {
            printf("Selected target physical device named %s\n", targetDeviceName);
        }
        else
        {
            tickernelError("failed to find GPUs with Vulkan support!");
        }
    }
}

static void createLogicalDevice(GraphicsContext *pGraphicsContext)
{
    VkResult result = VK_SUCCESS;

    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    uint32_t graphicsQueueFamilyIndex = pGraphicsContext->graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGraphicsContext->presentQueueFamilyIndex;
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo *queueCreateInfos;
    uint32_t queueCount;
    if (graphicsQueueFamilyIndex == presentQueueFamilyIndex)
    {
        queueCount = 1;
        queueCreateInfos = tickernelMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
        VkDeviceQueueCreateInfo graphicsCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = graphicsQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
        queueCreateInfos[0] = graphicsCreateInfo;
    }
    else
    {
        queueCount = 2;
        queueCreateInfos = tickernelMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
        VkDeviceQueueCreateInfo graphicsCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = graphicsQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
        queueCreateInfos[0] = graphicsCreateInfo;

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
    result = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, NULL, &pGraphicsContext->vkDevice);
    tryThrowVulkanError(result);
    vkGetDeviceQueue(pGraphicsContext->vkDevice, graphicsQueueFamilyIndex, 0, &pGraphicsContext->vkGraphicsQueue);
    vkGetDeviceQueue(pGraphicsContext->vkDevice, presentQueueFamilyIndex, 0, &pGraphicsContext->vkPresentQueue);
    tickernelFree(queueCreateInfos);
}

static void destroyLogicalDevice(GraphicsContext *pGraphicsContext)
{
    vkDestroyDevice(pGraphicsContext->vkDevice, NULL);
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

static void createImageView(VkDevice vkDevice, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView)
{
    VkComponentMapping components = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };
    VkImageSubresourceRange subresourceRange = {
        .aspectMask = imageAspectFlags,
        .levelCount = 1,
        .baseMipLevel = 0,
        .layerCount = 1,
        .baseArrayLayer = 0,
    };
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = components,
        .subresourceRange = subresourceRange,
    };
    VkResult result = vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, pImageView);
    tryThrowVulkanError(result);
}
static void destroyImageView(VkDevice vkDevice, VkImageView vkImageView)
{
    vkDestroyImageView(vkDevice, vkImageView, NULL);
}

static void createSwapchain(GraphicsContext *pGraphicsContext, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    VkResult result = VK_SUCCESS;
    pGraphicsContext->swapchainWidth = swapchainWidth;
    pGraphicsContext->swapchainHeight = swapchainHeight;

    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    VkSurfaceKHR vkSurface = pGraphicsContext->vkSurface;
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    uint32_t graphicsQueueFamilyIndex = pGraphicsContext->graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGraphicsContext->presentQueueFamilyIndex;

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
    chooseSurfaceFormat(supportSurfaceFormats, supportSurfaceFormatCount, &pGraphicsContext->surfaceFormat);
    choosePresentMode(supportPresentModes, supportPresentModeCount, pGraphicsContext->targetPresentMode, &presentMode);
    uint32_t swapchainImageCount = pGraphicsContext->targetSwapchainImageCount;
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
    if (pGraphicsContext->swapchainWidth > vkSurfaceCapabilities.maxImageExtent.width)
    {
        swapchainExtent.width = vkSurfaceCapabilities.maxImageExtent.width;
    }
    else
    {
        if (pGraphicsContext->swapchainWidth < vkSurfaceCapabilities.minImageExtent.width)
        {
            swapchainExtent.width = vkSurfaceCapabilities.minImageExtent.width;
        }
        else
        {
            swapchainExtent.width = pGraphicsContext->swapchainWidth;
        }
    }

    if (pGraphicsContext->swapchainHeight > vkSurfaceCapabilities.maxImageExtent.height)
    {
        swapchainExtent.height = vkSurfaceCapabilities.maxImageExtent.height;
    }
    else
    {
        if (pGraphicsContext->swapchainHeight < vkSurfaceCapabilities.minImageExtent.height)
        {
            swapchainExtent.height = vkSurfaceCapabilities.minImageExtent.height;
        }
        else
        {
            swapchainExtent.height = pGraphicsContext->swapchainHeight;
        }
    }
    VkSharingMode imageSharingMode;
    uint32_t queueFamilyIndexCount;
    uint32_t *pQueueFamilyIndices;

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
    {
        imageSharingMode = VK_SHARING_MODE_CONCURRENT;

        uint32_t queueFamilyIndices[] = {graphicsQueueFamilyIndex, presentQueueFamilyIndex};
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
            .imageFormat = pGraphicsContext->surfaceFormat.format,
            .imageColorSpace = pGraphicsContext->surfaceFormat.colorSpace,
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

    result = vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &pGraphicsContext->vkSwapchain);
    tryThrowVulkanError(result);

    tickernelFree(supportSurfaceFormats);
    tickernelFree(supportPresentModes);

    result = vkGetSwapchainImagesKHR(vkDevice, pGraphicsContext->vkSwapchain, &pGraphicsContext->swapchainImageCount, NULL);
    tryThrowVulkanError(result);
    pGraphicsContext->swapchainImages = tickernelMalloc(swapchainImageCount * sizeof(VkImage));
    result = vkGetSwapchainImagesKHR(vkDevice, pGraphicsContext->vkSwapchain, &pGraphicsContext->swapchainImageCount, pGraphicsContext->swapchainImages);
    tryThrowVulkanError(result);
    pGraphicsContext->swapchainImageViews = tickernelMalloc(swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        createImageView(vkDevice, pGraphicsContext->swapchainImages[i], pGraphicsContext->surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicsContext->swapchainImageViews[i]);
    }
    pGraphicsContext->swapchainAttachment.attachmentType = ATTACHMENT_TYPE_SWAPCHAIN;
};
static void destroySwapchain(GraphicsContext *pGraphicsContext)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    for (uint32_t i = 0; i < pGraphicsContext->swapchainImageCount; i++)
    {
        destroyImageView(vkDevice, pGraphicsContext->swapchainImageViews[i]);
    }
    tickernelFree(pGraphicsContext->swapchainImageViews);
    tickernelFree(pGraphicsContext->swapchainImages);
    vkDestroySwapchainKHR(vkDevice, pGraphicsContext->vkSwapchain, NULL);
}

static void createSignals(GraphicsContext *pGraphicsContext)
{
    VkResult result = VK_SUCCESS;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };
    VkDevice vkDevice = pGraphicsContext->vkDevice;

    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicsContext->imageAvailableSemaphore);
    tryThrowVulkanError(result);
    result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicsContext->renderFinishedSemaphore);
    tryThrowVulkanError(result);
    result = vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGraphicsContext->renderFinishedFence);
    tryThrowVulkanError(result);
}
static void destroySignals(GraphicsContext *pGraphicsContext)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    vkDestroySemaphore(vkDevice, pGraphicsContext->imageAvailableSemaphore, NULL);
    vkDestroySemaphore(vkDevice, pGraphicsContext->renderFinishedSemaphore, NULL);
    vkDestroyFence(vkDevice, pGraphicsContext->renderFinishedFence, NULL);
}

static void recreateSwapchain(GraphicsContext *pGraphicsContext)
{
    VkResult result = VK_SUCCESS;
    uint32_t swapchainWidth = pGraphicsContext->swapchainWidth;
    uint32_t swapchainHeight = pGraphicsContext->swapchainHeight;

    result = vkDeviceWaitIdle(pGraphicsContext->vkDevice);
    tryThrowVulkanError(result);

    destroySwapchain(pGraphicsContext);
    createSwapchain(pGraphicsContext, swapchainWidth, swapchainHeight);
}

static void createCommandPools(GraphicsContext *pGraphicsContext)
{
    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = pGraphicsContext->graphicsQueueFamilyIndex,
    };
    VkResult result = vkCreateCommandPool(pGraphicsContext->vkDevice, &vkCommandPoolCreateInfo, NULL, &pGraphicsContext->graphicsVkCommandPool);
    tryThrowVulkanError(result);
}

static void destroyCommandPools(GraphicsContext *pGraphicsContext)
{
    vkDestroyCommandPool(pGraphicsContext->vkDevice, pGraphicsContext->graphicsVkCommandPool, NULL);
}

static void submitCommandBuffer(GraphicsContext *pGraphicsContext)
{
    VkResult result = VK_SUCCESS;
    // Submit workflow...
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGraphicsContext->imageAvailableSemaphore},
        .pWaitDstStageMask = (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        .commandBufferCount = 1,
        .pCommandBuffers = &pGraphicsContext->graphicsVkCommandBuffers[pGraphicsContext->swapchainIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = (VkSemaphore[]){pGraphicsContext->renderFinishedSemaphore},
    };

    result = vkQueueSubmit(pGraphicsContext->vkGraphicsQueue, 1, &submitInfo, pGraphicsContext->renderFinishedFence);
    tryThrowVulkanError(result);
}

static void present(GraphicsContext *pGraphicsContext)
{
    VkResult result = VK_SUCCESS;
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGraphicsContext->renderFinishedSemaphore},
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]){pGraphicsContext->vkSwapchain},
        .pImageIndices = &pGraphicsContext->swapchainIndex,
        .pResults = NULL,
    };
    result = vkQueuePresentKHR(pGraphicsContext->vkPresentQueue, &presentInfo);
    if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
    {
        printf("Recreate swapchain because of the result: %d when presenting.\n", result);
        recreateSwapchain(pGraphicsContext);
    }
    else
    {
        tryThrowVulkanError(result);
    }
}

static void createVkCommandBuffers(GraphicsContext *pGraphicsContext)
{
    VkResult result = VK_SUCCESS;
    pGraphicsContext->graphicsVkCommandBuffers = tickernelMalloc(sizeof(VkCommandBuffer) * pGraphicsContext->swapchainImageCount);
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pGraphicsContext->graphicsVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = pGraphicsContext->swapchainImageCount,
    };
    result = vkAllocateCommandBuffers(pGraphicsContext->vkDevice, &vkCommandBufferAllocateInfo, pGraphicsContext->graphicsVkCommandBuffers);
    tryThrowVulkanError(result);
}

static void destroyVkCommandBuffers(GraphicsContext *pGraphicsContext)
{
    vkFreeCommandBuffers(pGraphicsContext->vkDevice, pGraphicsContext->graphicsVkCommandPool, pGraphicsContext->swapchainImageCount, pGraphicsContext->graphicsVkCommandBuffers);
    tickernelFree(pGraphicsContext->graphicsVkCommandBuffers);
}

static void recordCommandBuffer(GraphicsContext *pGraphicsContext)
{
    VkCommandBuffer vkCommandBuffer = pGraphicsContext->graphicsVkCommandBuffers[pGraphicsContext->swapchainIndex];
    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = NULL,
        };
    VkResult result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    tryThrowVulkanError(result);

    for (uint32_t i = 0; i < pGraphicsContext->pRenderPassDynamicArray.count; i++)
    {
        RenderPass *pRenderPass = tickernelGetFromDynamicArray(&pGraphicsContext->pRenderPassDynamicArray, i);
        for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
        {
            Subpass *pSubpass = &pRenderPass->subpasses[i];

            for (uint32_t j = 0; j < pSubpass->pPipelineDynamicArray.count; j++)
            {
                Pipeline *pPipeline = tickernelGetFromDynamicArray(&pSubpass->pPipelineDynamicArray, j);
                vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipeline);
                for (uint32_t modelIndex = 0; modelIndex < pPipeline->pMaterialDynamicArray.count; modelIndex++)
                {
                    Material *pMaterial = tickernelGetFromDynamicArray(&pPipeline->pMaterialDynamicArray, modelIndex);
                    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipelineLayout, 0, 1, &pMaterial->vkDescriptorSet, 0, NULL);
                    for (uint32_t meshIndex = 0; meshIndex < pMaterial->meshDynamicArray.count; meshIndex++)
                    {
                        Mesh *pMesh = tickernelGetFromDynamicArray(&pMaterial->meshDynamicArray, meshIndex);
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

static void createFramebuffer(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, Attachment **pAttachments, RenderPass *pRenderPass, VkFramebuffer *pVkFramebuffer)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkImageView *attachmentVkImageViews = tickernelMalloc(sizeof(VkImageView) * attachmentCount);
    uint32_t width = 0;
    uint32_t height = 0;
    for (uint32_t j = 0; j < attachmentCount; j++)
    {
        Attachment *pAttachment = pAttachments[j];
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            attachmentVkImageViews[j] = pGraphicsContext->swapchainImageViews[j];
            width = pGraphicsContext->swapchainWidth;
            height = pGraphicsContext->swapchainHeight;
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            DynamicAttachmentContent DynamicAttachmentContent = pAttachment->attachmentContent.dynamicAttachmentContent;
            attachmentVkImageViews[j] = DynamicAttachmentContent.graphicsImage.vkImageView;
            width = pGraphicsContext->swapchainWidth * DynamicAttachmentContent.scaler;
            height = pGraphicsContext->swapchainHeight * DynamicAttachmentContent.scaler;
        }
        else
        {
            FixedAttachmentContent FixedAttachmentContent = pAttachment->attachmentContent.fixedAttachmentContent;
            attachmentVkImageViews[j] = FixedAttachmentContent.graphicsImage.vkImageView;
            width = FixedAttachmentContent.width;
            height = FixedAttachmentContent.height;
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
    tickernelFree(attachmentVkImageViews);
    tryThrowVulkanError(result);
}
static void destroyFramebuffer(GraphicsContext *pGraphicsContext, VkFramebuffer vkFramebuffer)
{
    vkDestroyFramebuffer(pGraphicsContext->vkDevice, vkFramebuffer, NULL);
}

static void findMemoryType(VkPhysicalDevice vkPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex)
{
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &physicalDeviceMemoryProperties);
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
        {
            *memoryTypeIndex = i;
            return;
        }
    }
    printf("Failed to find suitable memory type!");
}

static void createImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage *pVkImage, VkDeviceMemory *pVkDeviceMemory)
{
    VkResult result = VK_SUCCESS;

    VkImageCreateInfo imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = vkFormat,
        .extent = vkExtent3D,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = vkImageTiling,
        .usage = vkImageUsageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = 0,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    result = vkCreateImage(vkDevice, &imageCreateInfo, NULL, pVkImage);
    tryThrowVulkanError(result);
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, *pVkImage, &memoryRequirements);
    uint32_t memoryTypeIndex;
    findMemoryType(vkPhysicalDevice, memoryRequirements.memoryTypeBits, vkMemoryPropertyFlags, &memoryTypeIndex);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };

    result = vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pVkDeviceMemory);
    tryThrowVulkanError(result);
    result = vkBindImageMemory(vkDevice, *pVkImage, *pVkDeviceMemory, 0);
    tryThrowVulkanError(result);
}
static void destroyImage(VkDevice vkDevice, VkImage vkImage, VkDeviceMemory vkDeviceMemory)
{
    vkDestroyImage(vkDevice, vkImage, NULL);
    vkFreeMemory(vkDevice, vkDeviceMemory, NULL);
}

static void createGraphicsImage(GraphicsContext *pGraphicsContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicsImage *pGraphicsImage)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    pGraphicsImage->vkFormat = vkFormat;
    createImage(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, &pGraphicsImage->vkImage, &pGraphicsImage->vkDeviceMemory);
    createImageView(vkDevice, pGraphicsImage->vkImage, pGraphicsImage->vkFormat, vkImageAspectFlags, &pGraphicsImage->vkImageView);
}
static void destroyGraphicsImage(GraphicsContext *pGraphicsContext, GraphicsImage graphicsImage)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    vkDestroyImageView(vkDevice, graphicsImage.vkImageView, NULL);
    destroyImage(vkDevice, graphicsImage.vkImage, graphicsImage.vkDeviceMemory);
}

static void createBuffer(GraphicsContext *pGraphicsContext, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, Buffer *pBuffer)
{
    VkResult result = VK_SUCCESS;
    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = bufferSize,
        .usage = bufferUsageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = 0,
    };
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    result = vkCreateBuffer(vkDevice, &bufferCreateInfo, NULL, &pBuffer->vkBuffer);
    tryThrowVulkanError(result);
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vkDevice, pBuffer->vkBuffer, &memoryRequirements);
    uint32_t memoryTypeIndex;
    findMemoryType(vkPhysicalDevice, memoryRequirements.memoryTypeBits, memoryPropertyFlags, &memoryTypeIndex);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    result = vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, &pBuffer->vkBufferMemory);
    tryThrowVulkanError(result);
    result = vkBindBufferMemory(vkDevice, pBuffer->vkBuffer, pBuffer->vkBufferMemory, 0);
    tryThrowVulkanError(result);
    pBuffer->size = bufferSize;
}
static void destroyBuffer(GraphicsContext *pGraphicsContext, Buffer buffer)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    vkFreeMemory(vkDevice, buffer.vkBufferMemory, NULL);
    vkDestroyBuffer(vkDevice, buffer.vkBuffer, NULL);
}
static void updateBuffer(GraphicsContext *pGraphicsContext, Buffer *pBuffer, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData)
{
    tickernelAssert(offset + bufferSize <= pBuffer->size, "Buffer update out of bounds!");
    void *data;
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkDeviceMemory vkBufferMemory = pBuffer->vkBufferMemory;
    VkResult result = vkMapMemory(vkDevice, vkBufferMemory, offset, bufferSize, 0, &data);
    tryThrowVulkanError(result);

    memcpy(data, bufferData, bufferSize);
    vkUnmapMemory(vkDevice, vkBufferMemory);
}

static void updateBufferWithStagingBuffer(GraphicsContext *pGraphicsContext, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData, Buffer buffer)
{
    Buffer stagingBuffer;
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    tickernelAssert(offset + bufferSize <= buffer.size, "Buffer update out of bounds!");
    createBuffer(pGraphicsContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer);

    void *pData;
    VkResult result = vkMapMemory(vkDevice, stagingBuffer.vkBufferMemory, offset, bufferSize, 0, &pData);
    tryThrowVulkanError(result);
    memcpy(pData, bufferData, bufferSize);
    vkUnmapMemory(vkDevice, stagingBuffer.vkBufferMemory);

    VkCommandBuffer vkCommandBuffer;
    VkCommandPool graphicsVkCommandPool = pGraphicsContext->graphicsVkCommandPool;
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = graphicsVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    result = vkAllocateCommandBuffers(vkDevice, &vkCommandBufferAllocateInfo, &vkCommandBuffer);
    tryThrowVulkanError(result);

    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = NULL,
        };
    result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    tryThrowVulkanError(result);

    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = offset,
        .size = bufferSize,
    };
    vkCmdCopyBuffer(vkCommandBuffer, stagingBuffer.vkBuffer, buffer.vkBuffer, 1, &copyRegion);

    vkEndCommandBuffer(vkCommandBuffer);
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .pWaitDstStageMask = NULL,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkCommandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = NULL,
    };
    VkQueue vkGraphicsQueue = pGraphicsContext->vkGraphicsQueue;
    vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkGraphicsQueue);

    vkFreeCommandBuffers(vkDevice, graphicsVkCommandPool, 1, &vkCommandBuffer);
    destroyBuffer(pGraphicsContext, stagingBuffer);
}

static void createMappedBuffer(GraphicsContext *pGraphicsContext, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, MappedBuffer *pMappedBuffer)
{
    createBuffer(pGraphicsContext, bufferSize, bufferUsageFlags, memoryPropertyFlags, &pMappedBuffer->buffer);
    vkMapMemory(pGraphicsContext->vkDevice, pMappedBuffer->buffer.vkBufferMemory, 0, bufferSize, 0, &pMappedBuffer->mapped);
}
static void destroyMappedBuffer(GraphicsContext *pGraphicsContext, MappedBuffer mappedBuffer)
{
    vkUnmapMemory(pGraphicsContext->vkDevice, mappedBuffer.buffer.vkBufferMemory);
    destroyBuffer(pGraphicsContext, mappedBuffer.buffer);
}
static void updateMappedBuffer(MappedBuffer *pMappedBuffer, VkDeviceSize offset, void *data, VkDeviceSize size)
{
    tickernelAssert(offset + size <= pMappedBuffer->buffer.size, "Mapped buffer update out of bounds!");
    memcpy(pMappedBuffer->mapped + offset, data, size);
}

static void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1};
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1}};

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        printf("Unsupported layout transition!\n");
        return;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier);

    vkEndCommandBuffer(commandBuffer);
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer};
    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

static void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1};
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {width, height, 1}};

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer};
    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void createGraphicsContext(int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight, GraphicsContext *pGraphicsContext)
{
    pGraphicsContext->targetSwapchainImageCount = targetSwapchainImageCount;
    pGraphicsContext->targetPresentMode = targetPresentMode;
    pGraphicsContext->vkInstance = vkInstance;
    pGraphicsContext->vkSurface = vkSurface;

    pickPhysicalDevice(pGraphicsContext);
    createLogicalDevice(pGraphicsContext);
    createSwapchain(pGraphicsContext, swapchainWidth, swapchainHeight);
    createSignals(pGraphicsContext);
    createCommandPools(pGraphicsContext);
    createVkCommandBuffers(pGraphicsContext);

    tickernelCreateDynamicArray(&pGraphicsContext->pRenderPassDynamicArray, sizeof(RenderPass *), 1);
    tickernelCreateDynamicArray(&pGraphicsContext->pDynamicAttachmentDynamicArray, sizeof(Attachment *), 1);
}

void updateGraphicsContext(GraphicsContext *pGraphicsContext, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    VkResult result = VK_SUCCESS;
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    // Wait for gpu
    result = vkWaitForFences(vkDevice, 1, &pGraphicsContext->renderFinishedFence, VK_TRUE, UINT64_MAX);
    tryThrowVulkanError(result);

    if (swapchainWidth != pGraphicsContext->swapchainWidth || swapchainHeight != pGraphicsContext->swapchainHeight)
    {
        printf("Recreate swapchain because of a size change: (%d, %d) to (%d, %d) \n",
               pGraphicsContext->swapchainWidth,
               pGraphicsContext->swapchainHeight,
               swapchainWidth,
               swapchainHeight);
        pGraphicsContext->swapchainWidth = swapchainWidth;
        pGraphicsContext->swapchainHeight = swapchainHeight;
        recreateSwapchain(pGraphicsContext);
    }
    else
    {

        result = vkAcquireNextImageKHR(vkDevice, pGraphicsContext->vkSwapchain, UINT64_MAX, pGraphicsContext->imageAvailableSemaphore, VK_NULL_HANDLE, &pGraphicsContext->swapchainIndex);
        if (result != VK_SUCCESS)
        {
            if (VK_ERROR_OUT_OF_DATE_KHR == result)
            {
                printf("Recreate swapchain because of result: %d\n", result);
                recreateSwapchain(pGraphicsContext);
            }
            else if (VK_SUBOPTIMAL_KHR == result)
            {
                result = vkResetFences(vkDevice, 1, &pGraphicsContext->renderFinishedFence);
                tryThrowVulkanError(result);
                recordCommandBuffer(pGraphicsContext);
                submitCommandBuffer(pGraphicsContext);
                present(pGraphicsContext);
            }
            else
            {
                tryThrowVulkanError(result);
            }
        }
        else
        {
            result = vkResetFences(vkDevice, 1, &pGraphicsContext->renderFinishedFence);
            tryThrowVulkanError(result);

            recordCommandBuffer(pGraphicsContext);
            submitCommandBuffer(pGraphicsContext);
            present(pGraphicsContext);
        }
    }
}

void destroyGraphicsContext(GraphicsContext *pGraphicsContext)
{
    tickernelDestroyDynamicArray(pGraphicsContext->pDynamicAttachmentDynamicArray);

    for (uint32_t i = 0; i < pGraphicsContext->pRenderPassDynamicArray.count; i++)
    {
        destroyRenderPass(pGraphicsContext, tickernelGetFromDynamicArray(&pGraphicsContext->pRenderPassDynamicArray, i));
    }
    tickernelDestroyDynamicArray(pGraphicsContext->pRenderPassDynamicArray);

    VkResult result = vkDeviceWaitIdle(pGraphicsContext->vkDevice);
    tryThrowVulkanError(result);

    destroyVkCommandBuffers(pGraphicsContext);
    destroyCommandPools(pGraphicsContext);
    destroySignals(pGraphicsContext);
    destroySwapchain(pGraphicsContext);
    destroyLogicalDevice(pGraphicsContext);
}

void createASTCGraphicsImage(GraphicsContext *pGraphicsContext, const char *fileName, GraphicsImage **ppGraphicsImage)
{
    GraphicsImage *pGraphicsImage = tickernelMalloc(sizeof(GraphicsImage));
    *ppGraphicsImage = pGraphicsImage;
    FILE *file = fopen(fileName, "rb");
    if (!file)
    {
        tickernelError("Failed to open ASTC file: %s\n", fileName);
    }

    ASTCHeader header;
    if (fread(&header, sizeof(ASTCHeader), 1, file) != 1)
    {
        fclose(file);
        tickernelError("Invalid ASTC file header: %s\n", fileName);
    }

    if (memcmp(header.magic, "ASTC", 4) != 0)
    {
        fclose(file);
        tickernelError("Not an ASTC file: %s\n", fileName);
    }

    uint32_t width = (header.width[0] << 16) | (header.width[1] << 8) | header.width[2];
    uint32_t height = (header.height[0] << 16) | (header.height[1] << 8) | header.height[2];
    uint8_t blockWidth = header.blockDimX;
    uint8_t blockHeight = header.blockDimY;

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file) - sizeof(ASTCHeader);
    fseek(file, sizeof(ASTCHeader), SEEK_SET);

    uint8_t *astcData = tickernelMalloc(fileSize);
    if (fread(astcData, 1, fileSize, file) != fileSize)
    {
        tickernelFree(astcData);
        fclose(file);
        tickernelError("Failed to read ASTC data: %s\n", fileName);
    }
    fclose(file);

    VkFormat astcFormat;
    switch ((blockWidth << 8) | blockHeight)
    {
    case 0x0404:
        astcFormat = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        break;
    case 0x0504:
        astcFormat = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        break;
    case 0x0505:
        astcFormat = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        break;
    case 0x0605:
        astcFormat = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        break;
    case 0x0606:
        astcFormat = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        break;
    case 0x0805:
        astcFormat = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        break;
    case 0x0806:
        astcFormat = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        break;
    case 0x0808:
        astcFormat = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        break;
    case 0x0A05:
        astcFormat = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        break;
    case 0x0A06:
        astcFormat = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        break;
    case 0x0A08:
        astcFormat = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        break;
    case 0x0A0A:
        astcFormat = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        break;
    case 0x0C08:
        astcFormat = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        break;
    case 0x0C0A:
        astcFormat = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        break;
    default:
        astcFormat = VK_FORMAT_UNDEFINED;
        tickernelFree(astcData);
        tickernelError("Unsupported ASTC block size: %dx%d\n", blockWidth, blockHeight);
    }

    VkFormatProperties props;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, astcFormat, &props);
    if (!(props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
    {
        tickernelFree(astcData);
        tickernelError("Device does not support ASTC format: %x\n", astcFormat);
    }

    VkExtent3D imageExtent = {width, height, 1};
    createGraphicsImage(
        pGraphicsContext,
        imageExtent,
        astcFormat,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        pGraphicsImage);

    Buffer stagingBuffer;
    createBuffer(
        pGraphicsContext,
        fileSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &stagingBuffer);

    updateBuffer(pGraphicsContext, &stagingBuffer, 0, fileSize, astcData);
    tickernelFree(astcData);
    VkCommandPool commandPool = pGraphicsContext->graphicsVkCommandPool;
    VkQueue graphicsQueue = pGraphicsContext->vkGraphicsQueue;
    transitionImageLayout(
        vkDevice, commandPool, graphicsQueue,
        pGraphicsImage->vkImage,
        astcFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(
        vkDevice, commandPool, graphicsQueue,
        stagingBuffer.vkBuffer,
        pGraphicsImage->vkImage,
        width, height);
    transitionImageLayout(
        vkDevice, commandPool, graphicsQueue,
        pGraphicsImage->vkImage,
        astcFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    destroyBuffer(pGraphicsContext, stagingBuffer);
}
void destroyASTCGraphicsImage(GraphicsContext *pGraphicsContext, GraphicsImage *pGraphicsImage)
{
    destroyGraphicsImage(pGraphicsContext, *pGraphicsImage);
    tickernelFree(pGraphicsImage);
}

void createSampler(GraphicsContext *pGraphicsContext, VkSamplerCreateInfo samplerCreateInfo, VkSampler *pVkSampler)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkResult result = vkCreateSampler(vkDevice, &samplerCreateInfo, NULL, pVkSampler);
    tryThrowVulkanError(result);
}
void destroySampler(GraphicsContext *pGraphicsContext, VkSampler vkSampler)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    vkDestroySampler(vkDevice, vkSampler, NULL);
}

void createPipeline(GraphicsContext *pGraphicsContext, uint32_t stageCount, const char **shaderPaths, MeshLayout meshLayout, VkPipelineInputAssemblyStateCreateInfo inputAssemblyState, VkPipelineViewportStateCreateInfo viewportState, VkPipelineRasterizationStateCreateInfo rasterizationState, VkPipelineMultisampleStateCreateInfo multisampleState, VkPipelineDepthStencilStateCreateInfo depthStencilState, VkPipelineColorBlendStateCreateInfo colorBlendState, VkPipelineDynamicStateCreateInfo dynamicState, RenderPass *pRenderPass, uint32_t subpassIndex, uint32_t pipelineIndex, Pipeline **ppPipeline)
{
    Pipeline *pPipeline = tickernelMalloc(sizeof(Pipeline));
    *ppPipeline = pPipeline;
    VkResult result = VK_SUCCESS;
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    SpvReflectShaderModule *spvReflectShaderModules = tickernelMalloc(sizeof(SpvReflectShaderModule) * stageCount);
    VkPipelineShaderStageCreateInfo *stages = tickernelMalloc(sizeof(VkPipelineShaderStageCreateInfo) * stageCount);

    uint32_t maxVertexInputAttributes = pGraphicsContext->vkPhysicalDeviceProperties.limits.maxVertexInputAttributes;
    uint32_t vkVertexInputAttributeDescriptionCount = 0;
    VkVertexInputAttributeDescription *vkVertexInputAttributeDescriptions = tickernelMalloc(sizeof(VkVertexInputAttributeDescription) * maxVertexInputAttributes);

    pPipeline->descriptorSetLayoutCount = 0;
    uint32_t maxDescriptorSetLayouts = pGraphicsContext->vkPhysicalDeviceProperties.limits.maxBoundDescriptorSets < SPV_REFLECT_MAX_DESCRIPTOR_SETS ? pGraphicsContext->vkPhysicalDeviceProperties.limits.maxBoundDescriptorSets : SPV_REFLECT_MAX_DESCRIPTOR_SETS;
    TickernelDynamicArray *bindingDynamicArrays = tickernelMalloc(sizeof(TickernelDynamicArray) * maxDescriptorSetLayouts);
    for (uint32_t i = 0; i < maxDescriptorSetLayouts; i++)
    {
        tickernelCreateDynamicArray(&bindingDynamicArrays[i], sizeof(VkDescriptorSetLayoutBinding), 1);
    }

    tickernelCreateDynamicArray(&pPipeline->vkDescriptorPoolSizeDynamicArray, sizeof(VkDescriptorPoolSize), 1);
    for (uint32_t stageIndex = 0; stageIndex < stageCount; stageIndex++)
    {
        const char *filePath = shaderPaths[stageIndex];
        FILE *file = fopen(filePath, "rb");
        if (!file)
        {
            tickernelError("Failed to open file: %s\n", filePath);
        }
        fseek(file, 0, SEEK_END);
        size_t shaderSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (shaderSize % 4 != 0)
        {
            fclose(file);
            tickernelError("Invalid SPIR-V file size: %s\n", filePath);
        }
        void *shaderCode = tickernelMalloc(shaderSize);
        size_t bytesRead = fread(shaderCode, 1, shaderSize, file);

        fclose(file);

        if (bytesRead != shaderSize)
        {
            tickernelError("Failed to read entire file: %s\n", filePath);
        }
        SpvReflectShaderModule spvReflectShaderModule;
        SpvReflectResult spvReflectResult = spvReflectCreateShaderModule(shaderSize, shaderCode, &spvReflectShaderModule);
        tickernelAssert(spvReflectResult == SPV_REFLECT_RESULT_SUCCESS, "Failed to reflect shader module: %s", shaderPaths[stageIndex]);
        tickernelFree(shaderCode);
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .codeSize = spvReflectGetCodeSize(&spvReflectShaderModule),
            .pCode = spvReflectGetCode(&spvReflectShaderModule),
        };
        VkShaderModule shaderModule;
        vkCreateShaderModule(vkDevice, &shaderModuleCreateInfo, NULL, &shaderModule);
        VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .stage = (VkShaderStageFlagBits)spvReflectShaderModule.shader_stage,
                .module = shaderModule,
                .pName = spvReflectShaderModule.entry_point_name,
                .pSpecializationInfo = NULL,
            };
        stages[stageIndex] = vkPipelineShaderStageCreateInfo;
        if (SPV_REFLECT_SHADER_STAGE_VERTEX_BIT == spvReflectShaderModule.shader_stage)
        {
            // handle pVertexAttributeDescriptions
            uint32_t inputVariableCount = spvReflectShaderModule.input_variable_count;
            for (uint32_t inputVariableIndex = 0; inputVariableIndex < inputVariableCount; inputVariableIndex++)
            {
                SpvReflectInterfaceVariable inputVariable = *spvReflectShaderModule.input_variables[inputVariableIndex];
                uint32_t binding = UINT32_MAX;
                FieldLayout fieldLayout;

                for (uint32_t vertexLayoutIndex = 0; vertexLayoutIndex < meshLayout.vertexLayoutCount; vertexLayoutIndex++)
                {
                    if (strcmp(meshLayout.vertexLayouts[vertexLayoutIndex].name, inputVariable.name) == 0)
                    {
                        binding = 0;
                        fieldLayout = meshLayout.vertexLayouts[vertexLayoutIndex];
                        break;
                    }
                }
                if (binding == UINT32_MAX)
                {
                    for (uint32_t instanceLayoutIndex = 0; instanceLayoutIndex < meshLayout.instanceLayoutCount; instanceLayoutIndex++)
                    {
                        if (strcmp(meshLayout.instanceLayouts[instanceLayoutIndex].name, inputVariable.name) == 0)
                        {
                            binding = 1;
                            fieldLayout = meshLayout.instanceLayouts[instanceLayoutIndex];
                            break;
                        }
                    }
                }

                tickernelAssert(binding != UINT32_MAX, "Failed to find binding for input variable: %s", inputVariable.name);
                if (SPV_REFLECT_TYPE_FLAG_MATRIX == inputVariable.type_description->type_flags)
                {
                    uint32_t columnCount = inputVariable.type_description->traits.numeric.matrix.column_count;
                    uint32_t rowCount = inputVariable.type_description->traits.numeric.matrix.row_count;
                    for (uint32_t rowIndex = 0; rowIndex < rowCount; rowIndex++)
                    {
                        VkVertexInputAttributeDescription vertexAttributeDescription = {
                            .location = inputVariable.location + rowIndex,
                            .binding = binding,
                            .format = (VkFormat)inputVariable.format,
                            .offset = fieldLayout.offset + rowIndex * inputVariable.numeric.scalar.width * columnCount / 8,
                        };
                        tickernelAssert(vkVertexInputAttributeDescriptionCount < maxVertexInputAttributes, "Too many vertex attributes");
                        vkVertexInputAttributeDescriptions[vkVertexInputAttributeDescriptionCount] = vertexAttributeDescription;
                        vkVertexInputAttributeDescriptionCount++;
                    }
                }
                else
                {
                    VkVertexInputAttributeDescription vertexAttributeDescription = {
                        .location = inputVariable.location,
                        .binding = binding,
                        .format = (VkFormat)inputVariable.format,
                        .offset = fieldLayout.offset,
                    };
                    tickernelAssert(vkVertexInputAttributeDescriptionCount < maxVertexInputAttributes, "Too many vertex attributes");
                    vkVertexInputAttributeDescriptions[vkVertexInputAttributeDescriptionCount] = vertexAttributeDescription;
                    vkVertexInputAttributeDescriptionCount++;
                }
            }
        }
        spvReflectShaderModules[stageIndex] = spvReflectShaderModule;

        // handle descriptor sets
        uint32_t descriptorSetCount = spvReflectShaderModule.descriptor_set_count;
        for (uint32_t descriptorSetIndex = 0; descriptorSetIndex < descriptorSetCount; descriptorSetIndex++)
        {
            SpvReflectDescriptorSet spvReflectDescriptorSet = spvReflectShaderModule.descriptor_sets[descriptorSetIndex];
            if (spvReflectDescriptorSet.set > pPipeline->descriptorSetLayoutCount)
            {
                pPipeline->descriptorSetLayoutCount = spvReflectDescriptorSet.set + 1;
            }
            for (uint32_t bindingIndex = 0; bindingIndex < spvReflectDescriptorSet.binding_count; bindingIndex++)
            {
                TickernelDynamicArray *pBindingDynamicArray = &bindingDynamicArrays[spvReflectDescriptorSet.set];
                bool bindingExists = false;
                SpvReflectDescriptorBinding *pCurrentBinding = spvReflectDescriptorSet.bindings[bindingIndex];
                for (uint32_t addedBindingIndex = 0; addedBindingIndex < pBindingDynamicArray->count; addedBindingIndex++)
                {
                    VkDescriptorSetLayoutBinding *pAddedBinding = tickernelGetFromDynamicArray(pBindingDynamicArray, addedBindingIndex);
                    if (pCurrentBinding->binding == pAddedBinding->binding)
                    {
                        bindingExists = true;
                        break;
                    }
                }
                if (bindingExists)
                {
                    printf("Binding %d already exists in descriptor set %d, skipping.\n", pCurrentBinding->binding, spvReflectDescriptorSet.set);
                }
                else
                {
                    VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = {
                        .binding = pCurrentBinding->binding,
                        .descriptorType = (VkDescriptorType)pCurrentBinding->descriptor_type,
                        .descriptorCount = pCurrentBinding->count,
                        .stageFlags = (VkShaderStageFlags)spvReflectShaderModule.shader_stage,
                        .pImmutableSamplers = NULL,
                    };

                    tickernelAddToDynamicArray(pBindingDynamicArray, &vkDescriptorSetLayoutBinding, pBindingDynamicArray->count);

                    bool hasFound = false;
                    for (uint32_t vkDescriptorPoolSizeIndex = 0; vkDescriptorPoolSizeIndex < pPipeline->vkDescriptorPoolSizeDynamicArray.count; vkDescriptorPoolSizeIndex++)
                    {
                        VkDescriptorPoolSize *currentPoolSize = tickernelGetFromDynamicArray(&pPipeline->vkDescriptorPoolSizeDynamicArray, vkDescriptorPoolSizeIndex);

                        if (currentPoolSize->type == (VkDescriptorType)pCurrentBinding->descriptor_type)
                        {
                            currentPoolSize->descriptorCount += pCurrentBinding->count;
                            hasFound = true;
                            break;
                        }
                    }

                    if (!hasFound)
                    {
                        // do nothing
                    }
                    else
                    {
                        VkDescriptorPoolSize vkDescriptorPoolSize = {
                            .type = (VkDescriptorType)pCurrentBinding->descriptor_type,
                            .descriptorCount = pCurrentBinding->count,
                        };
                        tickernelAddToDynamicArray(&pPipeline->vkDescriptorPoolSizeDynamicArray, &vkDescriptorPoolSize, pPipeline->vkDescriptorPoolSizeDynamicArray.count);
                    }
                }
            }
        }
    }

    // handle pVertexBindingDescriptions
    uint32_t vertexBindingDescriptionCount = 0;
    VkVertexInputBindingDescription *vertexBindingDescriptions = tickernelMalloc(sizeof(VkVertexInputBindingDescription) * 2);
    if (meshLayout.vertexLayoutCount > 0)
    {
        vertexBindingDescriptions[0] = (VkVertexInputBindingDescription){
            .binding = 0,
            .stride = meshLayout.vertexSize,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
        if (meshLayout.instanceLayoutCount > 0)
        {
            vertexBindingDescriptionCount = 2;
            vertexBindingDescriptions[1] = (VkVertexInputBindingDescription){
                .binding = 1,
                .stride = meshLayout.instanceSize,
                .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
            };
        }
        else
        {
            vertexBindingDescriptionCount = 1;
        }
    }
    else
    {
        vertexBindingDescriptionCount = 0;
    }

    pPipeline->descriptorSetLayouts = tickernelMalloc(sizeof(VkDescriptorSetLayout) * pPipeline->descriptorSetLayoutCount);
    for (uint32_t i = 0; i < pPipeline->descriptorSetLayoutCount; i++)
    {
        VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .bindingCount = bindingDynamicArrays[i].count,
            .pBindings = (VkDescriptorSetLayoutBinding *)bindingDynamicArrays[i].array,
        };
        vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &pPipeline->descriptorSetLayouts[i]);
    }
    VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = pPipeline->descriptorSetLayoutCount,
        .pSetLayouts = pPipeline->descriptorSetLayouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };
    result = vkCreatePipelineLayout(vkDevice, &vkPipelineLayoutCreateInfo, NULL, &pPipeline->vkPipelineLayout);
    tryThrowVulkanError(result);

    for (uint32_t i = 0; i < maxDescriptorSetLayouts; i++)
    {
        tickernelDestroyDynamicArray(bindingDynamicArrays[i]);
    }
    tickernelFree(bindingDynamicArrays);

    VkPipelineVertexInputStateCreateInfo vertexInputState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = vertexBindingDescriptionCount,
        .pVertexBindingDescriptions = vertexBindingDescriptions,
        .vertexAttributeDescriptionCount = vkVertexInputAttributeDescriptionCount,
        .pVertexAttributeDescriptions = vkVertexInputAttributeDescriptions,
    };

    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = stageCount,
        .pStages = stages,
        .pVertexInputState = &vertexInputState,
        .pInputAssemblyState = &inputAssemblyState,
        .pTessellationState = NULL,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizationState,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = &depthStencilState,
        .pColorBlendState = &colorBlendState,
        .pDynamicState = &dynamicState,
        .layout = pPipeline->vkPipelineLayout,
        .renderPass = pRenderPass->vkRenderPass,
        .subpass = subpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };
    result = vkCreateGraphicsPipelines(vkDevice, NULL, 1, &vkGraphicsPipelineCreateInfo, NULL, &pPipeline->vkPipeline);
    tryThrowVulkanError(result);

    tickernelFree(vertexBindingDescriptions);
    tickernelFree(vkVertexInputAttributeDescriptions);
    for (uint32_t i = 0; i < stageCount; i++)
    {
        vkDestroyShaderModule(vkDevice, stages[i].module, NULL);
    }
    tickernelFree(stages);
    for (uint32_t i = 0; i < stageCount; i++)
    {
        spvReflectDestroyShaderModule(&spvReflectShaderModules[i]);
    }
    tickernelFree(spvReflectShaderModules);

    tickernelCreateDynamicArray(&pPipeline->pMaterialDynamicArray, sizeof(Material *), 1);
}

void destroyPipeline(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass, uint32_t subpassIndex, Pipeline *pPipeline)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    for (uint32_t i = 0; i < pPipeline->pMaterialDynamicArray.count; i++)
    {
        Material *pMaterial = tickernelGetFromDynamicArray(&pPipeline->pMaterialDynamicArray, i);
        if (pMaterial->vkDescriptorSet != VK_NULL_HANDLE)
        {
            // destroyMaterial(pGraphicsContext, pPipeline, pMaterial);
        }
        tickernelFree(pMaterial);
    }

    tickernelDestroyDynamicArray(pPipeline->pMaterialDynamicArray);
    tickernelDestroyDynamicArray(pPipeline->vkDescriptorPoolSizeDynamicArray);
    vkDestroyPipeline(vkDevice, pPipeline->vkPipeline, NULL);
    vkDestroyPipelineLayout(vkDevice, pPipeline->vkPipelineLayout, NULL);
    for (uint32_t i = 0; i < pPipeline->descriptorSetLayoutCount; i++)
    {
        vkDestroyDescriptorSetLayout(vkDevice, pPipeline->descriptorSetLayouts[i], NULL);
    }
    tickernelFree(pPipeline->descriptorSetLayouts);
    tickernelFree(pPipeline);
}

void createMesh(GraphicsContext *pGraphicsContext, uint32_t vertexCount, VkDeviceSize vertexBufferSize, void *vertexBufferData, uint32_t indexCount, VkDeviceSize indexBufferSize, void *indexBufferData, uint32_t instanceCount, VkDeviceSize instanceSize, void *instanceBufferData, Mesh **ppMesh)
{
    Mesh *pMesh = tickernelMalloc(sizeof(Mesh));

    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    VkCommandPool graphicsVkCommandPool = pGraphicsContext->graphicsVkCommandPool;
    VkQueue vkGraphicsQueue = pGraphicsContext->vkGraphicsQueue;

    pMesh->vertexCount = vertexCount;
    if (vertexCount > 0)
    {
        createBuffer(pGraphicsContext, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pMesh->vertexBuffer);

        updateBufferWithStagingBuffer(pGraphicsContext, 0, vertexBufferSize, vertexBufferData, pMesh->vertexBuffer);

        pMesh->indexCount = indexCount;
        if (indexCount > 0)
        {
            createBuffer(pGraphicsContext, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pMesh->indexBuffer);
            updateBufferWithStagingBuffer(pGraphicsContext, 0, indexBufferSize, indexBufferData, pMesh->indexBuffer);
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
            VkDeviceSize instanceBufferSize = instanceSize * instanceCount;
            createBuffer(pGraphicsContext, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pMesh->instanceMappedBuffer.buffer);
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
    *ppMesh = pMesh;
}
void destroyMesh(GraphicsContext *pGraphicsContext, Mesh *pMesh)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;

    if (pMesh->vertexCount > 0)
    {
        destroyBuffer(pGraphicsContext, pMesh->vertexBuffer);
    }
    else
    {
        // continue;
    }
    if (pMesh->indexCount > 0)
    {
        destroyBuffer(pGraphicsContext, pMesh->indexBuffer);
    }
    else
    {
        // continue;
    }

    if (pMesh->instanceCount > 0)
    {
        vkUnmapMemory(vkDevice, pMesh->instanceMappedBuffer.buffer.vkBufferMemory);
        destroyBuffer(pGraphicsContext, pMesh->instanceMappedBuffer.buffer);
    }
    else
    {
        // continue;
    }

    tickernelFree(pMesh);
}
void updateMeshInstanceBuffer(GraphicsContext *pGraphicsContext, Mesh *pMesh, VkDeviceSize instanceSize, void *instanceBufferData, uint32_t instanceCount)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    VkDeviceSize instanceBufferSize = instanceSize * instanceCount;
    if (0 == pMesh->maxInstanceCount)
    {
        pMesh->maxInstanceCount = instanceCount;
        pMesh->instanceCount = instanceCount;

        createMappedBuffer(pGraphicsContext, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pMesh->instanceMappedBuffer);
        updateMappedBuffer(pMesh->instanceMappedBuffer.mapped, 0, instanceBufferData, instanceBufferSize);
    }
    else if (instanceCount <= pMesh->maxInstanceCount)
    {
        pMesh->instanceCount = instanceCount;
        updateMappedBuffer(pMesh->instanceMappedBuffer.mapped, 0, instanceBufferData, instanceBufferSize);
    }
    else
    {
        destroyMappedBuffer(pGraphicsContext, pMesh->instanceMappedBuffer);
        pMesh->maxInstanceCount = instanceCount;
        pMesh->instanceCount = instanceCount;
        createMappedBuffer(pGraphicsContext, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pMesh->instanceMappedBuffer);
        updateMappedBuffer(pMesh->instanceMappedBuffer.mapped, 0, instanceBufferData, instanceBufferSize);
    }
}

void createRenderPass(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **pAttachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass)
{
    RenderPass *pRenderPass = tickernelMalloc(sizeof(RenderPass));

    VkDevice vkDevice = pGraphicsContext->vkDevice;
    bool useSwapchain = false;
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        Attachment *pAttachment = pAttachments[i];
        if (ATTACHMENT_TYPE_SWAPCHAIN == pAttachment->attachmentType)
        {
            useSwapchain = true;
            vkAttachmentDescriptions[i].format = pGraphicsContext->surfaceFormat.format;
        }
        else if (ATTACHMENT_TYPE_DYNAMIC == pAttachment->attachmentType)
        {
            vkAttachmentDescriptions[i].format = pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage.vkFormat;
        }
        else
        {
            vkAttachmentDescriptions[i].format = pAttachment->attachmentContent.fixedAttachmentContent.graphicsImage.vkFormat;
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
        pRenderPass->vkFramebufferCount = pGraphicsContext->swapchainImageCount;
        pRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer) * pRenderPass->vkFramebufferCount);
        for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
        {
            createFramebuffer(pGraphicsContext, pRenderPass->vkFramebufferCount, pAttachments, pRenderPass, &pRenderPass->vkFramebuffers[i]);
        }
    }
    else
    {
        pRenderPass->vkFramebufferCount = 1;
        pRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer));
        createFramebuffer(pGraphicsContext, pRenderPass->vkFramebufferCount, pAttachments, pRenderPass, &pRenderPass->vkFramebuffers[0]);
    }

    pRenderPass->subpassCount = vkRenderPassCreateInfo.subpassCount;
    pRenderPass->subpasses = tickernelMalloc(sizeof(Subpass) * pRenderPass->subpassCount);
    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        tickernelCreateDynamicArray(&pSubpass->pPipelineDynamicArray, sizeof(Pipeline *), 4);
    }

    tickernelAddToDynamicArray(&pGraphicsContext->pRenderPassDynamicArray, pRenderPass, renderPassIndex);
    *ppRenderPass = pRenderPass;
}
void destroyRenderPass(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass)
{
    tickernelRemoveFromDynamicArray(&pGraphicsContext->pRenderPassDynamicArray, pRenderPass);

    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        for (uint32_t j = 0; j < pSubpass->pPipelineDynamicArray.count; j++)
        {
            Pipeline *pPipeline = tickernelGetFromDynamicArray(&pSubpass->pPipelineDynamicArray, j);
            destroyPipeline(pGraphicsContext, pRenderPass, i, pPipeline);
        }
        tickernelDestroyDynamicArray(pSubpass->pPipelineDynamicArray);
    }

    tickernelFree(pRenderPass->subpasses);

    for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
    {
        destroyFramebuffer(pGraphicsContext, pRenderPass->vkFramebuffers[i]);
    }
    tickernelFree(pRenderPass->vkFramebuffers);

    vkDestroyRenderPass(pGraphicsContext->vkDevice, pRenderPass->vkRenderPass, NULL);

    tickernelFree(pRenderPass);
}

void createDynamicAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment **ppAttachment)
{
    Attachment *pAttachment = tickernelMalloc(sizeof(Attachment));
    pAttachment->attachmentType = ATTACHMENT_TYPE_DYNAMIC;
    VkExtent3D vkExtent3D =
        {
            .width = (uint32_t)(pGraphicsContext->swapchainWidth * scaler),
            .height = (uint32_t)(pGraphicsContext->swapchainHeight * scaler),
            .depth = 1,
        };
    createGraphicsImage(
        pGraphicsContext,
        vkExtent3D,
        vkFormat,
        vkImageUsageFlags,
        vkMemoryPropertyFlags,
        vkImageAspectFlags,
        &pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage);

    pAttachment->attachmentContent.dynamicAttachmentContent.scaler = scaler;
    tickernelAddToDynamicArray(&pGraphicsContext->pDynamicAttachmentDynamicArray, &pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage, pGraphicsContext->pDynamicAttachmentDynamicArray.count);

    *ppAttachment = pAttachment;
}
void destroyDynamicAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment)
{
    tickernelRemoveFromDynamicArray(&pGraphicsContext->pDynamicAttachmentDynamicArray, &pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage);
    destroyGraphicsImage(pGraphicsContext, pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage);
    tickernelFree(pAttachment);
}

void createFixedAttachment(GraphicsContext *pGraphicsContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment **ppAttachment)
{
    Attachment *pAttachment = tickernelMalloc(sizeof(Attachment));
    *ppAttachment = pAttachment;
    pAttachment->attachmentType = ATTACHMENT_TYPE_DYNAMIC;
    VkExtent3D vkExtent3D =
        {
            .width = width,
            .height = height,
            .depth = 1,
        };
    createGraphicsImage(
        pGraphicsContext,
        vkExtent3D,
        vkFormat,
        vkImageUsageFlags,
        vkMemoryPropertyFlags,
        vkImageAspectFlags,
        &pAttachment->attachmentContent.fixedAttachmentContent.graphicsImage);

    pAttachment->attachmentContent.fixedAttachmentContent.width = width;
    pAttachment->attachmentContent.fixedAttachmentContent.height = height;
}
void destroyFixedAttachment(GraphicsContext *pGraphicsContext, Attachment *pAttachment)
{
    destroyGraphicsImage(pGraphicsContext, pAttachment->attachmentContent.dynamicAttachmentContent.graphicsImage);
    tickernelFree(pAttachment);
}

void findSupportedFormat(GraphicsContext *pGraphicsContext, VkFormat *candidates, uint32_t candidatesCount, VkFormatFeatureFlags features, VkImageTiling tiling, VkFormat *pVkFormat)
{
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    for (uint32_t i = 0; i < candidatesCount; i++)
    {
        VkFormat format = candidates[i];
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, format, &properties);
        if ((tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) ||
            (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features))
        {
            *pVkFormat = format;
            return;
        }
        else
        {
            // continue;
        }
    }
    *pVkFormat = VK_FORMAT_UNDEFINED;
}

void createUniformBuffer(GraphicsContext *pGraphicsContext, VkDeviceSize vkBufferSize, MappedBuffer **ppUniformBuffer)
{
    *ppUniformBuffer = tickernelMalloc(sizeof(MappedBuffer));
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    createMappedBuffer(pGraphicsContext, vkBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *ppUniformBuffer);
}
void destroyUniformBuffer(GraphicsContext *pGraphicsContext, MappedBuffer *pUniformBuffer)
{
    destroyMappedBuffer(pGraphicsContext, *pUniformBuffer);
    tickernelFree(pUniformBuffer);
}
void updateUniformBuffer(GraphicsContext *pGraphicContext, MappedBuffer *pUniformBuffer, size_t offset, void *data, size_t size)
{
    updateMappedBuffer(pUniformBuffer->mapped, offset, data, size);
}

void createMaterial(GraphicsContext *pGraphicsContext, Pipeline *pPipeline, Material **ppMaterial)
{
    Material *pMaterial = tickernelMalloc(sizeof(Material));
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkDescriptorPoolCreateInfo descriptorPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = pPipeline->vkDescriptorPoolSizeDynamicArray.count,
        .pPoolSizes = pPipeline->vkDescriptorPoolSizeDynamicArray.array,
        .maxSets = 1,
    };
    vkCreateDescriptorPool(vkDevice, &descriptorPoolInfo, NULL, &pMaterial->vkDescriptorPool);
    vkAllocateDescriptorSets(vkDevice, &(VkDescriptorSetAllocateInfo){
                                           .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                           .pNext = NULL,
                                           .descriptorPool = pMaterial->vkDescriptorPool,
                                           .descriptorSetCount = pPipeline->descriptorSetLayoutCount,
                                           .pSetLayouts = pPipeline->descriptorSetLayouts,
                                       },
                             &pMaterial->vkDescriptorSet);

    *ppMaterial = pMaterial;
    VkWriteDescriptorSet writeDescriptorSet = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = NULL,
        .dstSet = pMaterial->vkDescriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = NULL,
        .pBufferInfo = NULL,
        .pTexelBufferView = NULL,
    };
    vkUpdateDescriptorSets(vkDevice, 1, &writeDescriptorSet, 0, NULL);
}

void destroyMaterial(GraphicsContext *pGraphicsContext, Pipeline *pPipeline, Material *pMaterial)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;

    if (pMaterial->vkDescriptorSet != VK_NULL_HANDLE)
    {
        vkFreeDescriptorSets(vkDevice, pMaterial->vkDescriptorPool, 1, &pMaterial->vkDescriptorSet);
    }
    if (pMaterial->vkDescriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(vkDevice, pMaterial->vkDescriptorPool, NULL);
    }
    for (uint32_t i = 0; i < pPipeline->pMaterialDynamicArray.count; i++)
    {
        Material *pCurrentMaterial = tickernelGetFromDynamicArray(&pPipeline->pMaterialDynamicArray, i);
        if (pCurrentMaterial == pMaterial)
        {
            tickernelRemoveFromDynamicArray(&pPipeline->pMaterialDynamicArray, pCurrentMaterial);
            break;
        }
    }
    tickernelFree(pMaterial);
}
