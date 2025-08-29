#include "gfxCore.h"

static void getGfxAndPresentQueueFamilyIndices(GfxContext *pGfxContext, VkPhysicalDevice vkPhysicalDevice, uint32_t *pGfxQueueFamilyIndex, uint32_t *pPresentQueueFamilyIndex)
{
    VkSurfaceKHR vkSurface = pGfxContext->vkSurface;
    uint32_t queueFamilyPropertiesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyPropertiesCount, NULL);
    VkQueueFamilyProperties *vkQueueFamilyPropertiesArray = tknMalloc(queueFamilyPropertiesCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyPropertiesCount, vkQueueFamilyPropertiesArray);
    *pGfxQueueFamilyIndex = UINT32_MAX;
    *pPresentQueueFamilyIndex = UINT32_MAX;
    for (int queueFamilyPropertiesIndex = 0; queueFamilyPropertiesIndex < queueFamilyPropertiesCount; queueFamilyPropertiesIndex++)
    {
        VkQueueFamilyProperties vkQueueFamilyProperties = vkQueueFamilyPropertiesArray[queueFamilyPropertiesIndex];
        if (vkQueueFamilyProperties.queueCount > 0 && vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            *pGfxQueueFamilyIndex = queueFamilyPropertiesIndex;
        }
        else
        {
            // continue;
        }
        VkBool32 pSupported = VK_FALSE;
        assertVkResult(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, queueFamilyPropertiesIndex, vkSurface, &pSupported));
        if (vkQueueFamilyProperties.queueCount > 0 && pSupported)
        {
            *pPresentQueueFamilyIndex = queueFamilyPropertiesIndex;
        }
        else
        {
            // continue;
        }

        if (*pGfxQueueFamilyIndex != UINT32_MAX && *pPresentQueueFamilyIndex != UINT32_MAX)
        {
            break;
        }
    }
    tknFree(vkQueueFamilyPropertiesArray);
}
static void pickPhysicalDevice(GfxContext *pGfxContext, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode)
{
    uint32_t deviceCount = -1;
    assertVkResult(vkEnumeratePhysicalDevices(pGfxContext->vkInstance, &deviceCount, NULL));
    if (deviceCount <= 0)
    {
        printf("failed to find GPUs with Vulkan support!");
    }
    else
    {
        VkPhysicalDevice *devices = tknMalloc(deviceCount * sizeof(VkPhysicalDevice));
        assertVkResult(vkEnumeratePhysicalDevices(pGfxContext->vkInstance, &deviceCount, devices));
        uint32_t maxScore = 0;
        char *targetDeviceName = NULL;
        pGfxContext->vkPhysicalDevice = VK_NULL_HANDLE;
        VkSurfaceKHR vkSurface = pGfxContext->vkSurface;
        for (uint32_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
        {
            uint32_t score = 0;
            VkPhysicalDevice vkPhysicalDevice = devices[deviceIndex];
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);
            char *requiredExtensionNames[] = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            };
            uint32_t requiredExtensionCount = TKN_ARRAY_COUNT(requiredExtensionNames);
            uint32_t extensionCount = 0;
            assertVkResult(vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, NULL));
            VkExtensionProperties *extensionProperties = tknMalloc(extensionCount * sizeof(VkExtensionProperties));
            assertVkResult(vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, extensionProperties));
            uint32_t requiredExtensionIndex;
            for (requiredExtensionIndex = 0; requiredExtensionIndex < requiredExtensionCount; requiredExtensionIndex++)
            {
                char *requiredExtensionName = requiredExtensionNames[requiredExtensionIndex];
                uint32_t extensionIndex;
                for (extensionIndex = 0; extensionIndex < extensionCount; extensionIndex++)
                {
                    char *supportedExtensionName = extensionProperties[extensionIndex].extensionName;
                    if (0 == strcmp(supportedExtensionName, requiredExtensionName))
                    {
                        break;
                    }
                    else
                    {
                        // continue;
                    }
                }
                if (extensionIndex < extensionCount)
                {
                    // found one
                    continue;
                }
                else
                {
                    // not found
                    break;
                }
            }
            tknFree(extensionProperties);
            if (requiredExtensionIndex < requiredExtensionCount)
            {
                // not found all required extensions
                continue;
            }
            else
            {
                // found all
            }

            uint32_t gfxQueueFamilyIndex;
            uint32_t presentQueueFamilyIndex;
            getGfxAndPresentQueueFamilyIndices(pGfxContext, vkPhysicalDevice, &gfxQueueFamilyIndex, &presentQueueFamilyIndex);
            if (UINT32_MAX == gfxQueueFamilyIndex || UINT32_MAX == presentQueueFamilyIndex)
            {
                // No gfx or present queue family index
                continue;
            }

            uint32_t surfaceFormatCount;
            assertVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, NULL));
            VkSurfaceFormatKHR *supportedSurfaceFormats = tknMalloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
            assertVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, supportedSurfaceFormats));
            uint32_t supportedSurfaceFormatIndex;
            for (supportedSurfaceFormatIndex = 0; supportedSurfaceFormatIndex < surfaceFormatCount; supportedSurfaceFormatIndex++)
            {
                VkSurfaceFormatKHR vkSurfaceFormat = supportedSurfaceFormats[supportedSurfaceFormatIndex];
                if (vkSurfaceFormat.colorSpace == targetVkSurfaceFormat.colorSpace &&
                    vkSurfaceFormat.format == targetVkSurfaceFormat.format)
                {
                    break;
                }
                else
                {
                    // continue
                }
            }
            tknFree(supportedSurfaceFormats);
            if (supportedSurfaceFormatIndex < surfaceFormatCount)
            {
                // found one
            }
            else
            {
                // not found
                continue;
            }

            uint32_t presentModeCount;
            assertVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, NULL));
            VkPresentModeKHR *supportedPresentModes = tknMalloc(presentModeCount * sizeof(VkPresentModeKHR));
            assertVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, supportedPresentModes));
            uint32_t supportedPresentModeIndex;
            for (supportedPresentModeIndex = 0; supportedPresentModeIndex < presentModeCount; supportedPresentModeIndex++)
            {
                VkPresentModeKHR supportedPresentMode = supportedPresentModes[supportedPresentModeIndex];
                if (supportedPresentMode == targetVkPresentMode)
                {
                    // found one
                    break;
                }
                else
                {
                    // continue
                }
            }
            tknFree(supportedPresentModes);
            if (supportedPresentModeIndex < presentModeCount)
            {
                // found one
            }
            else
            {
                // not found
                continue;
            }

            VkFormatProperties vkFormatProperties;
            vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, VK_FORMAT_ASTC_4x4_UNORM_BLOCK, &vkFormatProperties);
            if (!(vkFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
            {
                // ASTC format not supported
                continue;
            }

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                score += 1000;
            }
            else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                score += 500;
            }
            else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
            {
                score += 300;
            }
            else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
            {
                score += 100;
            }

            if (score >= maxScore)
            {
                maxScore = score;
                targetDeviceName = deviceProperties.deviceName;
                pGfxContext->vkPhysicalDevice = vkPhysicalDevice;
                pGfxContext->gfxQueueFamilyIndex = gfxQueueFamilyIndex;
                pGfxContext->presentQueueFamilyIndex = presentQueueFamilyIndex;
                pGfxContext->vkPhysicalDeviceProperties = deviceProperties;
                pGfxContext->surfaceFormat = targetVkSurfaceFormat;
                pGfxContext->presentMode = targetVkPresentMode;
            }
            else
            {
                // continue
            }
        }
        tknFree(devices);

        if (NULL != pGfxContext->vkPhysicalDevice)
        {
            printf("Selected target physical device named %s\n", targetDeviceName);
        }
        else
        {
            tknError("failed to find GPUs with Vulkan support!");
        }
    }
}
static void populateLogicalDevice(GfxContext *pGfxContext)
{
    VkPhysicalDevice vkPhysicalDevice = pGfxContext->vkPhysicalDevice;
    uint32_t gfxQueueFamilyIndex = pGfxContext->gfxQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGfxContext->presentQueueFamilyIndex;
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo *queueCreateInfos;
    uint32_t queueCount;
    if (gfxQueueFamilyIndex == presentQueueFamilyIndex)
    {
        queueCount = 1;
        queueCreateInfos = tknMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
        VkDeviceQueueCreateInfo gfxCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = gfxQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
        queueCreateInfos[0] = gfxCreateInfo;
    }
    else
    {
        queueCount = 2;
        queueCreateInfos = tknMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
        VkDeviceQueueCreateInfo gfxCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = gfxQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
        VkDeviceQueueCreateInfo presentCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .queueFamilyIndex = presentQueueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        };
        queueCreateInfos[0] = gfxCreateInfo;
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
    };
    uint32_t extensionCount = TKN_ARRAY_COUNT(extensionNames);
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
    assertVkResult(vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, NULL, &pGfxContext->vkDevice));
    vkGetDeviceQueue(pGfxContext->vkDevice, gfxQueueFamilyIndex, 0, &pGfxContext->vkGfxQueue);
    vkGetDeviceQueue(pGfxContext->vkDevice, presentQueueFamilyIndex, 0, &pGfxContext->vkPresentQueue);
    tknFree(queueCreateInfos);
}
static void cleanupLogicalDevice(GfxContext *pGfxContext)
{
    vkDestroyDevice(pGfxContext->vkDevice, NULL);
}
static void createSwapchainAttachmentPtr(GfxContext *pGfxContext, VkExtent2D targetSwapchainExtent, uint32_t targetSwapchainImageCount)
{
    Attachment *pSwapchainAttachment = tknMalloc(sizeof(Attachment));

    VkPhysicalDevice vkPhysicalDevice = pGfxContext->vkPhysicalDevice;
    VkSurfaceKHR vkSurface = pGfxContext->vkSurface;
    VkDevice vkDevice = pGfxContext->vkDevice;

    assertVkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurface, &pGfxContext->vkSurfaceCapabilities));

    uint32_t swapchainImageCount = TKN_CLAMP(targetSwapchainImageCount, pGfxContext->vkSurfaceCapabilities.minImageCount, pGfxContext->vkSurfaceCapabilities.maxImageCount);

    VkExtent2D swapchainExtent;
    swapchainExtent.width = TKN_CLAMP(targetSwapchainExtent.width, pGfxContext->vkSurfaceCapabilities.minImageExtent.width, pGfxContext->vkSurfaceCapabilities.maxImageExtent.width);
    swapchainExtent.height = TKN_CLAMP(targetSwapchainExtent.height, pGfxContext->vkSurfaceCapabilities.minImageExtent.height, pGfxContext->vkSurfaceCapabilities.maxImageExtent.height);

    VkSharingMode imageSharingMode = pGfxContext->gfxQueueFamilyIndex != pGfxContext->presentQueueFamilyIndex ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    uint32_t queueFamilyIndexCount = pGfxContext->gfxQueueFamilyIndex != pGfxContext->presentQueueFamilyIndex ? 2 : 0;
    uint32_t pQueueFamilyIndices[] = {pGfxContext->gfxQueueFamilyIndex, pGfxContext->presentQueueFamilyIndex};

    VkSwapchainCreateInfoKHR swapchainCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0,
            .surface = vkSurface,
            .minImageCount = swapchainImageCount,
            .imageFormat = pGfxContext->surfaceFormat.format,
            .imageColorSpace = pGfxContext->surfaceFormat.colorSpace,
            .imageExtent = swapchainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = imageSharingMode,
            .queueFamilyIndexCount = queueFamilyIndexCount,
            .pQueueFamilyIndices = pQueueFamilyIndices,
            .preTransform = pGfxContext->vkSurfaceCapabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = pGfxContext->presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };
    VkSwapchainKHR vkSwapchain;
    assertVkResult(vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &vkSwapchain));

    VkImage *swapchainImages = tknMalloc(swapchainImageCount * sizeof(VkImage));
    assertVkResult(vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &swapchainImageCount, swapchainImages));
    VkImageView *swapchainImageViews = tknMalloc(swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        VkComponentMapping components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        };
        VkImageSubresourceRange subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .baseMipLevel = 0,
            .layerCount = 1,
            .baseArrayLayer = 0,
        };
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .image = swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = pGfxContext->surfaceFormat.format,
            .components = components,
            .subresourceRange = subresourceRange,
        };
        assertVkResult(vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, &swapchainImageViews[i]));
    }
    *pSwapchainAttachment = (Attachment){
        .attachmentType = ATTACHMENT_TYPE_SWAPCHAIN,
        .attachmentUnion.swapchainAttachment = {
            .swapchainExtent = swapchainExtent,
            .vkSwapchain = vkSwapchain,
            .swapchainImageCount = swapchainImageCount,
            .swapchainImages = swapchainImages,
            .swapchainImageViews = swapchainImageViews,
        },
        .vkFormat = pGfxContext->surfaceFormat.format,
        .renderPassPtrHashSet = tknCreateHashSet(sizeof(RenderPass*)),
    };
    pGfxContext->pSwapchainAttachment = pSwapchainAttachment;
};
static void destroySwapchainAttachmentPtr(GfxContext *pGfxContext)
{
    Attachment *pSwapchainAttachment = pGfxContext->pSwapchainAttachment;
    VkDevice vkDevice = pGfxContext->vkDevice;
    SwapchainAttachment swapchainAttachment = pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    for (uint32_t i = 0; i < swapchainAttachment.swapchainImageCount; i++)
    {
        vkDestroyImageView(vkDevice, swapchainAttachment.swapchainImageViews[i], NULL);
    }
    tknFree(swapchainAttachment.swapchainImageViews);
    tknFree(swapchainAttachment.swapchainImages);
    vkDestroySwapchainKHR(vkDevice, swapchainAttachment.vkSwapchain, NULL);

    tknDestroyHashSet(pSwapchainAttachment->renderPassPtrHashSet);
    tknFree(pSwapchainAttachment);
}
static void updateSwapchainAttachmentPtr(GfxContext *pGfxContext, VkExtent2D swapchainExtent)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    assertVkResult(vkDeviceWaitIdle(vkDevice));
    Attachment *pSwapchainAttachment = pGfxContext->pSwapchainAttachment;
    SwapchainAttachment *pswapchainAttachment = &pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    for (uint32_t i = 0; i < pswapchainAttachment->swapchainImageCount; i++)
    {
        vkDestroyImageView(vkDevice, pswapchainAttachment->swapchainImageViews[i], NULL);
    }
    vkDestroySwapchainKHR(vkDevice, pswapchainAttachment->vkSwapchain, NULL);

    swapchainExtent.width = TKN_CLAMP(swapchainExtent.width, pGfxContext->vkSurfaceCapabilities.minImageExtent.width, pGfxContext->vkSurfaceCapabilities.maxImageExtent.width);
    swapchainExtent.height = TKN_CLAMP(swapchainExtent.height, pGfxContext->vkSurfaceCapabilities.minImageExtent.height, pGfxContext->vkSurfaceCapabilities.maxImageExtent.height);
    pswapchainAttachment->swapchainExtent = swapchainExtent;

    VkSharingMode imageSharingMode = pGfxContext->gfxQueueFamilyIndex != pGfxContext->presentQueueFamilyIndex ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    uint32_t queueFamilyIndexCount = pGfxContext->gfxQueueFamilyIndex != pGfxContext->presentQueueFamilyIndex ? 2 : 0;
    uint32_t pQueueFamilyIndices[] = {pGfxContext->gfxQueueFamilyIndex, pGfxContext->presentQueueFamilyIndex};
    VkSwapchainCreateInfoKHR swapchainCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0,
            .surface = pGfxContext->vkSurface,
            .minImageCount = pswapchainAttachment->swapchainImageCount,
            .imageFormat = pGfxContext->surfaceFormat.format,
            .imageColorSpace = pGfxContext->surfaceFormat.colorSpace,
            .imageExtent = swapchainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = imageSharingMode,
            .queueFamilyIndexCount = queueFamilyIndexCount,
            .pQueueFamilyIndices = pQueueFamilyIndices,
            .preTransform = pGfxContext->vkSurfaceCapabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = pGfxContext->presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };
    assertVkResult(vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &pswapchainAttachment->vkSwapchain));
    assertVkResult(vkGetSwapchainImagesKHR(vkDevice, pswapchainAttachment->vkSwapchain, &pswapchainAttachment->swapchainImageCount, pswapchainAttachment->swapchainImages));
    for (uint32_t i = 0; i < pswapchainAttachment->swapchainImageCount; i++)
    {
        VkComponentMapping components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        };
        VkImageSubresourceRange subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .baseMipLevel = 0,
            .layerCount = 1,
            .baseArrayLayer = 0,
        };
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .image = pswapchainAttachment->swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = pGfxContext->surfaceFormat.format,
            .components = components,
            .subresourceRange = subresourceRange,
        };
        assertVkResult(vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, &pswapchainAttachment->swapchainImageViews[i]));
    }
}
static void populateSignals(GfxContext *pGfxContext)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };
    VkDevice vkDevice = pGfxContext->vkDevice;
    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    assertVkResult(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGfxContext->imageAvailableSemaphore));
    assertVkResult(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGfxContext->renderFinishedSemaphore));
    assertVkResult(vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGfxContext->renderFinishedFence));
}
static void cleanupSignals(GfxContext *pGfxContext)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    vkDestroySemaphore(vkDevice, pGfxContext->imageAvailableSemaphore, NULL);
    vkDestroySemaphore(vkDevice, pGfxContext->renderFinishedSemaphore, NULL);
    vkDestroyFence(vkDevice, pGfxContext->renderFinishedFence, NULL);
}
static void populateCommandPools(GfxContext *pGfxContext)
{
    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = pGfxContext->gfxQueueFamilyIndex,
    };
    assertVkResult(vkCreateCommandPool(pGfxContext->vkDevice, &vkCommandPoolCreateInfo, NULL, &pGfxContext->gfxVkCommandPool));
}
static void cleanupCommandPools(GfxContext *pGfxContext)
{
    vkDestroyCommandPool(pGfxContext->vkDevice, pGfxContext->gfxVkCommandPool, NULL);
}
static void populateVkCommandBuffers(GfxContext *pGfxContext)
{
    SwapchainAttachment *pswapchainAttachment = &pGfxContext->pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    pGfxContext->gfxVkCommandBuffers = tknMalloc(sizeof(VkCommandBuffer) * pswapchainAttachment->swapchainImageCount);
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pGfxContext->gfxVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = pswapchainAttachment->swapchainImageCount,
    };
    assertVkResult(vkAllocateCommandBuffers(pGfxContext->vkDevice, &vkCommandBufferAllocateInfo, pGfxContext->gfxVkCommandBuffers));
}
static void cleanupVkCommandBuffers(GfxContext *pGfxContext)
{
    SwapchainAttachment *pswapchainAttachment = &pGfxContext->pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    vkFreeCommandBuffers(pGfxContext->vkDevice, pGfxContext->gfxVkCommandPool, pswapchainAttachment->swapchainImageCount, pGfxContext->gfxVkCommandBuffers);
    tknFree(pGfxContext->gfxVkCommandBuffers);
}
static void recordCommandBuffer(GfxContext *pGfxContext, uint32_t swapchainIndex)
{
    VkCommandBuffer vkCommandBuffer = pGfxContext->gfxVkCommandBuffers[swapchainIndex];
    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = NULL,
        };
    assertVkResult(vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo));
    Material *pGlobalMaterial = *(Material **)tknGetFromDynamicArray(&pGfxContext->pGlobalDescriptorSet->materialPtrDynamicArray, 0);
    for (uint32_t renderPassIndex = 0; renderPassIndex < pGfxContext->renderPassPtrDynamicArray.count; renderPassIndex++)
    {
        RenderPass *pRenderPass = *(RenderPass **)tknGetFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, renderPassIndex);
        VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = pRenderPass->vkRenderPass,
            .framebuffer = pRenderPass->vkFramebuffers[swapchainIndex],
            .renderArea = pRenderPass->renderArea,
            .clearValueCount = pRenderPass->attachmentCount,
            .pClearValues = pRenderPass->vkClearValues,
        };
        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        VkViewport vkViewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = pRenderPass->renderArea.extent.width,
            .height = pRenderPass->renderArea.extent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        vkCmdSetViewport(vkCommandBuffer, 0, 1, &vkViewport);

        VkRect2D scissor = {
            .offset = {0, 0},
            .extent = pRenderPass->renderArea.extent,
        };
        vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
        for (uint32_t subpassIndex = 0; subpassIndex < pRenderPass->subpassCount; subpassIndex++)
        {
            Subpass *pSubpass = &pRenderPass->subpasses[subpassIndex];
            Material *pSubpassMaterial = *(Material **)tknGetFromDynamicArray(&pSubpass->pSubpassDescriptorSet->materialPtrDynamicArray, 0);
            for (uint32_t pipelineIndex = 0; pipelineIndex < pSubpass->pipelinePtrDynamicArray.count; pipelineIndex++)
            {
                Pipeline *pPipeline = *(Pipeline **)tknGetFromDynamicArray(&pSubpass->pipelinePtrDynamicArray, pipelineIndex);
                vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipeline);
                for (uint32_t materialPtrIndex = 0; materialPtrIndex < pPipeline->pPipelineDescriptorSet->materialPtrDynamicArray.count; materialPtrIndex++)
                {
                    Material *pMaterial = *(Material **)tknGetFromDynamicArray(&pPipeline->pPipelineDescriptorSet->materialPtrDynamicArray, materialPtrIndex);
                    VkDescriptorSet *vkDescriptorSets = tknMalloc(sizeof(VkDescriptorSet) * TKN_MAX_DESCRIPTOR_SET);
                    vkDescriptorSets[TKN_GLOBAL_DESCRIPTOR_SET] = pGlobalMaterial->vkDescriptorSet;
                    vkDescriptorSets[TKN_GLOBAL_DESCRIPTOR_SET] = pSubpassMaterial->vkDescriptorSet;
                    vkDescriptorSets[TKN_GLOBAL_DESCRIPTOR_SET] = pMaterial->vkDescriptorSet;
                    tknFree(vkDescriptorSets);
                    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipelineLayout, 0, TKN_MAX_DESCRIPTOR_SET, vkDescriptorSets, 0, NULL);
                    for (uint32_t instanceIndex = 0; instanceIndex < pMaterial->instancePtrDynamicArray.count; instanceIndex++)
                    {
                        Instance *pInstance = *(Instance **)tknGetFromDynamicArray(&pMaterial->instancePtrDynamicArray, instanceIndex);
                        if (pInstance->instanceCount > 0)
                        {
                            Mesh *pMesh = pInstance->pMesh;
                            tknAssert(pMesh->vertexCount > 0, "Mesh has no vertices");
                            VkBuffer vertexBuffers[] = {pMesh->vertexVkBuffer, pInstance->instanceMappedBuffer};
                            if (pMesh->indexCount > 0)
                            {
                                VkDeviceSize offsets[] = {0, 0};
                                vkCmdBindVertexBuffers(vkCommandBuffer, 0, 2, vertexBuffers, offsets);
                                vkCmdBindIndexBuffer(vkCommandBuffer, pMesh->indexVkBuffer, 0, pMesh->vkIndexType);
                                vkCmdDrawIndexed(vkCommandBuffer, pMesh->indexCount, pInstance->instanceCount, 0, 0, 0);
                            }
                            else
                            {
                                VkDeviceSize offsets[] = {0, 0};
                                vkCmdBindVertexBuffers(vkCommandBuffer, 0, 2, vertexBuffers, offsets);
                                vkCmdBindIndexBuffer(vkCommandBuffer, pMesh->indexVkBuffer, 0, pMesh->vkIndexType);
                                vkCmdDraw(vkCommandBuffer, pMesh->vertexCount, pInstance->instanceCount, 0, 0);
                            }
                        }
                        else
                        {
                            // skip
                        }
                    }
                }
            }
        }
        vkCmdEndRenderPass(vkCommandBuffer);
    }

    assertVkResult(vkEndCommandBuffer(vkCommandBuffer));
}
static void submitCommandBuffer(GfxContext *pGfxContext, uint32_t swapchainIndex)
{
    // Submit workflow...
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGfxContext->imageAvailableSemaphore},
        .pWaitDstStageMask = (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        .commandBufferCount = 1,
        .pCommandBuffers = &pGfxContext->gfxVkCommandBuffers[swapchainIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = (VkSemaphore[]){pGfxContext->renderFinishedSemaphore},
    };

    assertVkResult(vkQueueSubmit(pGfxContext->vkGfxQueue, 1, &submitInfo, pGfxContext->renderFinishedFence));
}
static void present(GfxContext *pGfxContext, uint32_t swapchainIndex)
{
    SwapchainAttachment *pswapchainAttachment = &pGfxContext->pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGfxContext->renderFinishedSemaphore},
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]){pswapchainAttachment->vkSwapchain},
        .pImageIndices = &swapchainIndex,
        .pResults = NULL,
    };
    VkResult result = vkQueuePresentKHR(pGfxContext->vkPresentQueue, &presentInfo);
    if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
    {
        printf("Recreate swapchain because of the result: %d when presenting.\n", result);
        updateSwapchainAttachmentPtr(pGfxContext, pswapchainAttachment->swapchainExtent);
        for (uint32_t renderPassIndex = 0; renderPassIndex < pGfxContext->renderPassPtrDynamicArray.count; renderPassIndex++)
        {
            RenderPass *pRenderPass = *(RenderPass **)tknGetFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, renderPassIndex);
            Attachment *pSwapchainAttachment = getSwapchainAttachmentPtr(pGfxContext);
            if (tknContainsInHashSet(&pSwapchainAttachment->renderPassPtrHashSet, pRenderPass))
            {
                repopulateFramebuffers(pGfxContext, pRenderPass);
            }
            else
            {
                // Don't need to recreate framebuffers
            }
        }
    }
    else
    {
        assertVkResult(result);
    }
}
static void setupRenderPipelineAndResources(GfxContext *pGfxContext, uint32_t spvPathCount, const char **spvPaths)
{
    pGfxContext->dynamicAttachmentPtrHashSet = tknCreateHashSet(sizeof(Attachment*));
    pGfxContext->renderPassPtrDynamicArray = tknCreateDynamicArray(sizeof(RenderPass *), TKN_DEFAULT_COLLECTION_SIZE);
    SpvReflectShaderModule *spvReflectShaderModules = tknMalloc(sizeof(SpvReflectShaderModule) * spvPathCount);
    for (uint32_t spvPathIndex = 0; spvPathIndex < spvPathCount; spvPathIndex++)
    {
        spvReflectShaderModules[spvPathIndex] = createSpvReflectShaderModule(spvPaths[spvPathIndex]);
    }
    pGfxContext->pGlobalDescriptorSet = createDescriptorSetPtr(pGfxContext, spvPathCount, spvReflectShaderModules, TKN_GLOBAL_DESCRIPTOR_SET);
    for (uint32_t spvPathIndex = 0; spvPathIndex < spvPathCount; spvPathIndex++)
    {
        destroySpvReflectShaderModule(&spvReflectShaderModules[spvPathIndex]);
    }
    tknFree(spvReflectShaderModules);
    createMaterialPtr(pGfxContext, pGfxContext->pGlobalDescriptorSet);

    pGfxContext->renderPassPtrHashSet = tknCreateHashSet(sizeof(RenderPass*));
}
static void teardownRenderPipelineAndResources(GfxContext *pGfxContext)
{
    destroyDescriptorSetPtr(pGfxContext, pGfxContext->pGlobalDescriptorSet);
    tknAssert(0 == pGfxContext->renderPassPtrDynamicArray.count, "Render pass dynamic array should be empty before destroying GfxContext.");
    tknDestroyDynamicArray(pGfxContext->renderPassPtrDynamicArray);
    tknAssert(0 == pGfxContext->dynamicAttachmentPtrHashSet.count, "Dynamic attachment hash set should be empty before destroying GfxContext.");
    tknDestroyHashSet(pGfxContext->dynamicAttachmentPtrHashSet);
    
    tknDestroyHashSet(pGfxContext->renderPassPtrHashSet);
}

GfxContext *createGfxContextPtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, uint32_t spvPathCount, const char **spvPaths)
{
    GfxContext *pGfxContext = tknMalloc(sizeof(GfxContext));
    *pGfxContext = (GfxContext){
        .frameCount = 0,
        .vkInstance = vkInstance,
        .vkSurface = vkSurface,

        .vkPhysicalDevice = VK_NULL_HANDLE,
        .vkPhysicalDeviceProperties = {},
        .gfxQueueFamilyIndex = UINT32_MAX,
        .presentQueueFamilyIndex = UINT32_MAX,

        .surfaceFormat = {},
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,

        .vkDevice = VK_NULL_HANDLE,
        .vkGfxQueue = VK_NULL_HANDLE,
        .vkPresentQueue = VK_NULL_HANDLE,

        .pSwapchainAttachment = NULL,

        .imageAvailableSemaphore = VK_NULL_HANDLE,
        .renderFinishedSemaphore = VK_NULL_HANDLE,
        .renderFinishedFence = VK_NULL_HANDLE,

        .gfxVkCommandPool = VK_NULL_HANDLE,
        .gfxVkCommandBuffers = NULL,

        .dynamicAttachmentPtrHashSet = {},
        .renderPassPtrDynamicArray = {},
        .pGlobalDescriptorSet = NULL,
    };
    pickPhysicalDevice(pGfxContext, targetVkSurfaceFormat, targetVkPresentMode);
    populateLogicalDevice(pGfxContext);
    createSwapchainAttachmentPtr(pGfxContext, swapchainExtent, targetSwapchainImageCount);
    populateSignals(pGfxContext);
    populateCommandPools(pGfxContext);
    populateVkCommandBuffers(pGfxContext);
    setupRenderPipelineAndResources(pGfxContext, spvPathCount, spvPaths);
    return pGfxContext;
}
void destroyGfxContextPtr(GfxContext *pGfxContext)
{
    teardownRenderPipelineAndResources(pGfxContext);
    cleanupVkCommandBuffers(pGfxContext);
    cleanupCommandPools(pGfxContext);
    cleanupSignals(pGfxContext);
    destroySwapchainAttachmentPtr(pGfxContext);
    cleanupLogicalDevice(pGfxContext);
    tknFree(pGfxContext);
}
void updateGfxContextPtr(GfxContext *pGfxContext, VkExtent2D swapchainExtent)
{
    SwapchainAttachment *pswapchainAttachment = &pGfxContext->pSwapchainAttachment->attachmentUnion.swapchainAttachment;
    if (pGfxContext->renderPassPtrDynamicArray.count > 0)
    {
        pGfxContext->frameCount++;
        uint32_t swapchainIndex = pGfxContext->frameCount % pswapchainAttachment->swapchainImageCount;
        VkDevice vkDevice = pGfxContext->vkDevice;

        if (swapchainExtent.width != pswapchainAttachment->swapchainExtent.width || swapchainExtent.height != pswapchainAttachment->swapchainExtent.height)
        {
            printf("Recreate swapchain because of a size change: (%d, %d) to (%d, %d) \n",
                   pswapchainAttachment->swapchainExtent.width,
                   pswapchainAttachment->swapchainExtent.height,
                   swapchainExtent.width,
                   swapchainExtent.height);
            updateSwapchainAttachmentPtr(pGfxContext, swapchainExtent);

            TknDynamicArray dirtyRenderPassPtrDynamicArray = tknCreateDynamicArray(sizeof(RenderPass *), TKN_DEFAULT_COLLECTION_SIZE);

            for (uint32_t i = 0; i < pGfxContext->dynamicAttachmentPtrHashSet.capacity; i++)
            {
                TknListNode *dynamicAttachmentPtrNode = pGfxContext->dynamicAttachmentPtrHashSet.nodePtrs[i];
                while (dynamicAttachmentPtrNode)
                {
                    Attachment *pDynamicAttachment = *(Attachment **)dynamicAttachmentPtrNode->data;
                    resizeDynamicAttachmentPtr(pGfxContext, pDynamicAttachment);
                    for (uint32_t i = 0; i < pDynamicAttachment->renderPassPtrHashSet.capacity; i++)
                    {
                        TknListNode *renderPassPtrNode = pDynamicAttachment->renderPassPtrHashSet.nodePtrs[i];
                        while (renderPassPtrNode)
                        {
                            RenderPass *pRenderPass = *(RenderPass **)renderPassPtrNode->data;
                            if (!tknContainsInDynamicArray(&dirtyRenderPassPtrDynamicArray, &pRenderPass))
                            {
                                tknAddToDynamicArray(&dirtyRenderPassPtrDynamicArray, &pRenderPass);
                            }
                            renderPassPtrNode = renderPassPtrNode->nextNodePtr;
                        }
                    }
                    dynamicAttachmentPtrNode = dynamicAttachmentPtrNode->nextNodePtr;
                }
            }

            for (uint32_t i = 0; i < pGfxContext->pSwapchainAttachment->renderPassPtrHashSet.capacity; i++)
            {
                TknListNode *renderPassPtrNode = pGfxContext->pSwapchainAttachment->renderPassPtrHashSet.nodePtrs[i];
                while (renderPassPtrNode)
                {
                    RenderPass *pRenderPass = *(RenderPass **)renderPassPtrNode->data;
                    if (!tknContainsInDynamicArray(&dirtyRenderPassPtrDynamicArray, &pRenderPass))
                    {
                        tknAddToDynamicArray(&dirtyRenderPassPtrDynamicArray, &pRenderPass);
                    }
                    renderPassPtrNode = renderPassPtrNode->nextNodePtr;
                }
            }
            for (uint32_t renderPassIndex = 0; renderPassIndex < dirtyRenderPassPtrDynamicArray.count; renderPassIndex++)
            {
                RenderPass *pRenderPass = *(RenderPass **)tknGetFromDynamicArray(&dirtyRenderPassPtrDynamicArray, renderPassIndex);
                repopulateFramebuffers(pGfxContext, pRenderPass);
            }
            tknDestroyDynamicArray(dirtyRenderPassPtrDynamicArray);
        }
        else
        {
            VkResult result = vkAcquireNextImageKHR(vkDevice, pswapchainAttachment->vkSwapchain, UINT64_MAX, pGfxContext->imageAvailableSemaphore, VK_NULL_HANDLE, &swapchainIndex);
            if (result != VK_SUCCESS)
            {
                if (VK_ERROR_OUT_OF_DATE_KHR == result)
                {
                    printf("Recreate swapchain because of result: %d\n", result);
                    updateSwapchainAttachmentPtr(pGfxContext, pswapchainAttachment->swapchainExtent);
                    for (uint32_t renderPassIndex = 0; renderPassIndex < pGfxContext->renderPassPtrDynamicArray.count; renderPassIndex++)
                    {
                        RenderPass *pRenderPass = *(RenderPass **)tknGetFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, renderPassIndex);
                        Attachment *pSwapchainAttachment = getSwapchainAttachmentPtr(pGfxContext);
                        if (tknContainsInHashSet(&pSwapchainAttachment->renderPassPtrHashSet, pRenderPass))
                        {
                            repopulateFramebuffers(pGfxContext, pRenderPass);
                        }
                        else
                        {
                            // Don't need to recreate framebuffers
                        }
                    }
                }
                else if (VK_SUBOPTIMAL_KHR == result)
                {
                    assertVkResult(vkResetFences(vkDevice, 1, &pGfxContext->renderFinishedFence));
                    recordCommandBuffer(pGfxContext, swapchainIndex);
                    submitCommandBuffer(pGfxContext, swapchainIndex);
                    present(pGfxContext, swapchainIndex);
                }
                else
                {
                    assertVkResult(result);
                }
            }
            else
            {
                assertVkResult(vkResetFences(vkDevice, 1, &pGfxContext->renderFinishedFence));
                recordCommandBuffer(pGfxContext, swapchainIndex);
                submitCommandBuffer(pGfxContext, swapchainIndex);
                present(pGfxContext, swapchainIndex);
            }
        }
    }
    else
    {
        printf("No render passes available, skipping updateGfxContextPtr.\n");
    }
}
void waitGfxContextPtr(GfxContext *pGfxContext)
{
    assertVkResult(vkWaitForFences(pGfxContext->vkDevice, 1, &pGfxContext->renderFinishedFence, VK_TRUE, UINT64_MAX));
}