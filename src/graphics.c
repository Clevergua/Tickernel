#include "graphics.h"
static void initializeGraphicsContext(GraphicsContext *pGraphicsContext, VkInstance vkInstance, VkSurfaceKHR vkSurface)
{
    *pGraphicsContext = (GraphicsContext){
        .vkInstance = vkInstance,
        .vkSurface = vkSurface,

        .vkPhysicalDevice = VK_NULL_HANDLE,
        .vkPhysicalDeviceProperties = {},
        .graphicsQueueFamilyIndex = UINT32_MAX,
        .presentQueueFamilyIndex = UINT32_MAX,

        .surfaceFormat = {},
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,

        .vkDevice = VK_NULL_HANDLE,
        .vkGraphicsQueue = VK_NULL_HANDLE,
        .vkPresentQueue = VK_NULL_HANDLE,

        .swapchainExtent = {},
        .vkSwapchain = VK_NULL_HANDLE,
        .swapchainImageCount = 0,
        .swapchainImages = NULL,
        .swapchainImageViews = NULL,
        .swapchainIndex = 0,

        .imageAvailableSemaphore = VK_NULL_HANDLE,
        .renderFinishedSemaphore = VK_NULL_HANDLE,
        .renderFinishedFence = VK_NULL_HANDLE,

        .graphicsVkCommandPool = VK_NULL_HANDLE,
        .graphicsVkCommandBuffers = NULL,

        .renderPassPtrDynamicArray = {},
    };
}

static void getGraphicsAndPresentQueueFamilyIndices(GraphicsContext *pGraphicsContext, VkPhysicalDevice vkPhysicalDevice, uint32_t *pGraphicsQueueFamilyIndex, uint32_t *pPresentQueueFamilyIndex)
{
    VkSurfaceKHR vkSurface = pGraphicsContext->vkSurface;
    uint32_t queueFamilyPropertiesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyPropertiesCount, NULL);
    VkQueueFamilyProperties *vkQueueFamilyPropertiesArray = tknMalloc(queueFamilyPropertiesCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyPropertiesCount, vkQueueFamilyPropertiesArray);
    *pGraphicsQueueFamilyIndex = UINT32_MAX;
    *pPresentQueueFamilyIndex = UINT32_MAX;
    for (int queueFamilyPropertiesIndex = 0; queueFamilyPropertiesIndex < queueFamilyPropertiesCount; queueFamilyPropertiesIndex++)
    {
        VkQueueFamilyProperties vkQueueFamilyProperties = vkQueueFamilyPropertiesArray[queueFamilyPropertiesIndex];
        if (vkQueueFamilyProperties.queueCount > 0 && vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            *pGraphicsQueueFamilyIndex = queueFamilyPropertiesIndex;
        }
        else
        {
            // continue;
        }
        VkBool32 pSupported = VK_FALSE;
        ASSERT_VK_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, queueFamilyPropertiesIndex, vkSurface, &pSupported));
        if (vkQueueFamilyProperties.queueCount > 0 && pSupported)
        {
            *pPresentQueueFamilyIndex = queueFamilyPropertiesIndex;
        }
        else
        {
            // continue;
        }

        if (*pGraphicsQueueFamilyIndex != UINT32_MAX && *pPresentQueueFamilyIndex != UINT32_MAX)
        {
            break;
        }
    }
    tknFree(vkQueueFamilyPropertiesArray);
}

static void pickPhysicalDevice(GraphicsContext *pGraphicsContext, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode)
{
    uint32_t deviceCount = -1;
    ASSERT_VK_SUCCESS(vkEnumeratePhysicalDevices(pGraphicsContext->vkInstance, &deviceCount, NULL));
    if (deviceCount <= 0)
    {
        printf("failed to find GPUs with Vulkan support!");
    }
    else
    {
        VkPhysicalDevice *devices = tknMalloc(deviceCount * sizeof(VkPhysicalDevice));
        ASSERT_VK_SUCCESS(vkEnumeratePhysicalDevices(pGraphicsContext->vkInstance, &deviceCount, devices));

        uint32_t maxScore = 0;
        char *targetDeviceName = NULL;
        pGraphicsContext->vkPhysicalDevice = VK_NULL_HANDLE;
        VkSurfaceKHR vkSurface = pGraphicsContext->vkSurface;
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
            ASSERT_VK_SUCCESS(vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, NULL));
            VkExtensionProperties *extensionProperties = tknMalloc(extensionCount * sizeof(VkExtensionProperties));
            ASSERT_VK_SUCCESS(vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, extensionProperties));
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
                // found all
            }
            else
            {
                // not found
                continue;
            }

            uint32_t graphicsQueueFamilyIndex;
            uint32_t presentQueueFamilyIndex;
            getGraphicsAndPresentQueueFamilyIndices(pGraphicsContext, vkPhysicalDevice, &graphicsQueueFamilyIndex, &presentQueueFamilyIndex);
            if (UINT32_MAX == graphicsQueueFamilyIndex || UINT32_MAX == presentQueueFamilyIndex)
            {
                // No graphics or present queue family index
                continue;
            }

            uint32_t surfaceFormatCount;
            ASSERT_VK_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, NULL));
            VkSurfaceFormatKHR *supportedSurfaceFormats = tknMalloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
            ASSERT_VK_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, supportedSurfaceFormats));
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
            ASSERT_VK_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, NULL));
            VkPresentModeKHR *supportedPresentModes = tknMalloc(presentModeCount * sizeof(VkPresentModeKHR));
            ASSERT_VK_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, supportedPresentModes));
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
                pGraphicsContext->vkPhysicalDevice = vkPhysicalDevice;
                pGraphicsContext->graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
                pGraphicsContext->presentQueueFamilyIndex = presentQueueFamilyIndex;
                pGraphicsContext->vkPhysicalDeviceProperties = deviceProperties;
                pGraphicsContext->surfaceFormat = targetVkSurfaceFormat;
                pGraphicsContext->presentMode = targetVkPresentMode;
            }
            else
            {
                // continue
            }
        }
        tknFree(devices);

        if (pGraphicsContext->vkPhysicalDevice != NULL)
        {
            printf("Selected target physical device named %s\n", targetDeviceName);
        }
        else
        {
            tknError("failed to find GPUs with Vulkan support!");
        }
    }
}

static void createLogicalDevice(GraphicsContext *pGraphicsContext)
{
    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    uint32_t graphicsQueueFamilyIndex = pGraphicsContext->graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGraphicsContext->presentQueueFamilyIndex;
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo *queueCreateInfos;
    uint32_t queueCount;
    if (graphicsQueueFamilyIndex == presentQueueFamilyIndex)
    {
        queueCount = 1;
        queueCreateInfos = tknMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
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
        queueCreateInfos = tknMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
        VkDeviceQueueCreateInfo graphicsCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = graphicsQueueFamilyIndex,
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
        queueCreateInfos[0] = graphicsCreateInfo;
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
    ASSERT_VK_SUCCESS(vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, NULL, &pGraphicsContext->vkDevice));
    vkGetDeviceQueue(pGraphicsContext->vkDevice, graphicsQueueFamilyIndex, 0, &pGraphicsContext->vkGraphicsQueue);
    vkGetDeviceQueue(pGraphicsContext->vkDevice, presentQueueFamilyIndex, 0, &pGraphicsContext->vkPresentQueue);
    tknFree(queueCreateInfos);
}
static void destroyLogicalDevice(GraphicsContext *pGraphicsContext)
{
    vkDestroyDevice(pGraphicsContext->vkDevice, NULL);
}

static void createSwapchain(GraphicsContext *pGraphicsContext, VkExtent2D targetSwapchainExtent, uint32_t targetSwapchainImageCount)
{
    VkResult result = VK_SUCCESS;

    VkPhysicalDevice vkPhysicalDevice = pGraphicsContext->vkPhysicalDevice;
    VkSurfaceKHR vkSurface = pGraphicsContext->vkSurface;
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    uint32_t graphicsQueueFamilyIndex = pGraphicsContext->graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGraphicsContext->presentQueueFamilyIndex;

    VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
    uint32_t supportSurfaceFormatCount;
    uint32_t supportPresentModeCount;
    ASSERT_VK_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurface, &vkSurfaceCapabilities));
    pGraphicsContext->swapchainImageCount = TKN_CLAMP(targetSwapchainImageCount, vkSurfaceCapabilities.minImageCount, vkSurfaceCapabilities.maxImageCount);

    VkExtent2D swapchainExtent;
    swapchainExtent.width = TKN_CLAMP(targetSwapchainExtent.width, vkSurfaceCapabilities.minImageExtent.width, vkSurfaceCapabilities.maxImageExtent.width);
    swapchainExtent.height = TKN_CLAMP(targetSwapchainExtent.height, vkSurfaceCapabilities.minImageExtent.height, vkSurfaceCapabilities.maxImageExtent.height);

    VkSharingMode imageSharingMode;
    uint32_t queueFamilyIndexCount;
    uint32_t *pQueueFamilyIndices;
    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
    {
        imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        uint32_t queueFamilyIndices[] = {graphicsQueueFamilyIndex, presentQueueFamilyIndex};
        queueFamilyIndexCount = TKN_ARRAY_COUNT(queueFamilyIndices);
        pQueueFamilyIndices = queueFamilyIndices;
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
            .minImageCount = pGraphicsContext->swapchainImageCount,
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
            .presentMode = pGraphicsContext->presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };
    pGraphicsContext->swapchainAttachmentPtr = tknMalloc(sizeof(Attachment));
    pGraphicsContext->swapchainAttachmentPtr->attachmentType = ATTACHMENT_TYPE_SWAPCHAIN;
    ASSERT_VK_SUCCESS(vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &pGraphicsContext->vkSwapchain));
    pGraphicsContext->swapchainImages = tknMalloc(pGraphicsContext->swapchainImageCount * sizeof(VkImage));
    ASSERT_VK_SUCCESS(vkGetSwapchainImagesKHR(vkDevice, pGraphicsContext->vkSwapchain, &pGraphicsContext->swapchainImageCount, pGraphicsContext->swapchainImages));
    pGraphicsContext->swapchainImageViews = tknMalloc(pGraphicsContext->swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < pGraphicsContext->swapchainImageCount; i++)
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
            .image = pGraphicsContext->swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = pGraphicsContext->surfaceFormat.format,
            .components = components,
            .subresourceRange = subresourceRange,
        };
        ASSERT_VK_SUCCESS(vkCreateImageView(vkDevice, &imageViewCreateInfo, NULL, &pGraphicsContext->swapchainImageViews[i]));
    }
};
static void destroySwapchain(GraphicsContext *pGraphicsContext)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    for (uint32_t i = 0; i < pGraphicsContext->swapchainImageCount; i++)
    {
        vkDestroyImageView(vkDevice, pGraphicsContext->swapchainImageViews[i], NULL);
    }
    tknFree(pGraphicsContext->swapchainImageViews);
    tknFree(pGraphicsContext->swapchainImages);
    vkDestroySwapchainKHR(vkDevice, pGraphicsContext->vkSwapchain, NULL);
    tknFree(pGraphicsContext->swapchainAttachmentPtr);
}
static void recreateSwapchain(GraphicsContext *pGraphicsContext)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    VkExtent2D swapchainExtent = pGraphicsContext->swapchainExtent;
    ASSERT_VK_SUCCESS(vkDeviceWaitIdle(vkDevice));
    destroySwapchain(pGraphicsContext);
    createSwapchain(pGraphicsContext, swapchainExtent, pGraphicsContext->swapchainImageCount);
}

static void createSignals(GraphicsContext *pGraphicsContext)
{
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
    ASSERT_VK_SUCCESS(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicsContext->imageAvailableSemaphore));
    ASSERT_VK_SUCCESS(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicsContext->renderFinishedSemaphore));
    ASSERT_VK_SUCCESS(vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGraphicsContext->renderFinishedFence));
}
static void destroySignals(GraphicsContext *pGraphicsContext)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    vkDestroySemaphore(vkDevice, pGraphicsContext->imageAvailableSemaphore, NULL);
    vkDestroySemaphore(vkDevice, pGraphicsContext->renderFinishedSemaphore, NULL);
    vkDestroyFence(vkDevice, pGraphicsContext->renderFinishedFence, NULL);
}

static void createCommandPools(GraphicsContext *pGraphicsContext)
{
    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = pGraphicsContext->graphicsQueueFamilyIndex,
    };
    ASSERT_VK_SUCCESS(vkCreateCommandPool(pGraphicsContext->vkDevice, &vkCommandPoolCreateInfo, NULL, &pGraphicsContext->graphicsVkCommandPool));
}

static void destroyCommandPools(GraphicsContext *pGraphicsContext)
{
    vkDestroyCommandPool(pGraphicsContext->vkDevice, pGraphicsContext->graphicsVkCommandPool, NULL);
}
static void createVkCommandBuffers(GraphicsContext *pGraphicsContext)
{
    pGraphicsContext->graphicsVkCommandBuffers = tknMalloc(sizeof(VkCommandBuffer) * pGraphicsContext->swapchainImageCount);
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pGraphicsContext->graphicsVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = pGraphicsContext->swapchainImageCount,
    };
    ASSERT_VK_SUCCESS(vkAllocateCommandBuffers(pGraphicsContext->vkDevice, &vkCommandBufferAllocateInfo, pGraphicsContext->graphicsVkCommandBuffers));
}
static void destroyVkCommandBuffers(GraphicsContext *pGraphicsContext)
{
    vkFreeCommandBuffers(pGraphicsContext->vkDevice, pGraphicsContext->graphicsVkCommandPool, pGraphicsContext->swapchainImageCount, pGraphicsContext->graphicsVkCommandBuffers);
    tknFree(pGraphicsContext->graphicsVkCommandBuffers);
}

void setupRenderPipeline(GraphicsContext *pGraphicsContext)
{
    tknCreateDynamicArray(sizeof(RenderPass *), 4, &pGraphicsContext->renderPassPtrDynamicArray);
}
void teardownRenderPipeline(GraphicsContext *pGraphicsContext)
{
    for (uint32_t pRenderPassIndex = 0; pRenderPassIndex < pGraphicsContext->renderPassPtrDynamicArray.count; pRenderPassIndex++)
    {
        RenderPass *pRenderPass;
        tknGetFromDynamicArray(&pGraphicsContext->renderPassPtrDynamicArray, pRenderPassIndex, (void **)&pRenderPass);
        destroyRenderPass(pGraphicsContext, pRenderPass);
    }
    tknDestroyDynamicArray(pGraphicsContext->renderPassPtrDynamicArray);
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
    ASSERT_VK_SUCCESS(vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo));
    // for (uint32_t pRenderPassIndex = 0; pRenderPassIndex < pGraphicsContext->renderPassPtrDynamicArray.count; pRenderPassIndex++)
    // {
    //     RenderPass *pRenderPass = TICKERNEL_GET_FROM_DYNAMIC_ARRAY(&pGraphicsContext->renderPassPtrDynamicArray, pRenderPassIndex, RenderPass *);
    //     VkRenderPassBeginInfo renderPassBeginInfo =
    //         {
    //             .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    //             .pNext = NULL,
    //             .renderPass = pRenderPass->vkRenderPass,
    //             .framebuffer = pRenderPass->vkFramebuffers[swapchainIndex % pRenderPass->vkFramebufferCount],
    //             .renderArea = renderArea,
    //             .clearValueCount = clearValueCount,
    //             .pClearValues = clearValues,
    //         };
    //     ASSERT_VK_SUCCESS(vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE));
    //     for (uint32_t pSubpassIndex = 0; pSubpassIndex < pRenderPass->subpassCount; pSubpassIndex++)
    //     {
    //         Subpass *pSubpass = &pRenderPass->subpasses[pSubpassIndex];

    //         for (uint32_t pPipelineIndex = 0; pPipelineIndex < pSubpass->pipelinePtrDynamicArray.count; pPipelineIndex++)
    //         {
    //             Pipeline *pPipeline = TICKERNEL_GET_FROM_DYNAMIC_ARRAY(&pSubpass->pipelinePtrDynamicArray, pPipelineIndex, Pipeline *);
    //             vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipeline);
    //             for (uint32_t modelIndex = 0; modelIndex < pPipeline->materialPtrDynamicArray.count; modelIndex++)
    //             {
    //                 Material *pMaterial = TICKERNEL_GET_FROM_DYNAMIC_ARRAY(&pPipeline->materialPtrDynamicArray, modelIndex, Material *);
    //                 vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipelineLayout, 0, 1, &pMaterial->vkDescriptorSet, 0, NULL);
    //                 for (uint32_t meshIndex = 0; meshIndex < pMaterial->meshPtrDynamicArray.count; meshIndex++)
    //                 {
    //                     Mesh *pMesh = TICKERNEL_GET_FROM_DYNAMIC_ARRAY(&pMaterial->meshPtrDynamicArray, meshIndex, Mesh *);
    //                     if (pMesh->vertexCount > 0)
    //                     {
    //                         if (pMesh->indexCount > 0)
    //                         {
    //                             if (pMesh->instanceCount > 0)
    //                             {
    //                                 VkBuffer vertexBuffers[] = {pMesh->vertexBuffer.vkBuffer, pMesh->instanceMappedBuffer.buffer.vkBuffer};
    //                                 VkDeviceSize offsets[] = {0, 0};
    //                                 vkCmdBindVertexBuffers(vkCommandBuffer, 0, 2, vertexBuffers, offsets);
    //                                 vkCmdBindIndexBuffer(vkCommandBuffer, pMesh->indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);
    //                                 vkCmdDrawIndexed(vkCommandBuffer, pMesh->indexCount, pMesh->instanceCount, 0, 0, 0);
    //                             }
    //                             else
    //                             {
    //                                 VkBuffer vertexBuffers[] = {pMesh->vertexBuffer.vkBuffer};
    //                                 VkDeviceSize offsets[] = {0};
    //                                 vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
    //                                 vkCmdBindIndexBuffer(vkCommandBuffer, pMesh->indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);
    //                                 vkCmdDrawIndexed(vkCommandBuffer, pMesh->indexCount, 1, 0, 0, 0);
    //                             }
    //                         }
    //                         else
    //                         {
    //                             if (pMesh->instanceCount > 0)
    //                             {
    //                                 VkBuffer vertexBuffers[] = {pMesh->vertexBuffer.vkBuffer, pMesh->instanceMappedBuffer.buffer.vkBuffer};
    //                                 VkDeviceSize offsets[] = {0, 0};
    //                                 vkCmdBindVertexBuffers(vkCommandBuffer, 0, 2, vertexBuffers, offsets);
    //                                 vkCmdDraw(vkCommandBuffer, pMesh->vertexCount, pMesh->instanceCount, 0, 0);
    //                             }
    //                             else
    //                             {
    //                                 VkBuffer vertexBuffers[] = {pMesh->vertexBuffer.vkBuffer};
    //                                 VkDeviceSize offsets[] = {0};
    //                                 vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
    //                                 vkCmdDraw(vkCommandBuffer, pMesh->vertexCount, 1, 0, 0);
    //                             }
    //                         }
    //                     }
    //                     else
    //                     {
    //                         vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipelineLayout, 0, 1, &pMaterial->vkDescriptorSet, 0, NULL);
    //                         vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0);
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }

    ASSERT_VK_SUCCESS(vkEndCommandBuffer(vkCommandBuffer));
}

static void submitCommandBuffer(GraphicsContext *pGraphicsContext)
{
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

    ASSERT_VK_SUCCESS(vkQueueSubmit(pGraphicsContext->vkGraphicsQueue, 1, &submitInfo, pGraphicsContext->renderFinishedFence));
}

static void present(GraphicsContext *pGraphicsContext)
{
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
    VkResult result = vkQueuePresentKHR(pGraphicsContext->vkPresentQueue, &presentInfo);
    if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
    {
        printf("Recreate swapchain because of the result: %d when presenting.\n", result);
        recreateSwapchain(pGraphicsContext);
        for (uint32_t renderPassIndex = 0; renderPassIndex < pGraphicsContext->renderPassPtrDynamicArray.count; renderPassIndex++)
        {
            RenderPass *pRenderPass;
            tknGetFromDynamicArray(&pGraphicsContext->renderPassPtrDynamicArray, renderPassIndex, (void **)&pRenderPass);
            if (pRenderPass->useSwapchain)
            {
                destroyFramebuffers(pGraphicsContext, pRenderPass);
                createFramebuffers(pGraphicsContext, pRenderPass);
            }
            else
            {
                // Don't need to recreate framebuffers
            }
        }
    }
    else
    {
        ASSERT_VK_SUCCESS(result);
    }
}

void createGraphicsContext(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, GraphicsContext *pGraphicsContext)
{
    initializeGraphicsContext(pGraphicsContext, vkInstance, vkSurface);
    pickPhysicalDevice(pGraphicsContext, targetVkSurfaceFormat, targetVkPresentMode);
    createLogicalDevice(pGraphicsContext);
    createSwapchain(pGraphicsContext, swapchainExtent, targetSwapchainImageCount);
    createSignals(pGraphicsContext);
    createCommandPools(pGraphicsContext);
    createVkCommandBuffers(pGraphicsContext);
    setupRenderPipeline(pGraphicsContext);
}
void destroyGraphicsContext(GraphicsContext graphicsContext)
{
    teardownRenderPipeline(&graphicsContext);
    destroyVkCommandBuffers(&graphicsContext);
    destroyCommandPools(&graphicsContext);
    destroySignals(&graphicsContext);
    destroySwapchain(&graphicsContext);
    destroyLogicalDevice(&graphicsContext);
}
void updateGraphicsContext(GraphicsContext *pGraphicsContext, VkExtent2D swapchainExtent)
{
    VkDevice vkDevice = pGraphicsContext->vkDevice;
    // Wait for gpu
    ASSERT_VK_SUCCESS(vkWaitForFences(vkDevice, 1, &pGraphicsContext->renderFinishedFence, VK_TRUE, UINT64_MAX));

    if (swapchainExtent.width != pGraphicsContext->swapchainExtent.width || swapchainExtent.height != pGraphicsContext->swapchainExtent.height)
    {
        printf("Recreate swapchain because of a size change: (%d, %d) to (%d, %d) \n",
               pGraphicsContext->swapchainExtent.width,
               pGraphicsContext->swapchainExtent.height,
               swapchainExtent.width,
               swapchainExtent.height);
        pGraphicsContext->swapchainExtent = swapchainExtent;
        recreateSwapchain(pGraphicsContext);
        for (uint32_t attachmentPtrIndex = 0; attachmentPtrIndex < pGraphicsContext->dynamicAttachmentPtrDynamicArray.count; attachmentPtrIndex++)
        {
            Attachment *pAttachment;
            tknGetFromDynamicArray(&pGraphicsContext->dynamicAttachmentPtrDynamicArray, attachmentPtrIndex, (void **)&pAttachment);
            resizeDynamicAttachment(pGraphicsContext, pAttachment);
        }
        for (uint32_t renderPassIndex = 0; renderPassIndex < pGraphicsContext->renderPassPtrDynamicArray.count; renderPassIndex++)
        {
            RenderPass *pRenderPass;
            tknGetFromDynamicArray(&pGraphicsContext->renderPassPtrDynamicArray, renderPassIndex, (void **)&pRenderPass);
            if (pRenderPass->attachmentCount > 0 && pRenderPass->attachmentPtrs[0]->attachmentType == ATTACHMENT_TYPE_SWAPCHAIN || pRenderPass->attachmentPtrs[0]->attachmentType == ATTACHMENT_TYPE_DYNAMIC)
            {
                destroyFramebuffers(pGraphicsContext, pRenderPass);
                createFramebuffers(pGraphicsContext, pRenderPass);
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
        VkResult result = vkAcquireNextImageKHR(vkDevice, pGraphicsContext->vkSwapchain, UINT64_MAX, pGraphicsContext->imageAvailableSemaphore, VK_NULL_HANDLE, &pGraphicsContext->swapchainIndex);
        if (result != VK_SUCCESS)
        {
            if (VK_ERROR_OUT_OF_DATE_KHR == result)
            {
                printf("Recreate swapchain because of result: %d\n", result);
                recreateSwapchain(pGraphicsContext);
                for (uint32_t renderPassIndex = 0; renderPassIndex < pGraphicsContext->renderPassPtrDynamicArray.count; renderPassIndex++)
                {
                    RenderPass *pRenderPass;
                    tknGetFromDynamicArray(&pGraphicsContext->renderPassPtrDynamicArray, renderPassIndex, (void **)&pRenderPass);
                    if (pRenderPass->useSwapchain)
                    {
                        destroyFramebuffers(pGraphicsContext, pRenderPass);
                        createFramebuffers(pGraphicsContext, pRenderPass);
                    }
                    else
                    {
                        // Don't need to recreate framebuffers
                    }
                }
            }
            else if (VK_SUBOPTIMAL_KHR == result)
            {
                ASSERT_VK_SUCCESS(vkResetFences(vkDevice, 1, &pGraphicsContext->renderFinishedFence));
                recordCommandBuffer(pGraphicsContext);
                submitCommandBuffer(pGraphicsContext);
                present(pGraphicsContext);
                pGraphicsContext->swapchainIndex = (pGraphicsContext->swapchainIndex + 1) % pGraphicsContext->swapchainImageCount;
            }
            else
            {
                ASSERT_VK_SUCCESS(result);
            }
        }
        else
        {
            ASSERT_VK_SUCCESS(vkResetFences(vkDevice, 1, &pGraphicsContext->renderFinishedFence));
            recordCommandBuffer(pGraphicsContext);
            submitCommandBuffer(pGraphicsContext);
            present(pGraphicsContext);
            pGraphicsContext->swapchainIndex = (pGraphicsContext->swapchainIndex + 1) % pGraphicsContext->swapchainImageCount;
        }
    }
}
