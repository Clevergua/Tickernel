#include "graphic.h"

#define GET_ARRAY_COUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))
void tryThrowVulkanError(VkResult vkResult)
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
    uint32_t requiredExtensionCount = GET_ARRAY_COUNT(requiredExtensionNames);

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
    uint32_t extensionCount = GET_ARRAY_COUNT(extensionNames);
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
        queueFamilyIndexCount = GET_ARRAY_COUNT(queueFamilyIndices);
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

static void createGraphicImages(GraphicContext *pGraphicContext)
{
    VkExtent3D vkExtent3D = {
        .width = pGraphicContext->swapchainWidth,
        .height = pGraphicContext->swapchainHeight,
        .depth = 1,
    };
    VkFormat depthVkFormat;
    findDepthFormat(pGraphicContext->vkPhysicalDevice, &depthVkFormat);
    createGraphicImage(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, vkExtent3D, depthVkFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, &pGraphicContext->depthGraphicImage);
    createGraphicImage(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, vkExtent3D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicContext->albedoGraphicImage);
    createGraphicImage(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, vkExtent3D, VK_FORMAT_A2R10G10B10_UNORM_PACK32, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicContext->normalGraphicImage);
    createGraphicImage(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, vkExtent3D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicContext->colorGraphicImage);
}

static void destroyGraphicImages(GraphicContext *pGraphicContext)
{
    destroyGraphicImage(pGraphicContext->vkDevice, pGraphicContext->normalGraphicImage);
    destroyGraphicImage(pGraphicContext->vkDevice, pGraphicContext->albedoGraphicImage);
    destroyGraphicImage(pGraphicContext->vkDevice, pGraphicContext->depthGraphicImage);
    destroyGraphicImage(pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage);
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

    destroyGraphicImages(pGraphicContext);
    createGraphicImages(pGraphicContext);

    updateDeferredRenderPass(&pGraphicContext->deferredRenderPass, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, pGraphicContext->depthGraphicImage, pGraphicContext->albedoGraphicImage, pGraphicContext->normalGraphicImage, pGraphicContext->swapchainImageCount, pGraphicContext->swapchainImageViews, swapchainWidth, swapchainHeight, pGraphicContext->globalUniformBuffer, pGraphicContext->lightsUniformBuffer);
    // updatePostProcessRenderPass(&pGraphicContext->postProcessRenderPass, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, swapchainWidth, swapchainHeight, pGraphicContext->swapchainImageViews);
    // updateUIRenderPass(&pGraphicContext->uiRenderPass, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, swapchainWidth, swapchainHeight, pGraphicContext->swapchainImageViews);
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

static void createUniformBuffers(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    size_t bufferSize = sizeof(GlobalUniformBuffer);
    createBuffer(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pGraphicContext->globalUniformBuffer, &pGraphicContext->globalUniformBufferMemory);
    result = vkMapMemory(pGraphicContext->vkDevice, pGraphicContext->globalUniformBufferMemory, 0, bufferSize, 0, &pGraphicContext->globalUniformBufferMapped);
    tryThrowVulkanError(result);

    bufferSize = sizeof(LightsUniformBuffer);
    createBuffer(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pGraphicContext->lightsUniformBuffer, &pGraphicContext->lightsUniformBufferMemory);
    result = vkMapMemory(pGraphicContext->vkDevice, pGraphicContext->lightsUniformBufferMemory, 0, bufferSize, 0, &pGraphicContext->lightsUniformBufferMapped);
    tryThrowVulkanError(result);
}

static void destroyUniformBuffers(GraphicContext *pGraphicContext)
{
    vkUnmapMemory(pGraphicContext->vkDevice, pGraphicContext->lightsUniformBufferMemory);
    destroyBuffer(pGraphicContext->vkDevice, pGraphicContext->lightsUniformBuffer, pGraphicContext->lightsUniformBufferMemory);

    vkUnmapMemory(pGraphicContext->vkDevice, pGraphicContext->globalUniformBufferMemory);
    destroyBuffer(pGraphicContext->vkDevice, pGraphicContext->globalUniformBuffer, pGraphicContext->globalUniformBufferMemory);
}

static void updateGlobalUniformBuffer(GraphicContext *pGraphicContext)
{
    if (pGraphicContext->canUpdateGlobalUniformBuffer)
    {
        memcpy(pGraphicContext->globalUniformBufferMapped, &pGraphicContext->inputGlobalUniformBuffer, sizeof(pGraphicContext->inputGlobalUniformBuffer));
        pGraphicContext->canUpdateGlobalUniformBuffer = false;
    }
    else
    {
        // do nothing.
    }
}

static void updateLightsUniformBuffer(GraphicContext *pGraphicContext)
{
    if (pGraphicContext->canUpdateLightsUniformBuffer)
    {
        memcpy(pGraphicContext->lightsUniformBufferMapped, &pGraphicContext->inputLightsUniformBuffer, sizeof(pGraphicContext->inputLightsUniformBuffer));
        pGraphicContext->canUpdateLightsUniformBuffer = false;
    }
    else
    {
        // do nothing.
    }
}

static void destroyVkCommandBuffers(GraphicContext *pGraphicContext)
{
    vkFreeCommandBuffers(pGraphicContext->vkDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->swapchainImageCount, pGraphicContext->graphicVkCommandBuffers);
    tickernelFree(pGraphicContext->graphicVkCommandBuffers);
}

static void recordCommandBuffer(GraphicContext *pGraphicContext)
{
    VkCommandBuffer vkCommandBuffer = pGraphicContext->graphicVkCommandBuffers[pGraphicContext->swapchainIndex];

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
    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = NULL,
        };
    VkResult result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    tryThrowVulkanError(result);

    recordDeferredRenderPass(&pGraphicContext->deferredRenderPass, vkCommandBuffer, viewport, scissor, pGraphicContext->vkDevice, pGraphicContext->swapchainIndex);
    // recordPostProcessRenderPass(&pGraphicContext->postProcessRenderPass, vkCommandBuffer, viewport, scissor, pGraphicContext->vkDevice, pGraphicContext->swapchainIndex);
    // recordUIRenderPass(&pGraphicContext->uiRenderPass, vkCommandBuffer, viewport, scissor, pGraphicContext->vkDevice, pGraphicContext->swapchainIndex);

    result = vkEndCommandBuffer(vkCommandBuffer);
    tryThrowVulkanError(result);
}

GraphicContext *startGraphic(const char *assetsPath, int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    GraphicContext *pGraphicContext = tickernelMalloc(sizeof(GraphicContext));
    pGraphicContext->assetsPath = assetsPath;
    pGraphicContext->targetSwapchainImageCount = targetSwapchainImageCount;
    pGraphicContext->targetPresentMode = targetPresentMode;
    pGraphicContext->vkInstance = vkInstance;
    pGraphicContext->vkSurface = vkSurface;
    pGraphicContext->canUpdateGlobalUniformBuffer = false;
    pGraphicContext->canUpdateLightsUniformBuffer = false;

    pickPhysicalDevice(pGraphicContext);
    createLogicalDevice(pGraphicContext);
    createSwapchain(pGraphicContext, swapchainWidth, swapchainHeight);
    createSignals(pGraphicContext);
    createCommandPools(pGraphicContext);
    createVkCommandBuffers(pGraphicContext);
    createUniformBuffers(pGraphicContext);
    createGraphicImages(pGraphicContext);

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
    createDeferredRenderPass(&pGraphicContext->deferredRenderPass, pGraphicContext->assetsPath, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, pGraphicContext->depthGraphicImage, pGraphicContext->albedoGraphicImage, pGraphicContext->normalGraphicImage, pGraphicContext->swapchainImageCount, pGraphicContext->swapchainImageViews, pGraphicContext->surfaceFormat.format, viewport, scissor, pGraphicContext->globalUniformBuffer, pGraphicContext->lightsUniformBuffer);
    // createPostProcessRenderPass(&pGraphicContext->postProcessRenderPass, pGraphicContext->assetsPath, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, viewport, scissor, pGraphicContext->swapchainImageCount, pGraphicContext->swapchainImageViews, pGraphicContext->surfaceFormat.format);
    createUIRenderPass(&pGraphicContext->uiRenderPass, pGraphicContext->assetsPath, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, viewport, scissor, pGraphicContext->swapchainImageCount, pGraphicContext->swapchainImageViews, pGraphicContext->surfaceFormat.format);
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
        // printf("pGraphicContext->swapchainIndex: %d\n", pGraphicContext->swapchainIndex);
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

                updateGlobalUniformBuffer(pGraphicContext);
                updateLightsUniformBuffer(pGraphicContext);
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

            updateGlobalUniformBuffer(pGraphicContext);
            updateLightsUniformBuffer(pGraphicContext);
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
    // destroyUIRenderPass(&pGraphicContext->uiRenderPass, pGraphicContext->vkDevice);
    // destroyPostProcessRenderPass(&pGraphicContext->postProcessRenderPass, pGraphicContext->vkDevice);
    destroyDeferredRenderPass(&pGraphicContext->deferredRenderPass, pGraphicContext->vkDevice);
    destroyGraphicImages(pGraphicContext);
    destroyUniformBuffers(pGraphicContext);
    destroyVkCommandBuffers(pGraphicContext);
    destroyCommandPools(pGraphicContext);
    destroySignals(pGraphicContext);
    destroySwapchain(pGraphicContext);
    destroyLogicalDevice(pGraphicContext);

    tickernelFree(pGraphicContext);
}
