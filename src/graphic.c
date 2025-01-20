#include "graphic.h"

#define GET_ARRAY_COUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))
void TryThrowVulkanError(VkResult vkResult)
{
    if (vkResult != VK_SUCCESS)
    {
        TickernelError("Vulkan error code: %d\n", vkResult);
    }
}

static void HasAllRequiredExtensions(GraphicContext *pGraphicContext, VkPhysicalDevice vkPhysicalDevice, bool *pHasAllRequiredExtensions)
{
    VkResult result = VK_SUCCESS;
    char *requiredExtensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    uint32_t requiredExtensionCount = GET_ARRAY_COUNT(requiredExtensionNames);
    
    uint32_t extensionCount = 0;
    result = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, NULL);
    TryThrowVulkanError(result);
    VkExtensionProperties *extensionProperties = TickernelMalloc(extensionCount * sizeof(VkExtensionProperties));
    result = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, extensionProperties);
    TryThrowVulkanError(result);
    
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
    TickernelFree(extensionProperties);
}

static void HasGraphicsAndPresentQueueFamilies(GraphicContext *pGraphicContext, VkPhysicalDevice vkPhysicalDevice, bool *pHasGraphicsAndPresentQueueFamilies, uint32_t *pGraphicsQueueFamilyIndex, uint32_t *pPresentQueueFamilyIndex)
{
    
    VkSurfaceKHR vkSurface = pGraphicContext->vkSurface;
    uint32_t *pQueueFamilyPropertyCount = &pGraphicContext->queueFamilyPropertyCount;
    
    VkResult result = VK_SUCCESS;
    
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, pQueueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *vkQueueFamilyPropertiesList = TickernelMalloc(*pQueueFamilyPropertyCount * sizeof(VkQueueFamilyProperties));
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
        TryThrowVulkanError(result);
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
    TickernelFree(vkQueueFamilyPropertiesList);
}

static void PickPhysicalDevice(GraphicContext *pGraphicContext)
{
    
    VkResult result = VK_SUCCESS;
    uint32_t deviceCount = -1;
    result = vkEnumeratePhysicalDevices(pGraphicContext->vkInstance, &deviceCount, NULL);
    TryThrowVulkanError(result);
    
    if (deviceCount <= 0)
    {
        printf("failed to find GPUs with Vulkan support!");
    }
    else
    {
        VkPhysicalDevice *devices = TickernelMalloc(deviceCount * sizeof(VkPhysicalDevice));
        result = vkEnumeratePhysicalDevices(pGraphicContext->vkInstance, &deviceCount, devices);
        TryThrowVulkanError(result);
        
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
            
            bool hasAllRequiredExtensions;
            HasAllRequiredExtensions(pGraphicContext, vkPhysicalDevice, &hasAllRequiredExtensions);
            TryThrowVulkanError(result);
            bool hasGraphicsAndPresentQueueFamilies;
            HasGraphicsAndPresentQueueFamilies(pGraphicContext, vkPhysicalDevice, &hasGraphicsAndPresentQueueFamilies, &graphicQueueFamilyIndex, &presentQueueFamilyIndex);
            
            uint32_t surfaceFormatCount;
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, NULL);
            TryThrowVulkanError(result);
            bool hasSurfaceFormat = surfaceFormatCount > 0;
            uint32_t presentModeCount;
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, NULL);
            TryThrowVulkanError(result);
            bool hasPresentMode = presentModeCount > 0;
            if (hasAllRequiredExtensions && hasGraphicsAndPresentQueueFamilies && hasSurfaceFormat && hasPresentMode)
            {
                uint32_t formatCount = 0;
                uint32_t modeCount = 0;
                result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &formatCount, NULL);
                TryThrowVulkanError(result);
                result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &modeCount, NULL);
                TryThrowVulkanError(result);
                
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
        TickernelFree(devices);
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

static void CreateLogicalDevice(GraphicContext *pGraphicContext)
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
        queueCreateInfos = TickernelMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
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
        queueCreateInfos = TickernelMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
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
    TryThrowVulkanError(result);
    vkGetDeviceQueue(pGraphicContext->vkDevice, graphicQueueFamilyIndex, 0, &pGraphicContext->vkGraphicQueue);
    vkGetDeviceQueue(pGraphicContext->vkDevice, presentQueueFamilyIndex, 0, &pGraphicContext->vkPresentQueue);
    TickernelFree(queueCreateInfos);
}

static void DestroyLogicalDevice(GraphicContext *pGraphicContext)
{
    vkDestroyDevice(pGraphicContext->vkDevice, NULL);
}

static void ChooseSurfaceFormat(VkSurfaceFormatKHR *surfaceFormats, uint32_t surfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormat)
{
    for (uint32_t i = 0; i < surfaceFormatCount; i++)
    {
        // Return srgb & nonlinears
        VkSurfaceFormatKHR surfaceFormat = surfaceFormats[i];
        if (VK_FORMAT_UNDEFINED == surfaceFormat.format)
        {
            VkSurfaceFormatKHR format =
            {
                .format = VK_FORMAT_R8G8B8A8_SRGB,
                .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            };
            *pSurfaceFormat = format;
        }
        else
        {
            if (VK_FORMAT_R8G8B8A8_SRGB == surfaceFormat.format && VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == surfaceFormat.colorSpace)
            {
                *pSurfaceFormat = surfaceFormat;
            }
            else
            {
                // continue;s
            }
        }
    }
    *pSurfaceFormat = surfaceFormats[0];
}
static void ChoosePresentMode(VkPresentModeKHR *supportPresentModes, uint32_t supportPresentModeCount, VkPresentModeKHR targetPresentMode, VkPresentModeKHR *pPresentMode)
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

static void CreateSwapchain(GraphicContext *pGraphicContext, uint32_t swapchainWidth, uint32_t swapchainHeight)
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
    TryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &supportSurfaceFormatCount, NULL);
    TryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &supportPresentModeCount, NULL);
    TryThrowVulkanError(result);
    VkSurfaceFormatKHR *supportSurfaceFormats = TickernelMalloc(supportSurfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    VkPresentModeKHR *supportPresentModes = TickernelMalloc(supportPresentModeCount * sizeof(VkPresentModeKHR));
    
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &supportSurfaceFormatCount, supportSurfaceFormats);
    TryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &supportPresentModeCount, supportPresentModes);
    TryThrowVulkanError(result);
    
    VkPresentModeKHR presentMode;
    ChooseSurfaceFormat(supportSurfaceFormats, supportSurfaceFormatCount, &pGraphicContext->surfaceFormat);
    ChoosePresentMode(supportPresentModes, supportPresentModeCount, pGraphicContext->targetPresentMode, &presentMode);
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
    TryThrowVulkanError(result);
    
    TickernelFree(supportSurfaceFormats);
    TickernelFree(supportPresentModes);
    
    result = vkGetSwapchainImagesKHR(vkDevice, pGraphicContext->vkSwapchain, &pGraphicContext->swapchainImageCount, NULL);
    TryThrowVulkanError(result);
    pGraphicContext->swapchainImages = TickernelMalloc(swapchainImageCount * sizeof(VkImage));
    result = vkGetSwapchainImagesKHR(vkDevice, pGraphicContext->vkSwapchain, &pGraphicContext->swapchainImageCount, pGraphicContext->swapchainImages);
    TryThrowVulkanError(result);
    pGraphicContext->swapchainImageViews = TickernelMalloc(swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        CreateImageView(pGraphicContext->vkDevice, pGraphicContext->swapchainImages[i], pGraphicContext->surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicContext->swapchainImageViews[i]);
    }
}
static void DestroySwapchain(GraphicContext *pGraphicContext)
{
    for (uint32_t i = 0; i < pGraphicContext->swapchainImageCount; i++)
    {
        vkDestroyImageView(pGraphicContext->vkDevice, pGraphicContext->swapchainImageViews[i], NULL);
    }
    TickernelFree(pGraphicContext->swapchainImageViews);
    TickernelFree(pGraphicContext->swapchainImages);
    vkDestroySwapchainKHR(pGraphicContext->vkDevice, pGraphicContext->vkSwapchain, NULL);
}

static void CreateSignals(GraphicContext *pGraphicContext)
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
    TryThrowVulkanError(result);
    result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicContext->renderFinishedSemaphore);
    TryThrowVulkanError(result);
    result = vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGraphicContext->renderFinishedFence);
    TryThrowVulkanError(result);
}

static void DestroySignals(GraphicContext *pGraphicContext)
{
    VkDevice vkDevice = pGraphicContext->vkDevice;
    vkDestroySemaphore(vkDevice, pGraphicContext->imageAvailableSemaphore, NULL);
    vkDestroySemaphore(vkDevice, pGraphicContext->renderFinishedSemaphore, NULL);
    vkDestroyFence(vkDevice, pGraphicContext->renderFinishedFence, NULL);
}

static void CreateGraphicImages(GraphicContext *pGraphicContext)
{
    VkExtent3D vkExtent3D = {
        .width = pGraphicContext->swapchainWidth,
        .height = pGraphicContext->swapchainHeight,
        .depth = 1,
    };
    VkFormat depthVkFormat;
    FindDepthFormat(pGraphicContext->vkPhysicalDevice, &depthVkFormat);
    CreateGraphicImage(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, vkExtent3D, depthVkFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, &pGraphicContext->depthGraphicImage);
    CreateGraphicImage(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, vkExtent3D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicContext->albedoGraphicImage);
    CreateGraphicImage(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, vkExtent3D, VK_FORMAT_A2R10G10B10_UNORM_PACK32, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicContext->normalGraphicImage);
    CreateGraphicImage(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, vkExtent3D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicContext->colorGraphicImage);
}

static void DestroyGraphicImages(GraphicContext *pGraphicContext)
{
    DestroyGraphicImage(pGraphicContext->vkDevice, pGraphicContext->normalGraphicImage);
    DestroyGraphicImage(pGraphicContext->vkDevice, pGraphicContext->albedoGraphicImage);
    DestroyGraphicImage(pGraphicContext->vkDevice, pGraphicContext->depthGraphicImage);
    DestroyGraphicImage(pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage);
}

static void RecreateSwapchain(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    uint32_t swapchainWidth = pGraphicContext->swapchainWidth;
    uint32_t swapchainHeight = pGraphicContext->swapchainHeight;
    printf("Recreate swapcahin with size: (%d, %d)", swapchainWidth, swapchainHeight);
    
    result = vkDeviceWaitIdle(pGraphicContext->vkDevice);
    TryThrowVulkanError(result);
    
    DestroySwapchain(pGraphicContext);
    CreateSwapchain(pGraphicContext, swapchainWidth, swapchainHeight);
    
    DestroyGraphicImages(pGraphicContext);
    CreateGraphicImages(pGraphicContext);
    
    UpdateDeferredRenderPass(&pGraphicContext->deferredRenderPass, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, pGraphicContext->depthGraphicImage, pGraphicContext->albedoGraphicImage, pGraphicContext->normalGraphicImage, swapchainWidth, swapchainHeight, pGraphicContext->globalUniformBuffer, pGraphicContext->lightsUniformBuffer);
    UpdatePostProcessRenderPass(&pGraphicContext->postProcessRenderPass, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, swapchainWidth, swapchainHeight, pGraphicContext->swapchainImageViews);
}

static void CreateCommandPools(GraphicContext *pGraphicContext)
{
    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = pGraphicContext->graphicQueueFamilyIndex,
    };
    VkResult result = vkCreateCommandPool(pGraphicContext->vkDevice, &vkCommandPoolCreateInfo, NULL, &pGraphicContext->graphicVkCommandPool);
    TryThrowVulkanError(result);
}

static void DestroyCommandPools(GraphicContext *pGraphicContext)
{
    vkDestroyCommandPool(pGraphicContext->vkDevice, pGraphicContext->graphicVkCommandPool, NULL);
}


static void SubmitCommandBuffer(GraphicContext *pGraphicContext)
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
    TryThrowVulkanError(result);
}

static void Present(GraphicContext *pGraphicContext)
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
    TryThrowVulkanError(result);
}

static void CreateVkCommandBuffers(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    pGraphicContext->graphicVkCommandBuffers = TickernelMalloc(sizeof(VkCommandBuffer) * pGraphicContext->swapchainImageCount);
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pGraphicContext->graphicVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = pGraphicContext->swapchainImageCount,
    };
    result = vkAllocateCommandBuffers(pGraphicContext->vkDevice, &vkCommandBufferAllocateInfo, pGraphicContext->graphicVkCommandBuffers);
    TryThrowVulkanError(result);
}

static void CreateUniformBuffers(GraphicContext *pGraphicContext)
{
    VkResult result = VK_SUCCESS;
    size_t bufferSize = sizeof(GlobalUniformBuffer);
    CreateBuffer(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pGraphicContext->globalUniformBuffer, &pGraphicContext->globalUniformBufferMemory);
    result = vkMapMemory(pGraphicContext->vkDevice, pGraphicContext->globalUniformBufferMemory, 0, bufferSize, 0, &pGraphicContext->globalUniformBufferMapped);
    TryThrowVulkanError(result);
    
    bufferSize = sizeof(LightsUniformBuffer);
    CreateBuffer(pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pGraphicContext->lightsUniformBuffer, &pGraphicContext->lightsUniformBufferMemory);
    result = vkMapMemory(pGraphicContext->vkDevice, pGraphicContext->lightsUniformBufferMemory, 0, bufferSize, 0, &pGraphicContext->lightsUniformBufferMapped);
    TryThrowVulkanError(result);
}

static void DestroyUniformBuffers(GraphicContext *pGraphicContext)
{
    vkUnmapMemory(pGraphicContext->vkDevice, pGraphicContext->lightsUniformBufferMemory);
    DestroyBuffer(pGraphicContext->vkDevice, pGraphicContext->lightsUniformBuffer, pGraphicContext->lightsUniformBufferMemory);
    
    vkUnmapMemory(pGraphicContext->vkDevice, pGraphicContext->globalUniformBufferMemory);
    DestroyBuffer(pGraphicContext->vkDevice, pGraphicContext->globalUniformBuffer, pGraphicContext->globalUniformBufferMemory);
}

static void UpdateGlobalUniformBuffer(GraphicContext *pGraphicContext)
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

static void UpdateLightsUniformBuffer(GraphicContext *pGraphicContext)
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

static void DestroyVkCommandBuffers(GraphicContext *pGraphicContext)
{
    vkFreeCommandBuffers(pGraphicContext->vkDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->swapchainImageCount, pGraphicContext->graphicVkCommandBuffers);
    TickernelFree(pGraphicContext->graphicVkCommandBuffers);
}

static void RecordCommandBuffer(GraphicContext *pGraphicContext)
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
    TryThrowVulkanError(result);
    
    RecordDeferredRenderPass(&pGraphicContext->deferredRenderPass, vkCommandBuffer, viewport, scissor, pGraphicContext->vkDevice);
    RecordPostProcessRenderPass(&pGraphicContext->postProcessRenderPass, vkCommandBuffer, viewport, scissor, pGraphicContext->vkDevice, pGraphicContext->swapchainIndex);
    
    result = vkEndCommandBuffer(vkCommandBuffer);
    TryThrowVulkanError(result);
}

GraphicContext *StartGraphic(const char *assetsPath, int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    GraphicContext *pGraphicContext = TickernelMalloc(sizeof(GraphicContext));
    pGraphicContext->assetsPath = assetsPath;
    pGraphicContext->targetSwapchainImageCount = targetSwapchainImageCount;
    pGraphicContext->targetPresentMode = targetPresentMode;
    pGraphicContext->vkInstance = vkInstance;
    pGraphicContext->vkSurface = vkSurface;
    pGraphicContext->canUpdateGlobalUniformBuffer = false;
    pGraphicContext->canUpdateLightsUniformBuffer = false;
    
    PickPhysicalDevice(pGraphicContext);
    CreateLogicalDevice(pGraphicContext);
    CreateSwapchain(pGraphicContext, swapchainWidth, swapchainHeight);
    CreateSignals(pGraphicContext);
    CreateCommandPools(pGraphicContext);
    CreateVkCommandBuffers(pGraphicContext);
    CreateUniformBuffers(pGraphicContext);
    CreateGraphicImages(pGraphicContext);
    
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
    CreateDeferredRenderPass(&pGraphicContext->deferredRenderPass, pGraphicContext->assetsPath, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, pGraphicContext->depthGraphicImage, pGraphicContext->albedoGraphicImage, pGraphicContext->normalGraphicImage, viewport, scissor, pGraphicContext->globalUniformBuffer, pGraphicContext->lightsUniformBuffer);
    CreatePostProcessRenderPass(&pGraphicContext->postProcessRenderPass, pGraphicContext->assetsPath, pGraphicContext->vkDevice, pGraphicContext->colorGraphicImage, viewport, scissor, pGraphicContext->swapchainImageCount, pGraphicContext->swapchainImageViews, pGraphicContext->surfaceFormat.format);
    return pGraphicContext;
}

void UpdateGraphic(GraphicContext *pGraphicContext, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    
    VkResult result = VK_SUCCESS;
    // Wait for gpu
    result = vkWaitForFences(pGraphicContext->vkDevice, 1, &pGraphicContext->renderFinishedFence, VK_TRUE, UINT64_MAX);
    TryThrowVulkanError(result);
    
    
    if (swapchainWidth != pGraphicContext->swapchainWidth || swapchainHeight != pGraphicContext->swapchainHeight)
    {
        pGraphicContext->swapchainWidth = swapchainWidth;
        pGraphicContext->swapchainHeight = swapchainHeight;
        printf("Recreate swapchain because of size.%d\n", result);
        RecreateSwapchain(pGraphicContext);
    }
    else
    {
        VkDevice vkDevice = pGraphicContext->vkDevice;
        result = vkAcquireNextImageKHR(vkDevice, pGraphicContext->vkSwapchain, UINT64_MAX, pGraphicContext->imageAvailableSemaphore, VK_NULL_HANDLE, &pGraphicContext->swapchainIndex);
        printf("pGraphicContext->swapchainIndex: %d\n", pGraphicContext->swapchainIndex);
        if (result != VK_SUCCESS)
        {
            if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
            {
                printf("Recreate swapchain because of result: %d\n", result);
                RecreateSwapchain(pGraphicContext);
            }
            else
            {
                TryThrowVulkanError(result);
            }
            
        }
        else
        {
            result = vkResetFences(pGraphicContext->vkDevice, 1, &pGraphicContext->renderFinishedFence);
            TryThrowVulkanError(result);
            
            UpdateGlobalUniformBuffer(pGraphicContext);
            UpdateLightsUniformBuffer(pGraphicContext);
            RecordCommandBuffer(pGraphicContext);
            SubmitCommandBuffer(pGraphicContext);
            Present(pGraphicContext);
        }
    }
}

void EndGraphic(GraphicContext *pGraphicContext)
{
    VkResult result = vkDeviceWaitIdle(pGraphicContext->vkDevice);
    TryThrowVulkanError(result);
    DestroyPostProcessRenderPass(&pGraphicContext->postProcessRenderPass, pGraphicContext->vkDevice);
    DestroyDeferredRenderPass(&pGraphicContext->deferredRenderPass, pGraphicContext->vkDevice);
    DestroyGraphicImages(pGraphicContext);
    DestroyUniformBuffers(pGraphicContext);
    DestroyVkCommandBuffers(pGraphicContext);
    DestroyCommandPools(pGraphicContext);
    DestroySignals(pGraphicContext);
    DestroySwapchain(pGraphicContext);
    DestroyLogicalDevice(pGraphicContext);
    // Destroy vkPhysicsDevice
    
    TickernelFree(pGraphicContext);
}
