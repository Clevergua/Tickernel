#include "gfx.h"
static void initializeGfxContext(GfxContext *pGfxContext, VkInstance vkInstance, VkSurfaceKHR vkSurface)
{
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

        .swapchainExtent = {},
        .vkSwapchain = VK_NULL_HANDLE,
        .swapchainImageCount = 0,
        .swapchainImages = NULL,
        .swapchainImageViews = NULL,

        .imageAvailableSemaphore = VK_NULL_HANDLE,
        .renderFinishedSemaphore = VK_NULL_HANDLE,
        .renderFinishedFence = VK_NULL_HANDLE,

        .gfxVkCommandPool = VK_NULL_HANDLE,
        .gfxVkCommandBuffers = NULL,

        .renderPassPtrDynamicArray = {},
    };
}

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
            if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                // nothing
            }
            else
            {
                tknError("Unknown device type");
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

        if (pGfxContext->vkPhysicalDevice != NULL)
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

static void populateSwapchain(GfxContext *pGfxContext, VkExtent2D targetSwapchainExtent, uint32_t targetSwapchainImageCount)
{
    VkPhysicalDevice vkPhysicalDevice = pGfxContext->vkPhysicalDevice;
    VkSurfaceKHR vkSurface = pGfxContext->vkSurface;
    VkDevice vkDevice = pGfxContext->vkDevice;
    uint32_t gfxQueueFamilyIndex = pGfxContext->gfxQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGfxContext->presentQueueFamilyIndex;

    VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
    assertVkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurface, &vkSurfaceCapabilities));
    pGfxContext->swapchainImageCount = TKN_CLAMP(targetSwapchainImageCount, vkSurfaceCapabilities.minImageCount, vkSurfaceCapabilities.maxImageCount);

    VkExtent2D swapchainExtent;
    swapchainExtent.width = TKN_CLAMP(targetSwapchainExtent.width, vkSurfaceCapabilities.minImageExtent.width, vkSurfaceCapabilities.maxImageExtent.width);
    swapchainExtent.height = TKN_CLAMP(targetSwapchainExtent.height, vkSurfaceCapabilities.minImageExtent.height, vkSurfaceCapabilities.maxImageExtent.height);

    VkSharingMode imageSharingMode = gfxQueueFamilyIndex != presentQueueFamilyIndex ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    uint32_t queueFamilyIndexCount = gfxQueueFamilyIndex != presentQueueFamilyIndex ? 2 : 0;
    uint32_t pQueueFamilyIndices[] = {gfxQueueFamilyIndex, presentQueueFamilyIndex};

    VkSwapchainCreateInfoKHR swapchainCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0,
            .surface = vkSurface,
            .minImageCount = pGfxContext->swapchainImageCount,
            .imageFormat = pGfxContext->surfaceFormat.format,
            .imageColorSpace = pGfxContext->surfaceFormat.colorSpace,
            .imageExtent = swapchainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = imageSharingMode,
            .queueFamilyIndexCount = queueFamilyIndexCount,
            .pQueueFamilyIndices = pQueueFamilyIndices,
            .preTransform = vkSurfaceCapabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = pGfxContext->presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };
    pGfxContext->swapchainAttachmentPtr = tknMalloc(sizeof(Attachment));
    pGfxContext->swapchainAttachmentPtr->attachmentType = ATTACHMENT_TYPE_SWAPCHAIN;
    assertVkResult(vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &pGfxContext->vkSwapchain));
    pGfxContext->swapchainImages = tknMalloc(pGfxContext->swapchainImageCount * sizeof(VkImage));
    assertVkResult(vkGetSwapchainImagesKHR(vkDevice, pGfxContext->vkSwapchain, &pGfxContext->swapchainImageCount, pGfxContext->swapchainImages));
    pGfxContext->swapchainImageViews = tknMalloc(pGfxContext->swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < pGfxContext->swapchainImageCount; i++)
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
            .image = pGfxContext->swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = pGfxContext->surfaceFormat.format,
            .components = components,
            .subresourceRange = subresourceRange,
        };
        assertVkResult(vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, &pGfxContext->swapchainImageViews[i]));
    }
};
static void cleanupSwapchain(GfxContext *pGfxContext)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    for (uint32_t i = 0; i < pGfxContext->swapchainImageCount; i++)
    {
        vkDestroyImageView(vkDevice, pGfxContext->swapchainImageViews[i], NULL);
    }
    tknFree(pGfxContext->swapchainImageViews);
    tknFree(pGfxContext->swapchainImages);
    vkDestroySwapchainKHR(vkDevice, pGfxContext->vkSwapchain, NULL);
    tknFree(pGfxContext->swapchainAttachmentPtr);
}
static void repopulateSwapchain(GfxContext *pGfxContext)
{
    VkDevice vkDevice = pGfxContext->vkDevice;
    VkExtent2D swapchainExtent = pGfxContext->swapchainExtent;
    assertVkResult(vkDeviceWaitIdle(vkDevice));
    cleanupSwapchain(pGfxContext);
    populateSwapchain(pGfxContext, swapchainExtent, pGfxContext->swapchainImageCount);
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
    pGfxContext->gfxVkCommandBuffers = tknMalloc(sizeof(VkCommandBuffer) * pGfxContext->swapchainImageCount);
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pGfxContext->gfxVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = pGfxContext->swapchainImageCount,
    };
    assertVkResult(vkAllocateCommandBuffers(pGfxContext->vkDevice, &vkCommandBufferAllocateInfo, pGfxContext->gfxVkCommandBuffers));
}
static void cleanupVkCommandBuffers(GfxContext *pGfxContext)
{
    vkFreeCommandBuffers(pGfxContext->vkDevice, pGfxContext->gfxVkCommandPool, pGfxContext->swapchainImageCount, pGfxContext->gfxVkCommandBuffers);
    tknFree(pGfxContext->gfxVkCommandBuffers);
}

void setupRenderPipeline(GfxContext *pGfxContext)
{
    pGfxContext->renderPassPtrDynamicArray = tknCreateDynamicArray(sizeof(RenderPass *), 4);
}
void teardownRenderPipeline(GfxContext *pGfxContext)
{
    for (uint32_t pRenderPassIndex = 0; pRenderPassIndex < pGfxContext->renderPassPtrDynamicArray.count; pRenderPassIndex++)
    {
        RenderPass *pRenderPass = tknGetFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, pRenderPassIndex);
        destroyRenderPassPtr(pGfxContext, pRenderPass);
    }
    tknDestroyDynamicArray(pGfxContext->renderPassPtrDynamicArray);
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
    // TODO: Record rendering commands here
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
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGfxContext->renderFinishedSemaphore},
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]){pGfxContext->vkSwapchain},
        .pImageIndices = &swapchainIndex,
        .pResults = NULL,
    };
    VkResult result = vkQueuePresentKHR(pGfxContext->vkPresentQueue, &presentInfo);
    if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
    {
        printf("Recreate swapchain because of the result: %d when presenting.\n", result);
        repopulateSwapchain(pGfxContext);
        for (uint32_t renderPassIndex = 0; renderPassIndex < pGfxContext->renderPassPtrDynamicArray.count; renderPassIndex++)
        {
            RenderPass *pRenderPass = tknGetFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, renderPassIndex);
            if (pRenderPass->useSwapchain)
            {
                cleanupFramebuffers(pGfxContext, pRenderPass);
                populateFramebuffers(pGfxContext, pRenderPass);
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

GfxContext *createGfxContextPtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent)
{
    GfxContext *pGfxContext = tknMalloc(sizeof(GfxContext));
    initializeGfxContext(pGfxContext, vkInstance, vkSurface);
    pickPhysicalDevice(pGfxContext, targetVkSurfaceFormat, targetVkPresentMode);
    populateLogicalDevice(pGfxContext);
    populateSwapchain(pGfxContext, swapchainExtent, targetSwapchainImageCount);

    populateSignals(pGfxContext);
    populateCommandPools(pGfxContext);
    populateVkCommandBuffers(pGfxContext);
    setupRenderPipeline(pGfxContext);
    return pGfxContext;
}

void destroyGfxContextPtr(GfxContext *pGfxContext)
{
    teardownRenderPipeline(pGfxContext);
    cleanupVkCommandBuffers(pGfxContext);
    cleanupCommandPools(pGfxContext);
    cleanupSignals(pGfxContext);
    cleanupSwapchain(pGfxContext);
    cleanupLogicalDevice(pGfxContext);
}
void updateGfxContextPtr(GfxContext *pGfxContext, VkExtent2D swapchainExtent)
{
    pGfxContext->frameCount++;
    uint32_t swapchainIndex = pGfxContext->frameCount % pGfxContext->swapchainImageCount;
    VkDevice vkDevice = pGfxContext->vkDevice;

    for (uint32_t renderPassIndex = 0; renderPassIndex < pGfxContext->renderPassPtrDynamicArray.count; renderPassIndex++)
    {
        /* code */
    }

    if (swapchainExtent.width != pGfxContext->swapchainExtent.width || swapchainExtent.height != pGfxContext->swapchainExtent.height)
    {
        printf("Recreate swapchain because of a size change: (%d, %d) to (%d, %d) \n",
               pGfxContext->swapchainExtent.width,
               pGfxContext->swapchainExtent.height,
               swapchainExtent.width,
               swapchainExtent.height);
        pGfxContext->swapchainExtent = swapchainExtent;
        repopulateSwapchain(pGfxContext);
        for (uint32_t attachmentPtrIndex = 0; attachmentPtrIndex < pGfxContext->dynamicAttachmentPtrDynamicArray.count; attachmentPtrIndex++)
        {
            Attachment *pAttachment = tknGetFromDynamicArray(&pGfxContext->dynamicAttachmentPtrDynamicArray, attachmentPtrIndex);
            resizeDynamicAttachmentPtr(pGfxContext, pAttachment);
        }
        for (uint32_t renderPassIndex = 0; renderPassIndex < pGfxContext->renderPassPtrDynamicArray.count; renderPassIndex++)
        {
            RenderPass *pRenderPass = tknGetFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, renderPassIndex);
            if (pRenderPass->attachmentCount > 0 && (pRenderPass->attachmentPtrs[0]->attachmentType == ATTACHMENT_TYPE_SWAPCHAIN || pRenderPass->attachmentPtrs[0]->attachmentType == ATTACHMENT_TYPE_DYNAMIC))
            {
                cleanupFramebuffers(pGfxContext, pRenderPass);
                populateFramebuffers(pGfxContext, pRenderPass);
            }
            else
            {
                // Don't need to recreate framebuffers
            }
        }
        // TODO Recreate subpass descriptor sets for input attachments
    }
    else
    {
        VkResult result = vkAcquireNextImageKHR(vkDevice, pGfxContext->vkSwapchain, UINT64_MAX, pGfxContext->imageAvailableSemaphore, VK_NULL_HANDLE, &swapchainIndex);
        if (result != VK_SUCCESS)
        {
            if (VK_ERROR_OUT_OF_DATE_KHR == result)
            {
                printf("Recreate swapchain because of result: %d\n", result);
                repopulateSwapchain(pGfxContext);
                for (uint32_t renderPassIndex = 0; renderPassIndex < pGfxContext->renderPassPtrDynamicArray.count; renderPassIndex++)
                {
                    RenderPass *pRenderPass = tknGetFromDynamicArray(&pGfxContext->renderPassPtrDynamicArray, renderPassIndex);

                    if (pRenderPass->useSwapchain)
                    {
                        cleanupFramebuffers(pGfxContext, pRenderPass);
                        populateFramebuffers(pGfxContext, pRenderPass);
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
void waitGfxContextPtr(GfxContext *pGfxContext)
{
    assertVkResult(vkWaitForFences(pGfxContext->vkDevice, 1, &pGfxContext->renderFinishedFence, VK_TRUE, UINT64_MAX));
}