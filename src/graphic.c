#include "graphic.h"

static void tryThrowVulkanError(VkResult vkResult)
{
    if (vkResult != VK_SUCCESS)
    {
        tickernelError("Vulkan error code: %d\n", vkResult);
    }
}

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
    pGraphicContext->swapchainAttachmentContent.attachmentType = ATTACHMENT_TYPE_SWAPCHAIN;
};
static void destroySwapchain(GraphicContext *pGraphicContext)
{
    for (uint32_t i = 0; i < pGraphicContext->swapchainImageCount; i++)
    {
        destroyImageView(pGraphicContext->vkDevice, pGraphicContext->swapchainImageViews[i]);
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

    for (uint32_t i = 0; i < pGraphicContext->renderPasseDynamicArray.length; i++)
    {
        RenderPass *pRenderPass = pGraphicContext->renderPasseDynamicArray.array[i];
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
    uint32_t width = 0;
    uint32_t height = 0;
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
            dynamicAttachmentContent dynamicAttachmentContent = attachment.attachmentContent.dynamicAttachmentContent;
            attachmentVkImageViews[j] = dynamicAttachmentContent.graphicImage.vkImageView;
            width = pGraphicContext->swapchainWidth * dynamicAttachmentContent.scaler;
            height = pGraphicContext->swapchainHeight * dynamicAttachmentContent.scaler;
        }
        else
        {
            fixedAttachmentContent fixedAttachmentContent = attachment.attachmentContent.fixedAttachmentContent;
            attachmentVkImageViews[j] = fixedAttachmentContent.graphicImage.vkImageView;
            width = fixedAttachmentContent.width;
            height = fixedAttachmentContent.height;
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

static void createGraphicImage(GraphicContext *pGraphicContext, VkExtent3D vkExtent3D, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, GraphicImage *pGraphicImage)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicContext->vkPhysicalDevice;
    pGraphicImage->vkFormat = vkFormat;
    createImage(vkDevice, vkPhysicalDevice, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, vkMemoryPropertyFlags, &pGraphicImage->vkImage, &pGraphicImage->vkDeviceMemory);
    createImageView(vkDevice, pGraphicImage->vkImage, pGraphicImage->vkFormat, vkImageAspectFlags, &pGraphicImage->vkImageView);
}
static void destroyGraphicImage(GraphicContext *pGraphicContext, GraphicImage graphicImage)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    vkDestroyImageView(vkDevice, graphicImage.vkImageView, NULL);
    destroyImage(vkDevice, graphicImage.vkImage, graphicImage.vkDeviceMemory);
}

static void createBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, Buffer *pBuffer)
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
}
static void destroyBuffer(VkDevice vkDevice, Buffer buffer)
{
    vkFreeMemory(vkDevice, buffer.vkBufferMemory, NULL);
    vkDestroyBuffer(vkDevice, buffer.vkBuffer, NULL);
}

static void updateBufferWithStagingBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer vkBuffer)
{
    Buffer stagingBuffer;
    createBuffer(vkDevice, vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer);

    void *pData;
    VkResult result = vkMapMemory(vkDevice, stagingBuffer.vkBufferMemory, offset, bufferSize, 0, &pData);
    tryThrowVulkanError(result);
    memcpy(pData, bufferData, bufferSize);
    vkUnmapMemory(vkDevice, stagingBuffer.vkBufferMemory);

    VkCommandBuffer vkCommandBuffer;
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = graphicVkCommandPool,
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
    vkCmdCopyBuffer(vkCommandBuffer, stagingBuffer.vkBuffer, vkBuffer, 1, &copyRegion);

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
    vkQueueSubmit(vkGraphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkGraphicQueue);

    vkFreeCommandBuffers(vkDevice, graphicVkCommandPool, 1, &vkCommandBuffer);
    destroyBuffer(vkDevice, stagingBuffer);
}

static void updateBuffer(VkDevice vkDevice, VkDeviceMemory vkBufferMemory, VkDeviceSize offset, VkDeviceSize bufferSize, void *bufferData)
{
    void *data;
    vkMapMemory(vkDevice, vkBufferMemory, offset, bufferSize, 0, &data);
    memcpy(data, bufferData, bufferSize);
    vkUnmapMemory(vkDevice, vkBufferMemory);
}

static void createMappedBuffer(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, MappedBuffer *pMappedBuffer)
{
    createBuffer(vkDevice, vkPhysicalDevice, bufferSize, bufferUsageFlags, memoryPropertyFlags, &pMappedBuffer->buffer);
    vkMapMemory(vkDevice, pMappedBuffer->buffer.vkBufferMemory, 0, bufferSize, 0, &pMappedBuffer->mapped);
}
static void destroyMappedBuffer(VkDevice vkDevice, MappedBuffer mappedBuffer)
{
    vkUnmapMemory(vkDevice, mappedBuffer.buffer.vkBufferMemory);
    destroyBuffer(vkDevice, mappedBuffer.buffer);
}

static void updateMappedBuffer(MappedBuffer *pMappedBuffer, void *data, VkDeviceSize size)
{
    memcpy(pMappedBuffer->mapped, data, size);
}

static void createVkShaderModule(VkDevice vkDevice, const char *filePath, VkShaderModule *pVkShaderModule)
{
    FILE *pFile = fopen(filePath, "rb");
    if (NULL == pFile)
    {
        tickernelError("Failed to read file with path: %s\n", filePath);
    }
    else
    {
        fseek(pFile, 0, SEEK_END);
        size_t fileLength = ftell(pFile);
        rewind(pFile);

        uint32_t *pCode = tickernelMalloc(fileLength);
        size_t codeSize = fread(pCode, 1, fileLength, pFile);

        fclose(pFile);
        if (codeSize == fileLength)
        {
            printf("Succeeded to read file!\n");
            VkShaderModuleCreateInfo shaderModuleCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .codeSize = codeSize,
                .pCode = pCode,
            };
            VkResult result = vkCreateShaderModule(vkDevice, &shaderModuleCreateInfo, NULL, pVkShaderModule);
            tryThrowVulkanError(result);
            tickernelFree(pCode);
        }
        else
        {
            printf("Failed to read file codeSize:%zu fileLength:%zu\n", codeSize, fileLength);
        }
    }
}
static void destroyVkShaderModule(VkDevice vkDevice, VkShaderModule vkShaderModule)
{
    vkDestroyShaderModule(vkDevice, vkShaderModule, NULL);
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

    tickernelCreateDynamicArray(&pGraphicContext->renderPasseDynamicArray, 1);
    tickernelCreateDynamicArray(&pGraphicContext->dynamicAttachmentDynamicArray, 1);
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
    tickernelDestroyDynamicArray(pGraphicContext->dynamicAttachmentDynamicArray);

    for (uint32_t i = 0; i < pGraphicContext->renderPasseDynamicArray.length; i++)
    {
        destroyRenderPass(pGraphicContext, pGraphicContext->renderPasseDynamicArray.array[i]);
    }
    tickernelDestroyDynamicArray(pGraphicContext->renderPasseDynamicArray);

    VkResult result = vkDeviceWaitIdle(pGraphicContext->vkDevice);
    tryThrowVulkanError(result);

    destroyVkCommandBuffers(pGraphicContext);
    destroyCommandPools(pGraphicContext);
    destroySignals(pGraphicContext);
    destroySwapchain(pGraphicContext);
    destroyLogicalDevice(pGraphicContext);

    tickernelFree(pGraphicContext);
}

void createASTCGraphicImage(GraphicContext *pGraphicContext, const char *fileName, GraphicImage *pGraphicImage)
{
    pGraphicImage = tickernelMalloc(sizeof(GraphicImage));
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
    VkPhysicalDevice vkPhysicalDevice = pGraphicContext->vkPhysicalDevice;
    VkDevice vkDevice = pGraphicContext->vkDevice;
    vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, astcFormat, &props);
    if (!(props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
    {
        tickernelFree(astcData);
        tickernelError("Device does not support ASTC format: %x\n", astcFormat);
    }

    VkExtent3D imageExtent = {width, height, 1};
    createGraphicImage(
        pGraphicContext,
        imageExtent,
        astcFormat,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        pGraphicImage);

    Buffer stagingBuffer;
    createBuffer(
        vkDevice, vkPhysicalDevice,
        fileSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &stagingBuffer);

    updateBuffer(vkDevice, stagingBuffer.vkBufferMemory, 0, fileSize, astcData);
    tickernelFree(astcData);
    VkCommandPool commandPool = pGraphicContext->graphicVkCommandPool;
    VkQueue graphicQueue = pGraphicContext->vkGraphicQueue;
    transitionImageLayout(
        vkDevice, commandPool, graphicQueue,
        pGraphicImage->vkImage,
        astcFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(
        vkDevice, commandPool, graphicQueue,
        stagingBuffer.vkBuffer,
        pGraphicImage->vkImage,
        width, height);
    transitionImageLayout(
        vkDevice, commandPool, graphicQueue,
        pGraphicImage->vkImage,
        astcFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    destroyBuffer(vkDevice, stagingBuffer);
}
void destroyASTCGraphicImage(GraphicContext *pGraphicContext, GraphicImage* pGraphicImage)
{
    destroyGraphicImage(pGraphicContext, *pGraphicImage);
    tickernelFree(pGraphicImage);
}

void createSampler(GraphicContext *pGraphicContext, VkSamplerCreateInfo samplerCreateInfo, VkSampler *pVkSampler)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    VkResult result = vkCreateSampler(vkDevice, &samplerCreateInfo, NULL, pVkSampler);
    tryThrowVulkanError(result);
}
void destroySampler(GraphicContext *pGraphicContext, VkSampler vkSampler)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    vkDestroySampler(vkDevice, vkSampler, NULL);
}

void createPipeline(GraphicContext *pGraphicContext, uint32_t stageCount, char **shaderPaths, VkPipelineShaderStageCreateInfo *stages, VkPipelineVertexInputStateCreateInfo *pVertexInputState, VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState, VkPipelineViewportStateCreateInfo *pViewportState, VkPipelineRasterizationStateCreateInfo *pRasterizationState, VkPipelineMultisampleStateCreateInfo *pMultisampleState, VkPipelineDepthStencilStateCreateInfo *pDepthStencilState, VkPipelineColorBlendStateCreateInfo *pColorBlendState, VkPipelineDynamicStateCreateInfo *pDynamicState, VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo, RenderPass *pRenderPass, uint32_t subpassIndex, uint32_t vkDescriptorPoolSizeCount, VkDescriptorPoolSize *vkDescriptorPoolSizes, uint32_t pipelineIndex, Pipeline *pPipeline)
{
    pPipeline = tickernelMalloc(sizeof(Pipeline));

    VkPipelineCache pipelineCache = NULL;
    VkDevice vkDevice = pGraphicContext->vkDevice;
    for (uint32_t i = 0; i < stageCount; i++)
    {
        createVkShaderModule(vkDevice, shaderPaths[i], &stages[i].module);
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

    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = stageCount,
        .pStages = stages,
        .pVertexInputState = pVertexInputState,
        .pInputAssemblyState = pInputAssemblyState,
        .pTessellationState = NULL,
        .pViewportState = pViewportState,
        .pRasterizationState = pRasterizationState,
        .pMultisampleState = pMultisampleState,
        .pDepthStencilState = pDepthStencilState,
        .pColorBlendState = pColorBlendState,
        .pDynamicState = pDynamicState,
        .layout = pPipeline->vkPipelineLayout,
        .renderPass = pRenderPass->vkRenderPass,
        .subpass = subpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };
    result = vkCreateGraphicsPipelines(vkDevice, pipelineCache, 1, &vkGraphicsPipelineCreateInfo, NULL, &pPipeline->vkPipeline);
    tryThrowVulkanError(result);

    for (uint32_t i = 0; i < vkGraphicsPipelineCreateInfo.stageCount; i++)
    {
        destroyVkShaderModule(vkDevice, stages[i].module);
    }

    pPipeline->vkDescriptorPoolSizeCount = vkDescriptorPoolSizeCount;
    pPipeline->vkDescriptorPoolSizes = tickernelMalloc(sizeof(VkDescriptorPoolSize) * pPipeline->vkDescriptorPoolSizeCount);
    memcpy(pPipeline->vkDescriptorPoolSizes, vkDescriptorPoolSizes, sizeof(VkDescriptorPoolSize) * pPipeline->vkDescriptorPoolSizeCount);
    tickernelCreateDynamicArray(&pPipeline->materialDynamicArray, 1);

    tickernelAddToDynamicArray(&pRenderPass->subpasses[subpassIndex].pipelineDynamicArray, pPipeline, pipelineIndex);
}
void destroyPipeline(GraphicContext *pGraphicContext, RenderPass *pRenderPass, uint32_t subpassIndex, Pipeline *pPipeline)
{
    tickernelRemoveFromDynamicArray(&pRenderPass->subpasses[subpassIndex].pipelineDynamicArray, pPipeline);

    VkDevice vkDevice = pGraphicContext->vkDevice;
    for (uint32_t i = 0; i < pPipeline->materialDynamicArray.length; i++)
    {
        destroyMaterial(pGraphicContext, pPipeline, pPipeline->materialDynamicArray.array[i]);
    }
    tickernelDestroyDynamicArray(pPipeline->materialDynamicArray);

    tickernelFree(pPipeline->vkDescriptorPoolSizes);

    vkDestroyPipeline(vkDevice, pPipeline->vkPipeline, NULL);
    vkDestroyPipelineLayout(vkDevice, pPipeline->vkPipelineLayout, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pPipeline->descriptorSetLayout, NULL);

    tickernelFree(pPipeline);
}

void createMaterial(GraphicContext *pGraphicContext, Pipeline *pPipeline, VkWriteDescriptorSet *vkWriteDescriptorSets, uint32_t vkWriteDescriptorSetCount, Material *pMaterial)
{
    pMaterial = tickernelMalloc(sizeof(Material));
    VkDevice vkDevice = pGraphicContext->vkDevice;
    tickernelCreateDynamicArray(&pMaterial->meshDynamicArray, 1);
    pMaterial->vkDescriptorSet = VK_NULL_HANDLE;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = pPipeline->vkDescriptorPoolSizeCount,
        .pPoolSizes = pPipeline->vkDescriptorPoolSizes,
    };
    VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &pMaterial->vkDescriptorPool);
    tryThrowVulkanError(result);

    // Create vkDescriptorSet
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = pMaterial->vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pPipeline->descriptorSetLayout,
    };
    result = vkAllocateDescriptorSets(vkDevice, &descriptorSetAllocateInfo, &pMaterial->vkDescriptorSet);
    tryThrowVulkanError(result);

    vkUpdateDescriptorSets(vkDevice, vkWriteDescriptorSetCount, vkWriteDescriptorSets, 0, NULL);

    tickernelAddToDynamicArray(&pPipeline->materialDynamicArray, pMaterial, pPipeline->materialDynamicArray.length);
}
void destroyMaterial(GraphicContext *pGraphicContext, Pipeline *pPipeline, Material *pMaterial)
{
    tickernelRemoveFromDynamicArray(&pPipeline->materialDynamicArray, pMaterial);

    VkDevice vkDevice = pGraphicContext->vkDevice;
    vkFreeDescriptorSets(vkDevice, pMaterial->vkDescriptorPool, 1, &pMaterial->vkDescriptorSet);
    vkDestroyDescriptorPool(vkDevice, pMaterial->vkDescriptorPool, NULL);

    tickernelDestroyDynamicArray(pMaterial->meshDynamicArray);
    tickernelFree(pMaterial);
}

void createMesh(GraphicContext *pGraphicContext, uint32_t vertexCount, VkDeviceSize vertexBufferSize, void *vertexBufferData, uint32_t indexCount, VkDeviceSize indexBufferSize, void *indexBufferData, uint32_t instanceCount, VkDeviceSize instanceBufferSize, void *instanceBufferData, Mesh *pMesh)
{
    pMesh = tickernelMalloc(sizeof(Mesh));
    VkDevice vkDevice = pGraphicContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicContext->vkPhysicalDevice;
    VkCommandPool graphicVkCommandPool = pGraphicContext->graphicVkCommandPool;
    VkQueue vkGraphicQueue = pGraphicContext->vkGraphicQueue;

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
void destroyMesh(GraphicContext *pGraphicContext, Mesh *pMesh)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;

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
void updateMeshInstanceBuffer(GraphicContext *pGraphicContext, Mesh *pMesh, VkDeviceSize instanceBufferSize, void *instanceBufferData, uint32_t instanceCount)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    VkPhysicalDevice vkPhysicalDevice = pGraphicContext->vkPhysicalDevice;

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

void createRenderPass(GraphicContext *pGraphicContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment *attachments, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass *pRenderPass)
{
    pRenderPass = tickernelMalloc(sizeof(RenderPass));

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
            vkAttachmentDescriptions[i].format = attachment.attachmentContent.dynamicAttachmentContent.graphicImage.vkFormat;
        }
        else
        {
            vkAttachmentDescriptions[i].format = attachment.attachmentContent.fixedAttachmentContent.graphicImage.vkFormat;
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
        tickernelCreateDynamicArray(&pSubpass->pipelineDynamicArray, 1);
    }

    tickernelAddToDynamicArray(&pGraphicContext->renderPasseDynamicArray, pRenderPass, renderPassIndex);
}
void destroyRenderPass(GraphicContext *pGraphicContext, RenderPass *pRenderPass)
{
    tickernelRemoveFromDynamicArray(&pGraphicContext->renderPasseDynamicArray, pRenderPass);

    for (uint32_t i = 0; i < pRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pRenderPass->subpasses[i];
        for (uint32_t j = 0; j < pSubpass->pipelineDynamicArray.length; j++)
        {
            Pipeline *pPipeline = pSubpass->pipelineDynamicArray.array[j];
            destroyPipeline(pGraphicContext, pRenderPass, i, pPipeline);
        }
        tickernelDestroyDynamicArray(pSubpass->pipelineDynamicArray);
    }

    tickernelFree(pRenderPass->subpasses);

    for (uint32_t i = 0; i < pRenderPass->vkFramebufferCount; i++)
    {
        destroyFramebuffer(pGraphicContext, pRenderPass->vkFramebuffers[i]);
    }
    tickernelFree(pRenderPass->vkFramebuffers);

    vkDestroyRenderPass(pGraphicContext->vkDevice, pRenderPass->vkRenderPass, NULL);

    tickernelFree(pRenderPass);
}

void createDynamicAttachment(GraphicContext *pGraphicContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, float scaler, Attachment *pAttachment)
{
    pAttachment = tickernelMalloc(sizeof(Attachment));
    pAttachment->attachmentType = ATTACHMENT_TYPE_DYNAMIC;
    VkExtent3D vkExtent3D =
        {
            .width = (uint32_t)(pGraphicContext->swapchainWidth * scaler),
            .height = (uint32_t)(pGraphicContext->swapchainHeight * scaler),
            .depth = 1,
        };
    createGraphicImage(
        pGraphicContext,
        vkExtent3D,
        vkFormat,
        vkImageUsageFlags,
        vkMemoryPropertyFlags,
        vkImageAspectFlags,
        &pAttachment->attachmentContent.dynamicAttachmentContent.graphicImage);

    pAttachment->attachmentContent.dynamicAttachmentContent.scaler = scaler;
    tickernelAddToDynamicArray(&pGraphicContext->dynamicAttachmentDynamicArray, &pAttachment->attachmentContent.dynamicAttachmentContent.graphicImage, pGraphicContext->dynamicAttachmentDynamicArray.length);
}
void destroyDynamicAttachment(GraphicContext *pGraphicContext, Attachment *pAttachment)
{
    tickernelRemoveFromDynamicArray(&pGraphicContext->dynamicAttachmentDynamicArray, &pAttachment->attachmentContent.dynamicAttachmentContent.graphicImage);
    destroyGraphicImage(pGraphicContext, pAttachment->attachmentContent.dynamicAttachmentContent.graphicImage);
    tickernelFree(pAttachment);
}

void createFixedAttachment(GraphicContext *pGraphicContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment *pAttachment)
{
    pAttachment = tickernelMalloc(sizeof(Attachment));
    pAttachment->attachmentType = ATTACHMENT_TYPE_DYNAMIC;
    VkExtent3D vkExtent3D =
        {
            .width = width,
            .height = height,
            .depth = 1,
        };
    createGraphicImage(
        pGraphicContext,
        vkExtent3D,
        vkFormat,
        vkImageUsageFlags,
        vkMemoryPropertyFlags,
        vkImageAspectFlags,
        &pAttachment->attachmentContent.fixedAttachmentContent.graphicImage);

    pAttachment->attachmentContent.fixedAttachmentContent.width = width;
    pAttachment->attachmentContent.fixedAttachmentContent.height = height;
}
void destroyFixedAttachment(GraphicContext *pGraphicContext, Attachment *pAttachment)
{
    destroyGraphicImage(pGraphicContext, pAttachment->attachmentContent.dynamicAttachmentContent.graphicImage);
    tickernelFree(pAttachment);
}

void findSupportedFormat(GraphicContext *pGraphicContext, VkFormat *candidates, uint32_t candidatesCount, VkFormatFeatureFlags features, VkImageTiling tiling, VkFormat *pVkFormat)
{
    VkPhysicalDevice vkPhysicalDevice = pGraphicContext->vkPhysicalDevice;
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