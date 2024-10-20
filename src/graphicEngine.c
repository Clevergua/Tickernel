#include <graphicEngine.h>

#define GET_ARRAY_COUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))
void TryThrowVulkanError(VkResult vkResult)
{
    if (vkResult != VK_SUCCESS)
    {
        printf("Vulkan error code: %d\n", vkResult);
        abort();
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL LogMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    FILE *logStream = pUserData;
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        printf("Vulkan Error:%s\n", pCallbackData->pMessage);
    }
    else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        printf("Vulkan Warning:%s\n", pCallbackData->pMessage);
    }
    else
    {
        printf("Vulkan Log:%s\n", pCallbackData->pMessage);
    }
    return VK_TRUE;
}

static void CreateVkInstance(GraphicEngine *pGraphicEngine)
{
    char **enabledLayerNames;
    char **engineExtensionNames;
    PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback;

    uint32_t enabledLayerNamesCountInStack;
    uint32_t engineExtensionNamesCountInStack;
    void *pVkInstanceCreateInfoNext;
    char *engineExtensionNamesInStack[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#if PLATFORM_OSX
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif
    };
    if (pGraphicEngine->enableValidationLayers)
    {
        char *enabledLayerNamesInStack[] = {
            "VK_LAYER_KHRONOS_validation",
        };
        enabledLayerNamesCountInStack = GET_ARRAY_COUNT(enabledLayerNamesInStack);
        engineExtensionNamesCountInStack = GET_ARRAY_COUNT(engineExtensionNamesInStack);
        enabledLayerNames = enabledLayerNamesInStack;
        engineExtensionNames = engineExtensionNamesInStack;
        pfnUserCallback = LogMessenger;

        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = NULL,
                .flags = 0,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = pfnUserCallback,
                .pUserData = NULL,
            };
        pVkInstanceCreateInfoNext = &messengerCreateInfo;
    }
    else
    {
        enabledLayerNamesCountInStack = 0;
        enabledLayerNames = NULL;

        engineExtensionNamesCountInStack = GET_ARRAY_COUNT(engineExtensionNamesInStack);
        engineExtensionNames = engineExtensionNamesInStack;

        pfnUserCallback = NULL;
        pVkInstanceCreateInfoNext = NULL;
    }

    uint32_t enabledLayerCount = enabledLayerNamesCountInStack;
    uint32_t engineExtensionCount = engineExtensionNamesCountInStack;

    uint32_t extensionCount = 0;
    extensionCount += engineExtensionCount;

    uint32_t windowExtensionCount;
    TickernelGetWindowExtensionCount(&windowExtensionCount);
    char **windowExtensionNames = TickernelMalloc(sizeof(char *) * windowExtensionCount);
    TickernelGetWindowExtensions(windowExtensionNames);
    extensionCount += windowExtensionCount;

    char **extensionNames = TickernelMalloc(sizeof(char *) * extensionCount);
    memcpy(extensionNames, engineExtensionNames, engineExtensionCount * sizeof(char *));
    memcpy(extensionNames + engineExtensionCount, windowExtensionNames, windowExtensionCount * sizeof(char *));
    for (int i = 0; i < extensionCount; i++)
    {
        printf("Add extension: %s\n", extensionNames[i]);
    }
    VkApplicationInfo appInfo =
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = NULL,
            .pApplicationName = pGraphicEngine->name,
            .applicationVersion = 0,
            .pEngineName = NULL,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_3,
        };

    VkInstanceCreateInfo vkInstanceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = pVkInstanceCreateInfoNext,
#if PLATFORM_OSX
            .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#else
            .flags = 0,

#endif
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = enabledLayerCount,
            .ppEnabledLayerNames = (const char *const *)enabledLayerNames,
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = (const char *const *)extensionNames,
        };
    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, NULL, &pGraphicEngine->vkInstance);
    TickernelFree(windowExtensionNames);
    TickernelFree(extensionNames);
}
static void DestroyVKInstance(GraphicEngine *pGraphicEngine)
{
    vkDestroyInstance(pGraphicEngine->vkInstance, NULL);
}

static void CreateVKSurface(GraphicEngine *pGraphicEngine)
{
    VkResult result = CreateWindowVkSurface(&pGraphicEngine->tickernelWindow, pGraphicEngine->vkInstance, NULL, &pGraphicEngine->vkSurface);
    TryThrowVulkanError(result);
}

static void DestroyVKSurface(GraphicEngine *pGraphicEngine)
{
    vkDestroySurfaceKHR(pGraphicEngine->vkInstance, pGraphicEngine->vkSurface, NULL);
}

static void HasAllRequiredExtensions(GraphicEngine *pGraphicEngine, VkPhysicalDevice vkPhysicalDevice, bool *pHasAllRequiredExtensions)
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

static void HasGraphicsAndPresentQueueFamilies(GraphicEngine *pGraphicEngine, VkPhysicalDevice vkPhysicalDevice, bool *pHasGraphicsAndPresentQueueFamilies, uint32_t *pGraphicsQueueFamilyIndex, uint32_t *pPresentQueueFamilyIndex)
{

    VkSurfaceKHR vkSurface = pGraphicEngine->vkSurface;
    uint32_t *pQueueFamilyPropertyCount = &pGraphicEngine->queueFamilyPropertyCount;

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

static void PickPhysicalDevice(GraphicEngine *pGraphicEngine)
{

    VkResult result = VK_SUCCESS;
    uint32_t deviceCount = -1;
    result = vkEnumeratePhysicalDevices(pGraphicEngine->vkInstance, &deviceCount, NULL);
    TryThrowVulkanError(result);

    if (deviceCount <= 0)
    {
        printf("failed to find GPUs with Vulkan support!");
    }
    else
    {
        VkPhysicalDevice *devices = TickernelMalloc(deviceCount * sizeof(VkPhysicalDevice));
        result = vkEnumeratePhysicalDevices(pGraphicEngine->vkInstance, &deviceCount, devices);
        TryThrowVulkanError(result);

        uint32_t graphicQueueFamilyIndex = -1;
        uint32_t presentQueueFamilyIndex = -1;
        VkPhysicalDevice targetDevice = NULL;
        uint32_t maxScore = 0;
        char *targetDeviceName;

        VkSurfaceKHR vkSurface = pGraphicEngine->vkSurface;
        for (uint32_t i = 0; i < deviceCount; i++)
        {
            VkPhysicalDevice vkPhysicalDevice = devices[i];

            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

            bool hasAllRequiredExtensions;
            HasAllRequiredExtensions(pGraphicEngine, vkPhysicalDevice, &hasAllRequiredExtensions);
            TryThrowVulkanError(result);
            bool hasGraphicsAndPresentQueueFamilies;
            HasGraphicsAndPresentQueueFamilies(pGraphicEngine, vkPhysicalDevice, &hasGraphicsAndPresentQueueFamilies, &graphicQueueFamilyIndex, &presentQueueFamilyIndex);

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
                    if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == deviceProperties.deviceType)
                    {
                        score += 10;
                    }
                    else
                    {
                        // Keep score;
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
        if (NULL == targetDevice)
        {
            printf("failed to find GPUs with Vulkan support!");
        }
        else
        {
            pGraphicEngine->vkPhysicalDevice = targetDevice;
            pGraphicEngine->graphicQueueFamilyIndex = graphicQueueFamilyIndex;
            pGraphicEngine->presentQueueFamilyIndex = presentQueueFamilyIndex;
            printf("Selected target physical device named %s\n", targetDeviceName);
        }
    }
}

static void CreateLogicalDevice(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;

    VkPhysicalDevice vkPhysicalDevice = pGraphicEngine->vkPhysicalDevice;
    uint32_t graphicQueueFamilyIndex = pGraphicEngine->graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGraphicEngine->presentQueueFamilyIndex;
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
    char **enabledLayerNames;
    uint32_t enabledLayerCount;

    if (pGraphicEngine->enableValidationLayers)
    {
        char *enabledLayerNamesInStack[] = {
            "VK_LAYER_KHRONOS_validation",
        };
        enabledLayerCount = GET_ARRAY_COUNT(enabledLayerNamesInStack);
        enabledLayerNames = enabledLayerNamesInStack;
    }
    else
    {
        enabledLayerNames = NULL;
        enabledLayerCount = 0;
    }
    char *extensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if PLATFORM_OSX
        "VK_KHR_portability_subset",
#endif
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
    result = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, NULL, &pGraphicEngine->vkDevice);
    TryThrowVulkanError(result);
    vkGetDeviceQueue(pGraphicEngine->vkDevice, graphicQueueFamilyIndex, 0, &pGraphicEngine->vkGraphicQueue);
    vkGetDeviceQueue(pGraphicEngine->vkDevice, presentQueueFamilyIndex, 0, &pGraphicEngine->vkPresentQueue);
    TickernelFree(queueCreateInfos);
}

static void DestroyLogicalDevice(GraphicEngine *pGraphicEngine)
{
    vkDestroyDevice(pGraphicEngine->vkDevice, NULL);
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

static void CreateSwapchain(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;

    VkPhysicalDevice vkPhysicalDevice = pGraphicEngine->vkPhysicalDevice;
    VkSurfaceKHR vkSurface = pGraphicEngine->vkSurface;
    VkDevice vkDevice = pGraphicEngine->vkDevice;
    uint32_t graphicQueueFamilyIndex = pGraphicEngine->graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGraphicEngine->presentQueueFamilyIndex;

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
    ChooseSurfaceFormat(supportSurfaceFormats, supportSurfaceFormatCount, &pGraphicEngine->surfaceFormat);
    ChoosePresentMode(supportPresentModes, supportPresentModeCount, pGraphicEngine->targetPresentMode, &presentMode);
    uint32_t swapchainImageCount = pGraphicEngine->targetSwapchainImageCount;
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

    TickernelGetWindowFramebufferSize(&pGraphicEngine->tickernelWindow, &pGraphicEngine->width, &pGraphicEngine->height);
    VkExtent2D swapchainExtent;
    if (pGraphicEngine->width > vkSurfaceCapabilities.maxImageExtent.width)
    {
        swapchainExtent.width = vkSurfaceCapabilities.maxImageExtent.width;
    }
    else
    {
        if (pGraphicEngine->width < vkSurfaceCapabilities.minImageExtent.width)
        {
            swapchainExtent.width = vkSurfaceCapabilities.minImageExtent.width;
        }
        else
        {
            swapchainExtent.width = pGraphicEngine->width;
        }
    }

    if (pGraphicEngine->height > vkSurfaceCapabilities.maxImageExtent.height)
    {
        swapchainExtent.height = vkSurfaceCapabilities.maxImageExtent.height;
    }
    else
    {
        if (pGraphicEngine->height < vkSurfaceCapabilities.minImageExtent.height)
        {
            swapchainExtent.height = vkSurfaceCapabilities.minImageExtent.height;
        }
        else
        {
            swapchainExtent.height = pGraphicEngine->height;
        }
    }
    VkSharingMode imageSharingMode;
    uint32_t queueFamilyIndexCount;
    uint32_t *pQueueFamilyIndices;
    if (graphicQueueFamilyIndex != presentQueueFamilyIndex)
    {
        imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        queueFamilyIndexCount = 2;
        pQueueFamilyIndices = (uint32_t[]){graphicQueueFamilyIndex, presentQueueFamilyIndex};
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
            .imageFormat = pGraphicEngine->surfaceFormat.format,
            .imageColorSpace = pGraphicEngine->surfaceFormat.colorSpace,
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

    result = vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &pGraphicEngine->vkSwapchain);
    TryThrowVulkanError(result);

    TickernelFree(supportSurfaceFormats);
    TickernelFree(supportPresentModes);

    result = vkGetSwapchainImagesKHR(vkDevice, pGraphicEngine->vkSwapchain, &pGraphicEngine->swapchainImageCount, NULL);
    TryThrowVulkanError(result);
    pGraphicEngine->swapchainImages = TickernelMalloc(swapchainImageCount * sizeof(VkImage));
    result = vkGetSwapchainImagesKHR(vkDevice, pGraphicEngine->vkSwapchain, &pGraphicEngine->swapchainImageCount, pGraphicEngine->swapchainImages);
    TryThrowVulkanError(result);
    pGraphicEngine->swapchainImageViews = TickernelMalloc(swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        CreateImageView(pGraphicEngine, pGraphicEngine->swapchainImages[i], pGraphicEngine->surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicEngine->swapchainImageViews[i]);
    }
}
static void DestroySwapchain(GraphicEngine *pGraphicEngine)
{
    for (uint32_t i = 0; i < pGraphicEngine->swapchainImageCount; i++)
    {
        vkDestroyImageView(pGraphicEngine->vkDevice, pGraphicEngine->swapchainImageViews[i], NULL);
    }
    TickernelFree(pGraphicEngine->swapchainImageViews);
    TickernelFree(pGraphicEngine->swapchainImages);
    vkDestroySwapchainKHR(pGraphicEngine->vkDevice, pGraphicEngine->vkSwapchain, NULL);
}

static void CreateSignals(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };
    VkDevice vkDevice = pGraphicEngine->vkDevice;

    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicEngine->imageAvailableSemaphore);
    TryThrowVulkanError(result);
    result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicEngine->renderFinishedSemaphore);
    TryThrowVulkanError(result);
    result = vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGraphicEngine->renderFinishedFence);
    TryThrowVulkanError(result);
}

static void DestroySignals(GraphicEngine *pGraphicEngine)
{
    VkDevice vkDevice = pGraphicEngine->vkDevice;
    vkDestroySemaphore(vkDevice, pGraphicEngine->imageAvailableSemaphore, NULL);
    vkDestroySemaphore(vkDevice, pGraphicEngine->renderFinishedSemaphore, NULL);
    vkDestroyFence(vkDevice, pGraphicEngine->renderFinishedFence, NULL);
}

static void CreateGraphicImages(GraphicEngine *pGraphicEngine)
{
    VkExtent3D vkExtent3D = {
        .width = pGraphicEngine->width,
        .height = pGraphicEngine->height,
        .depth = 1,
    };
    VkFormat depthVkFormat;
    FindDepthFormat(pGraphicEngine, &depthVkFormat);
    CreateGraphicImage(pGraphicEngine, vkExtent3D, depthVkFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, &pGraphicEngine->depthGraphicImage);
    CreateGraphicImage(pGraphicEngine, vkExtent3D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicEngine->albedoGraphicImage);
    CreateGraphicImage(pGraphicEngine, vkExtent3D, VK_FORMAT_A2R10G10B10_UNORM_PACK32, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicEngine->normalGraphicImage);
}

static void DestroyGraphicImages(GraphicEngine *pGraphicEngine)
{
    DestroyGraphicImage(pGraphicEngine, pGraphicEngine->normalGraphicImage);
    DestroyGraphicImage(pGraphicEngine, pGraphicEngine->albedoGraphicImage);
    DestroyGraphicImage(pGraphicEngine, pGraphicEngine->depthGraphicImage);
}

static void RecreateSwapchain(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;

    uint32_t width, height;
    TickernelGetWindowFramebufferSize(&pGraphicEngine->tickernelWindow, &width, &height);
    while (0 == width || 0 == height)
    {
        TickernelGetWindowFramebufferSize(&pGraphicEngine->tickernelWindow, &width, &height);
        TickernelWaitWindowEvent();
    }
    if (width != pGraphicEngine->width || height != pGraphicEngine->height)
    {
        printf("Swapchain's width and height have changed! (%d, %d) to (%d, %d)", pGraphicEngine->width, pGraphicEngine->height, width, height);
        pGraphicEngine->width = width;
        pGraphicEngine->height = height;
    }
    else
    {
        // Do nothing
    }
    result = vkDeviceWaitIdle(pGraphicEngine->vkDevice);
    TryThrowVulkanError(result);

    DestroySwapchain(pGraphicEngine);
    CreateSwapchain(pGraphicEngine);

    DestroyGraphicImages(pGraphicEngine);
    CreateGraphicImages(pGraphicEngine);

    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    RecreateLightingSubpassModel(pGraphicEngine);
    for (uint32_t i = 0; i < pDeferredRenderPass->vkFramebufferCount; i++)
    {
        if (pDeferredRenderPass->vkFramebuffers[i] == INVALID_VKFRAMEBUFFER)
        {
            // continue;
        }
        else
        {
            vkDestroyFramebuffer(pGraphicEngine->vkDevice, pDeferredRenderPass->vkFramebuffers[i], NULL);
            pDeferredRenderPass->vkFramebuffers[i] = INVALID_VKFRAMEBUFFER;
        }
    }
}

static void CreateCommandPools(GraphicEngine *pGraphicEngine)
{
    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = pGraphicEngine->graphicQueueFamilyIndex,
    };
    VkResult result = vkCreateCommandPool(pGraphicEngine->vkDevice, &vkCommandPoolCreateInfo, NULL, &pGraphicEngine->graphicVkCommandPool);
    TryThrowVulkanError(result);
}

static void DestroyCommandPools(GraphicEngine *pGraphicEngine)
{
    vkDestroyCommandPool(pGraphicEngine->vkDevice, pGraphicEngine->graphicVkCommandPool, NULL);
}

static void WaitForGPU(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;
    // Wait for gpu
    result = vkWaitForFences(pGraphicEngine->vkDevice, 1, &pGraphicEngine->renderFinishedFence, VK_TRUE, UINT64_MAX);
    TryThrowVulkanError(result);
    result = vkResetFences(pGraphicEngine->vkDevice, 1, &pGraphicEngine->renderFinishedFence);
    TryThrowVulkanError(result);
}

static void SubmitCommandBuffer(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;

    VkDevice vkDevice = pGraphicEngine->vkDevice;
    uint32_t frameIndex = pGraphicEngine->frameIndex;
    // Submit workflow...
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGraphicEngine->imageAvailableSemaphore},
        .pWaitDstStageMask = (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        .commandBufferCount = 1,
        .pCommandBuffers = &pGraphicEngine->graphicVkCommandBuffers[frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = (VkSemaphore[]){pGraphicEngine->renderFinishedSemaphore},
    };

    result = vkQueueSubmit(pGraphicEngine->vkGraphicQueue, 1, &submitInfo, pGraphicEngine->renderFinishedFence);
    TryThrowVulkanError(result);
}

static void Present(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;
    uint32_t frameIndex = pGraphicEngine->frameIndex;
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGraphicEngine->renderFinishedSemaphore},
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]){pGraphicEngine->vkSwapchain},
        .pImageIndices = &pGraphicEngine->acquiredImageIndex,
        .pResults = NULL,
    };
    result = vkQueuePresentKHR(pGraphicEngine->vkPresentQueue, &presentInfo);
    if (result == VK_SUCCESS)
    {
        // continue
    }
    else
    {
        printf("Vulkan error code when Present: %d\n", result);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapchain(pGraphicEngine);
        }
        else
        {
            abort();
        }
    }
}

static void CreateVkCommandBuffers(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;
    pGraphicEngine->graphicVkCommandBuffers = TickernelMalloc(sizeof(VkCommandBuffer) * pGraphicEngine->swapchainImageCount);
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pGraphicEngine->graphicVkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = pGraphicEngine->swapchainImageCount,
    };
    result = vkAllocateCommandBuffers(pGraphicEngine->vkDevice, &vkCommandBufferAllocateInfo, pGraphicEngine->graphicVkCommandBuffers);
    TryThrowVulkanError(result);
}

static void CreateGlobalUniformBuffers(GraphicEngine *pGraphicEngine)
{
    size_t bufferSize = sizeof(GlobalUniformBuffer);
    VkResult result = VK_SUCCESS;
    CreateBuffer(pGraphicEngine, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pGraphicEngine->globalUniformBuffer, &pGraphicEngine->globalUniformBufferMemory);
    result = vkMapMemory(pGraphicEngine->vkDevice, pGraphicEngine->globalUniformBufferMemory, 0, bufferSize, 0, &pGraphicEngine->globalUniformBufferMapped);
    TryThrowVulkanError(result);
}

static void DestroyGlobalUniformBuffers(GraphicEngine *pGraphicEngine)
{
    vkUnmapMemory(pGraphicEngine->vkDevice, pGraphicEngine->globalUniformBufferMemory);
    DestroyBuffer(pGraphicEngine->vkDevice, pGraphicEngine->globalUniformBuffer, pGraphicEngine->globalUniformBufferMemory);
}

static void UpdateGlobalUniformBuffer(GraphicEngine *pGraphicEngine)
{
    GlobalUniformBuffer ubo;
    glm_lookat(pGraphicEngine->cameraPosition, pGraphicEngine->targetPosition, (vec3){0.0f, 0.0f, 1.0f}, ubo.view);
    float deg = 64.0f;
    // ubo.pointSizeFactor = 0.3f * pGraphicEngine->height / tanf(glm_rad(deg / 2));
    ubo.pointSizeFactor = 0.618f * pGraphicEngine->height / tanf(glm_rad(deg / 2));
    glm_perspective(glm_rad(deg), pGraphicEngine->width / (float)pGraphicEngine->height, 1.0f, 2048.0f, ubo.proj);
    ubo.proj[1][1] *= -1;
    mat4 view_proj;
    glm_mat4_mul(ubo.proj, ubo.proj, view_proj);
    glm_mat4_inv(view_proj, ubo.inv_view_proj);
    memcpy(pGraphicEngine->globalUniformBufferMapped, &ubo, sizeof(ubo));
}

static void DestroyVkCommandBuffers(GraphicEngine *pGraphicEngine)
{
    vkFreeCommandBuffers(pGraphicEngine->vkDevice, pGraphicEngine->graphicVkCommandPool, pGraphicEngine->swapchainImageCount, pGraphicEngine->graphicVkCommandBuffers);
    TickernelFree(pGraphicEngine->graphicVkCommandBuffers);
}

static void RecordCommandBuffer(GraphicEngine *pGraphicEngine)
{
    RecordDeferredRenderPass(pGraphicEngine);
}

void StartGraphicEngine(GraphicEngine *pGraphicEngine)
{
    pGraphicEngine->frameCount = 0;
    TickernelCreateWindow(pGraphicEngine->windowWidth, pGraphicEngine->windowHeight, "Tickernel Engine", &pGraphicEngine->tickernelWindow);
    CreateVkInstance(pGraphicEngine);
    CreateVKSurface(pGraphicEngine);
    PickPhysicalDevice(pGraphicEngine);
    CreateLogicalDevice(pGraphicEngine);
    CreateSwapchain(pGraphicEngine);
    CreateSignals(pGraphicEngine);
    CreateCommandPools(pGraphicEngine);
    CreateVkCommandBuffers(pGraphicEngine);
    CreateGlobalUniformBuffers(pGraphicEngine);
    CreateGraphicImages(pGraphicEngine);
    CreateDeferredRenderPass(pGraphicEngine);
}

void UpdateGraphicEngine(GraphicEngine *pGraphicEngine, bool *pCanUpdate)
{
    if (TickernelWindowShouldClose(&pGraphicEngine->tickernelWindow))
    {
        VkResult vkResult = vkDeviceWaitIdle(pGraphicEngine->vkDevice);
        TryThrowVulkanError(vkResult);
        *pCanUpdate = false;
    }
    else
    {
        pGraphicEngine->frameIndex = pGraphicEngine->frameCount % pGraphicEngine->swapchainImageCount;
        TickernelPollWindowEvents();
        WaitForGPU(pGraphicEngine);
        VkResult result = VK_SUCCESS;
        VkDevice vkDevice = pGraphicEngine->vkDevice;
        // Acquire next image
        result = vkAcquireNextImageKHR(vkDevice, pGraphicEngine->vkSwapchain, UINT64_MAX, pGraphicEngine->imageAvailableSemaphore, VK_NULL_HANDLE, &pGraphicEngine->acquiredImageIndex);
        if (VK_SUCCESS == result || VK_SUBOPTIMAL_KHR == result)
        {
            UpdateGlobalUniformBuffer(pGraphicEngine);
            RecordCommandBuffer(pGraphicEngine);
            SubmitCommandBuffer(pGraphicEngine);
            Present(pGraphicEngine);
            pGraphicEngine->frameCount++;
        }
        else
        {
            if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
            {
                RecreateSwapchain(pGraphicEngine);
                // pGraphicEngine->frameCount++;
                return;
            }
            else
            {
                TryThrowVulkanError(result);
            }
        }
    }
}

void EndGraphicEngine(GraphicEngine *pGraphicEngine)
{
    VkResult result = vkDeviceWaitIdle(pGraphicEngine->vkDevice);
    TryThrowVulkanError(result);

    DestroyDeferredRenderPass(pGraphicEngine);
    DestroyGraphicImages(pGraphicEngine);
    DestroyGlobalUniformBuffers(pGraphicEngine);
    DestroyVkCommandBuffers(pGraphicEngine);
    DestroyCommandPools(pGraphicEngine);
    DestroySignals(pGraphicEngine);
    // DestroyDepthResources(pGraphicEngine);
    DestroySwapchain(pGraphicEngine);
    DestroyLogicalDevice(pGraphicEngine);
    // Destroy vkPhysicsDevice
    DestroyVKSurface(pGraphicEngine);
    DestroyVKInstance(pGraphicEngine);
    TickernelDestroyWindow(&pGraphicEngine->tickernelWindow);
}