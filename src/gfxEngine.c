#include <gfxEngine.h>

#define GETARRAYCOUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))

static void TryThrowVulkanError(VkResult vkResult)
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

static void CreateVkInstance(GFXEngine *pGFXEngine)
{
    char **enabledLayerNames;
    char **engineExtensionNames;
    PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback;

    uint32_t enabledLayerNamesCountInStack;
    uint32_t engineExtensionNamesCountInStack;
    void *pVkInstanceCreateInfoNext;
    char *engineExtensionNamesInStack[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    };
    if (pGFXEngine->enableValidationLayers)
    {
        char *enabledLayerNamesInStack[] = {
            "VK_LAYER_KHRONOS_validation",
        };

        enabledLayerNamesCountInStack = GETARRAYCOUNT(enabledLayerNamesInStack);
        engineExtensionNamesCountInStack = GETARRAYCOUNT(engineExtensionNamesInStack);
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

        engineExtensionNamesCountInStack = GETARRAYCOUNT(engineExtensionNamesInStack);
        engineExtensionNames = engineExtensionNamesInStack;

        pfnUserCallback = NULL;
        pVkInstanceCreateInfoNext = NULL;
    }

    uint32_t enabledLayerCount = enabledLayerNamesCountInStack;
    uint32_t engineExtensionCount = engineExtensionNamesCountInStack;

    uint32_t extensionCount = 0;
    extensionCount += engineExtensionCount;
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    extensionCount += glfwExtensionCount;

    char **extensionNames = TKNMalloc(extensionCount * sizeof(char *));
    memcpy(extensionNames, engineExtensionNames, engineExtensionCount * sizeof(char *));
    memcpy(extensionNames + engineExtensionCount, glfwExtensions, glfwExtensionCount * sizeof(char *));
    for (int i = 0; i < extensionCount; i++)
    {
        printf("Add extension: %s\n", extensionNames[i]);
    }
    VkApplicationInfo appInfo =
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = NULL,
            .pApplicationName = pGFXEngine->name,
            .applicationVersion = 0,
            .pEngineName = NULL,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_3,
        };

    VkInstanceCreateInfo vkInstanceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = pVkInstanceCreateInfoNext,
            .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = enabledLayerCount,
            .ppEnabledLayerNames = (const char *const *)enabledLayerNames,
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = (const char *const *)extensionNames,
        };
    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, NULL, &pGFXEngine->vkInstance);
    TKNFree(extensionNames);
}
static void DestroyVKInstance(GFXEngine *pGFXEngine)
{
    vkDestroyInstance(pGFXEngine->vkInstance, NULL);
}

static void CreateGLFWWindow(GFXEngine *pGFXEngine)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwInitHint(GLFW_RESIZABLE, GLFW_FALSE);
    pGFXEngine->pGLFWWindow = glfwCreateWindow(pGFXEngine->width, pGFXEngine->height, pGFXEngine->name, NULL, NULL);
}

static void DestroyGLFWWindow(GFXEngine *pGFXEngine)
{
    glfwDestroyWindow(pGFXEngine->pGLFWWindow);
    glfwTerminate();
}

static void CreateVKSurface(GFXEngine *pGFXEngine)
{
    VkResult result = glfwCreateWindowSurface(pGFXEngine->vkInstance, pGFXEngine->pGLFWWindow, NULL, &pGFXEngine->vkSurface);
    TryThrowVulkanError(result);
}

static void DestroyVKSurface(GFXEngine *pGFXEngine)
{
    vkDestroySurfaceKHR(pGFXEngine->vkInstance, pGFXEngine->vkSurface, NULL);
}

static void HasAllRequiredExtensions(GFXEngine *pGFXEngine, VkPhysicalDevice vkPhysicalDevice, bool *pHasAllRequiredExtensions)
{
    VkResult result = VK_SUCCESS;
    char *requiredExtensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    uint32_t requiredExtensionCount = GETARRAYCOUNT(requiredExtensionNames);

    uint32_t extensionCount = 0;
    result = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, NULL);
    TryThrowVulkanError(result);
    VkExtensionProperties *extensionProperties = TKNMalloc(extensionCount * sizeof(VkExtensionProperties));
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
    TKNFree(extensionProperties);
}

static void HasGraphicsAndPresentQueueFamilies(GFXEngine *pGFXEngine, VkPhysicalDevice vkPhysicalDevice, bool *pHasGraphicsAndPresentQueueFamilies, uint32_t *pGraphicsQueueFamilyIndex, uint32_t *pPresentQueueFamilyIndex)
{

    VkSurfaceKHR vkSurface = pGFXEngine->vkSurface;
    uint32_t *pQueueFamilyPropertyCount = &pGFXEngine->queueFamilyPropertyCount;

    VkResult result = VK_SUCCESS;

    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, pQueueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *vkQueueFamilyPropertiesList = TKNMalloc(*pQueueFamilyPropertyCount * sizeof(VkQueueFamilyProperties));
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
    TKNFree(vkQueueFamilyPropertiesList);
}

static void PickPhysicalDevice(GFXEngine *pGFXEngine)
{

    VkResult result = VK_SUCCESS;
    uint32_t deviceCount = -1;
    result = vkEnumeratePhysicalDevices(pGFXEngine->vkInstance, &deviceCount, NULL);
    TryThrowVulkanError(result);

    if (deviceCount <= 0)
    {
        printf("failed to find GPUs with Vulkan support!");
    }
    else
    {
        VkPhysicalDevice *devices = TKNMalloc(deviceCount * sizeof(VkPhysicalDevice));
        result = vkEnumeratePhysicalDevices(pGFXEngine->vkInstance, &deviceCount, devices);
        TryThrowVulkanError(result);

        uint32_t graphicQueueFamilyIndex = -1;
        uint32_t presentQueueFamilyIndex = -1;
        VkPhysicalDevice targetDevice = NULL;
        uint32_t maxScore = 0;
        char *targetDeviceName;

        VkSurfaceKHR vkSurface = pGFXEngine->vkSurface;
        for (uint32_t i = 0; i < deviceCount; i++)
        {
            VkPhysicalDevice vkPhysicalDevice = devices[i];

            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

            bool hasAllRequiredExtensions;
            HasAllRequiredExtensions(pGFXEngine, vkPhysicalDevice, &hasAllRequiredExtensions);
            TryThrowVulkanError(result);
            bool hasGraphicsAndPresentQueueFamilies;
            HasGraphicsAndPresentQueueFamilies(pGFXEngine, vkPhysicalDevice, &hasGraphicsAndPresentQueueFamilies, &graphicQueueFamilyIndex, &presentQueueFamilyIndex);

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
        TKNFree(devices);
        if (NULL == targetDevice)
        {
            printf("failed to find GPUs with Vulkan support!");
        }
        else
        {
            pGFXEngine->vkPhysicalDevice = targetDevice;
            pGFXEngine->graphicQueueFamilyIndex = graphicQueueFamilyIndex;
            pGFXEngine->presentQueueFamilyIndex = presentQueueFamilyIndex;
            printf("Selected target physical device named %s\n", targetDeviceName);
        }
    }
}

static void CreateLogicalDevice(GFXEngine *pGFXEngine)
{
    VkResult result = VK_SUCCESS;

    VkPhysicalDevice vkPhysicalDevice = pGFXEngine->vkPhysicalDevice;
    uint32_t graphicQueueFamilyIndex = pGFXEngine->graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGFXEngine->presentQueueFamilyIndex;
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo *queueCreateInfos;
    uint32_t queueCount;
    if (graphicQueueFamilyIndex == presentQueueFamilyIndex)
    {
        queueCount = 1;
        queueCreateInfos = TKNMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
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
        queueCreateInfos = TKNMalloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
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

    if (pGFXEngine->enableValidationLayers)
    {
        char *enabledLayerNamesInStack[] = {
            "VK_LAYER_KHRONOS_validation",
        };
        enabledLayerCount = GETARRAYCOUNT(enabledLayerNamesInStack);
        enabledLayerNames = enabledLayerNamesInStack;
    }
    else
    {
        enabledLayerNames = NULL;
        enabledLayerCount = 0;
    }
    char *extensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // "VK_KHR_portability_subset"
    };
    uint32_t extensionCount = GETARRAYCOUNT(extensionNames);
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
    result = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, NULL, &pGFXEngine->vkDevice);
    TryThrowVulkanError(result);
    vkGetDeviceQueue(pGFXEngine->vkDevice, graphicQueueFamilyIndex, 0, &pGFXEngine->vkGraphicQueue);
    vkGetDeviceQueue(pGFXEngine->vkDevice, presentQueueFamilyIndex, 0, &pGFXEngine->vkPresentQueue);
    TKNFree(queueCreateInfos);
}

static void DestroyLogicalDevice(GFXEngine *pGFXEngine)
{
    vkDestroyDevice(pGFXEngine->vkDevice, NULL);
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

static void CreateImageView(GFXEngine *pGFXEngine, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView)
{

    VkDevice vkDevice = pGFXEngine->vkDevice;
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
    TryThrowVulkanError(result);
}

static void CreateSwapchain(GFXEngine *pGFXEngine)
{
    VkResult result = VK_SUCCESS;

    VkPhysicalDevice vkPhysicalDevice = pGFXEngine->vkPhysicalDevice;
    VkSurfaceKHR vkSurface = pGFXEngine->vkSurface;
    VkDevice vkDevice = pGFXEngine->vkDevice;
    uint32_t graphicQueueFamilyIndex = pGFXEngine->graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGFXEngine->presentQueueFamilyIndex;

    VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
    uint32_t supportSurfaceFormatCount;
    uint32_t supportPresentModeCount;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurface, &vkSurfaceCapabilities);
    TryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &supportSurfaceFormatCount, NULL);
    TryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &supportPresentModeCount, NULL);
    TryThrowVulkanError(result);
    VkSurfaceFormatKHR *supportSurfaceFormats = TKNMalloc(supportSurfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    VkPresentModeKHR *supportPresentModes = TKNMalloc(supportPresentModeCount * sizeof(VkPresentModeKHR));

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &supportSurfaceFormatCount, supportSurfaceFormats);
    TryThrowVulkanError(result);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &supportPresentModeCount, supportPresentModes);
    TryThrowVulkanError(result);

    VkPresentModeKHR presentMode;
    ChooseSurfaceFormat(supportSurfaceFormats, supportSurfaceFormatCount, &pGFXEngine->surfaceFormat);
    ChoosePresentMode(supportPresentModes, supportPresentModeCount, pGFXEngine->targetPresentMode, &presentMode);
    uint32_t swapchainImageCount = pGFXEngine->targetSwapchainImageCount;
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

    pGFXEngine->swapchainExtent = vkSurfaceCapabilities.currentExtent;
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
            .imageFormat = pGFXEngine->surfaceFormat.format,
            .imageColorSpace = pGFXEngine->surfaceFormat.colorSpace,
            .imageExtent = pGFXEngine->swapchainExtent,
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

    result = vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &pGFXEngine->vkSwapchain);
    TryThrowVulkanError(result);

    TKNFree(supportSurfaceFormats);
    TKNFree(supportPresentModes);

    result = vkGetSwapchainImagesKHR(vkDevice, pGFXEngine->vkSwapchain, &pGFXEngine->swapchainImageCount, NULL);
    TryThrowVulkanError(result);
    pGFXEngine->swapchainImages = TKNMalloc(swapchainImageCount * sizeof(VkImage));
    result = vkGetSwapchainImagesKHR(vkDevice, pGFXEngine->vkSwapchain, &pGFXEngine->swapchainImageCount, pGFXEngine->swapchainImages);
    TryThrowVulkanError(result);
    pGFXEngine->swapchainImageViews = TKNMalloc(swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        CreateImageView(pGFXEngine, pGFXEngine->swapchainImages[i], pGFXEngine->surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &pGFXEngine->swapchainImageViews[i]);
    }
}
static void DestroySwapchain(GFXEngine *pGFXEngine)
{
    for (uint32_t i = 0; i < pGFXEngine->swapchainImageCount; i++)
    {
        vkDestroyImageView(pGFXEngine->vkDevice, pGFXEngine->swapchainImageViews[i], NULL);
    }
    TKNFree(pGFXEngine->swapchainImageViews);
    TKNFree(pGFXEngine->swapchainImages);
    vkDestroySwapchainKHR(pGFXEngine->vkDevice, pGFXEngine->vkSwapchain, NULL);
}

static void FindSupportedFormat(GFXEngine *pGFXEngine, VkFormat *candidates, uint32_t candidatesCount, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat *vkFormat)
{
    VkResult result = VK_SUCCESS;
    VkPhysicalDevice vkPhysicalDevice = pGFXEngine->vkPhysicalDevice;

    for (uint32_t i = 0; i < candidatesCount; i++)
    {
        VkFormat format = candidates[i];
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, format, &properties);
        if ((properties.optimalTilingFeatures & features) == features)
        {
            if (VK_IMAGE_TILING_LINEAR == tiling || VK_IMAGE_TILING_OPTIMAL == tiling)
            {
                *vkFormat = format;
                return;
            }
            else
            {
                // continue;
            }
        }
    }
    printf("Target format not found!");
}

static void FindDepthFormat(GFXEngine *pGFXEngine, VkFormat *pDepthFormat)
{
    uint32_t candidatesCount = 3;
    VkFormat *candidates = (VkFormat[]){VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    FindSupportedFormat(pGFXEngine, candidates, candidatesCount, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, pDepthFormat);
}

static void FindMemoryType(GFXEngine *pGFXEngine, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex)
{

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(pGFXEngine->vkPhysicalDevice, &physicalDeviceMemoryProperties);
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

static void CreateImage(GFXEngine *pGFXEngine, int32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *pImage, VkDeviceMemory *pImageMemory)
{
    VkResult result = VK_SUCCESS;

    VkDevice vkDevice = pGFXEngine->vkDevice;
    VkExtent3D extent = {
        .width = width,
        .height = height,
        .depth = 1,
    };

    VkImageCreateInfo imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = 0,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    result = vkCreateImage(vkDevice, &imageCreateInfo, NULL, pImage);
    TryThrowVulkanError(result);
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, *pImage, &memoryRequirements);
    uint32_t memoryTypeIndex;
    FindMemoryType(pGFXEngine, memoryRequirements.memoryTypeBits, properties, &memoryTypeIndex);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };

    result = vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pImageMemory);
    TryThrowVulkanError(result);
    result = vkBindImageMemory(vkDevice, *pImage, *pImageMemory, 0);
    TryThrowVulkanError(result);
}

static void CreateDepthResources(GFXEngine *pGFXEngine)
{
    FindDepthFormat(pGFXEngine, &pGFXEngine->depthFormat);
    CreateImage(pGFXEngine, pGFXEngine->swapchainExtent.width, pGFXEngine->swapchainExtent.height, pGFXEngine->depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pGFXEngine->depthImage, &pGFXEngine->depthImageMemory);

    CreateImageView(pGFXEngine, pGFXEngine->depthImage, pGFXEngine->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &pGFXEngine->depthImageView);
}

static void DestroyDepthResources(GFXEngine *pGFXEngine)
{
    VkDevice vkDevice = pGFXEngine->vkDevice;
    vkDestroyImageView(vkDevice, pGFXEngine->depthImageView, NULL);
    vkDestroyImage(vkDevice, pGFXEngine->depthImage, NULL);
    vkFreeMemory(vkDevice, pGFXEngine->depthImageMemory, NULL);
}

// static void CreateRenderPass(GFXEngine *pGFXEngine, VkRenderPass *pVkRenderPass)
// {
//     VkResult result = VK_SUCCESS;
//     VkAttachmentDescription colorAttachmentDescription = {
//         .flags = 0,
//         .format = pGFXEngine->surfaceFormat.format,
//         .samples = VK_SAMPLE_COUNT_1_BIT,
//         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
//         .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
//         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
//         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
//         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//         .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//     };

//     VkAttachmentDescription depthAttachmentDescription = {
//         .flags = 0,
//         .format = pGFXEngine->surfaceFormat.format,
//         .samples = VK_SAMPLE_COUNT_1_BIT,
//         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
//         .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
//         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
//         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
//         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//         .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//     };

//     VkAttachmentReference colorAttachmentReference = {
//         .attachment = 0,
//         .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//     };
//     VkAttachmentReference depthAttachmentReference = {
//         .attachment = 1,
//         .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
//     };

//     VkSubpassDescription subpassDescription = {
//         .flags = 0,
//         .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
//         .inputAttachmentCount = 0,
//         .pInputAttachments = NULL,
//         .colorAttachmentCount = 1,
//         .pColorAttachments = &colorAttachmentReference,
//         .pResolveAttachments = NULL,
//         .pDepthStencilAttachment = &depthAttachmentReference,
//         .preserveAttachmentCount = 0,
//         .pPreserveAttachments = NULL,
//     };

//     VkSubpassDependency subpassDependency = {
//         .srcSubpass = VK_SUBPASS_EXTERNAL,
//         .dstSubpass = 0,
//         .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
//         .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
//         .srcAccessMask = 0,
//         .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
//         .dependencyFlags = 0,
//     };

//     VkRenderPassCreateInfo renderPassCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .attachmentCount = 2,
//         .pAttachments = (VkAttachmentDescription[]){
//             colorAttachmentDescription,
//             depthAttachmentDescription,
//         },
//         .subpassCount = 1,
//         .pSubpasses = &subpassDescription,
//         .dependencyCount = 1,
//         .pDependencies = &subpassDependency,
//     };

//     result = vkCreateRenderPass(pGFXEngine->vkDevice, &renderPassCreateInfo, NULL, pVkRenderPass);
//     TryThrowVulkanError(result);
// }

// static void DestroyRenderPass(GFXEngine *pGFXEngine, VkRenderPass *pVkRenderPass)
// {
//     vkDestroyRenderPass(pGFXEngine->vkDevice, *pVkRenderPass, NULL);
// }

// static void CreateFramebuffers(GFXEngine *pGFXEngine)
// {
//     VkResult result = VK_SUCCESS;
//     pGFXEngine->vkFramebuffers = TickernelMalloc(sizeof(VkFramebuffer) * pGFXEngine->swapchainImageCount);
//     for (int32_t i = 0; i < pGFXEngine->swapchainImageCount; i++)
//     {
//         uint32_t attachmentCount = 2;
//         VkImageView attachments[] = {pGFXEngine->swapchainImageViews[i], pGFXEngine->depthImageView};
//         VkFramebufferCreateInfo framebufferCreateInfo = {
//             .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
//             .pNext = NULL,
//             .flags = 0,
//             .renderPass = pGFXEngine->vkRenderPass,
//             .attachmentCount = attachmentCount,
//             .pAttachments = attachments,
//             .width = pGFXEngine->width,
//             .height = pGFXEngine->height,
//             .layers = 1,
//         };
//         result = vkCreateFramebuffer(pGFXEngine->vkDevice, &framebufferCreateInfo, NULL, &pGFXEngine->vkFramebuffers[i]);
//         TRY_THROW_VULKAN_ERROR(result);
//     }
// }

// static void DestroyFramebuffers(GFXEngine *pGFXEngine)
// {
//     for (int32_t i = 0; i < pGFXEngine->swapchainImageCount; i++)
//     {
//         vkDestroyFramebuffer(pGFXEngine->vkDevice, pGFXEngine->vkFramebuffers[i], NULL);
//     }
//     TickernelFree(pGFXEngine->vkFramebuffers);
// }

static void CreateSemaphores(GFXEngine *pGFXEngine)
{
    VkResult result = VK_SUCCESS;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };
    VkDevice vkDevice = pGFXEngine->vkDevice;

    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    pGFXEngine->imageAvailableSemaphores = TKNMalloc(sizeof(VkSemaphore) * pGFXEngine->swapchainImageCount);
    pGFXEngine->renderFinishedSemaphores = TKNMalloc(sizeof(VkSemaphore) * pGFXEngine->swapchainImageCount);
    pGFXEngine->renderFinishedFences = TKNMalloc(sizeof(VkFence) * pGFXEngine->swapchainImageCount);
    for (uint32_t i = 0; i < pGFXEngine->swapchainImageCount; i++)
    {
        result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGFXEngine->imageAvailableSemaphores[i]);
        TryThrowVulkanError(result);
        result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGFXEngine->renderFinishedSemaphores[i]);
        TryThrowVulkanError(result);
        result = vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGFXEngine->renderFinishedFences[i]);
        TryThrowVulkanError(result);
    }
}

static void DestroySemaphores(GFXEngine *pGFXEngine)
{
    VkDevice vkDevice = pGFXEngine->vkDevice;
    for (uint32_t i = 0; i < pGFXEngine->swapchainImageCount; i++)
    {
        vkDestroySemaphore(vkDevice, pGFXEngine->imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(vkDevice, pGFXEngine->renderFinishedSemaphores[i], NULL);
        vkDestroyFence(vkDevice, pGFXEngine->renderFinishedFences[i], NULL);
    }
    TKNFree(pGFXEngine->imageAvailableSemaphores);
    TKNFree(pGFXEngine->renderFinishedSemaphores);
    TKNFree(pGFXEngine->renderFinishedFences);
}

static void RecreateSwapchain(GFXEngine *pGFXEngine)
{
    VkResult result = VK_SUCCESS;

    int width, height;
    glfwGetFramebufferSize(pGFXEngine->pGLFWWindow, &width, &height);
    while (0 == width || 0 == height)
    {
        glfwGetFramebufferSize(pGFXEngine->pGLFWWindow, &width, &height);
        glfwWaitEvents();
    }

    result = vkDeviceWaitIdle(pGFXEngine->vkDevice);
    TryThrowVulkanError(result);
    
    // Destroy all vkRenderPasses
    // Destroy all vkFrameBuffers
    // Reset all vkCommandBuffers
    DestroyDepthResources(pGFXEngine);
    DestroySwapchain(pGFXEngine);

    CreateSwapchain(pGFXEngine);
    CreateDepthResources(pGFXEngine);
    // Create all vkFrameBuffers
    // Create all vkRenderPasses
    // Record all vkCommandBuffers
}

static void CreateCommandPools(GFXEngine *pGFXEngine)
{

    pGFXEngine->vkCommandPools = TKNMalloc(sizeof(VkCommandBuffer) * pGFXEngine->queueFamilyPropertyCount);

    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = pGFXEngine->graphicQueueFamilyIndex,
    };
    VkResult result = vkCreateCommandPool(pGFXEngine->vkDevice, &vkCommandPoolCreateInfo, NULL, &pGFXEngine->vkCommandPools[pGFXEngine->graphicQueueFamilyIndex]);
    TryThrowVulkanError(result);
}

static void DestroyCommandPools(GFXEngine *pGFXEngine)
{
    for (size_t i = 0; i < pGFXEngine->queueFamilyPropertyCount; i++)
    {
        VkCommandPool vkCommandPool = pGFXEngine->vkCommandPools[i];
        if (vkCommandPool != NULL)
        {
            vkDestroyCommandPool(pGFXEngine->vkDevice, pGFXEngine->vkCommandPools[i], NULL);
        }
    }
    TKNFree(pGFXEngine->vkCommandPools);
}

static void WaitForGPU(GFXEngine *pGFXEngine)
{
    VkResult result = VK_SUCCESS;

    // Wait for gpu
    uint32_t frameIndex = pGFXEngine->frameIndex;
    result = vkWaitForFences(pGFXEngine->vkDevice, 1, &pGFXEngine->renderFinishedFences[frameIndex], VK_TRUE, UINT64_MAX);
    TryThrowVulkanError(result);
    result = vkResetFences(pGFXEngine->vkDevice, 1, &pGFXEngine->renderFinishedFences[frameIndex]);
    TryThrowVulkanError(result);
}

static void AcquireImage(GFXEngine *pGFXEngine)
{
    VkResult result = VK_SUCCESS;
    VkDevice vkDevice = pGFXEngine->vkDevice;
    // Acquire next image
    result = vkAcquireNextImageKHR(vkDevice, pGFXEngine->vkSwapchain, UINT64_MAX, pGFXEngine->imageAvailableSemaphores[pGFXEngine->frameIndex], VK_NULL_HANDLE, &pGFXEngine->acquiredImageIndex);
    if (VK_SUCCESS == result || VK_SUBOPTIMAL_KHR == result)
    {
        return;
    }
    else if (VK_ERROR_OUT_OF_DATE_KHR == result)
    {
        RecreateSwapchain(pGFXEngine);
        return;
    }
    else
    {
        TryThrowVulkanError(result);
    }
}

static void SubmitCommandBuffers(GFXEngine *pGFXEngine)
{
    VkResult result = VK_SUCCESS;

    VkDevice vkDevice = pGFXEngine->vkDevice;
    uint32_t frameIndex = pGFXEngine->frameIndex;
    // Submit workflow...
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGFXEngine->imageAvailableSemaphores[frameIndex]},
        .pWaitDstStageMask = (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        .commandBufferCount = pGFXEngine->vkCommandBufferCount,
        .pCommandBuffers = pGFXEngine->vkCommandBuffers,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = (VkSemaphore[]){pGFXEngine->renderFinishedSemaphores[frameIndex]},
    };
    result = vkQueueSubmit(pGFXEngine->vkGraphicQueue, 1, &submitInfo, pGFXEngine->renderFinishedFences[frameIndex]);
    TryThrowVulkanError(result);
}

static void Present(GFXEngine *pGFXEngine)
{
    VkResult result = VK_SUCCESS;

    uint32_t frameIndex = pGFXEngine->frameIndex;
    // Present
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGFXEngine->renderFinishedSemaphores[frameIndex]},
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]){pGFXEngine->vkSwapchain},
        .pImageIndices = &pGFXEngine->acquiredImageIndex,
        .pResults = NULL,
    };
    result = vkQueuePresentKHR(pGFXEngine->vkPresentQueue, &presentInfo);
    TryThrowVulkanError(result);
}

void StartGFXEngine(GFXEngine *pGFXEngine)
{
    pGFXEngine->vkCommandBuffers = TKNMalloc(sizeof(VkCommandBuffer) * pGFXEngine->maxCommandBufferListCount);
    CreateGLFWWindow(pGFXEngine);
    CreateVkInstance(pGFXEngine);
    CreateVKSurface(pGFXEngine);
    PickPhysicalDevice(pGFXEngine);
    CreateLogicalDevice(pGFXEngine);
    CreateSwapchain(pGFXEngine);
    CreateDepthResources(pGFXEngine);
    CreateSemaphores(pGFXEngine);
    CreateCommandPools(pGFXEngine);
    // CreateRenderPass(pGFXEngine);
    // CreateFramebuffers(pGFXEngine);
}

void UpdateGFXEngine(GFXEngine *pGFXEngine)
{
    pGFXEngine->frameIndex = pGFXEngine->frameCount % pGFXEngine->swapchainImageCount;

    WaitForGPU(pGFXEngine);
    AcquireImage(pGFXEngine);
    SubmitCommandBuffers(pGFXEngine);
    Present(pGFXEngine);

    pGFXEngine->frameCount++;
}

void EndGFXEngine(GFXEngine *pGFXEngine)
{
    // DestroyFramebuffers(pGFXEngine);
    // DestroyRenderPass(pGFXEngine);
    DestroyCommandPools(pGFXEngine);
    DestroySemaphores(pGFXEngine);
    DestroyDepthResources(pGFXEngine);
    DestroySwapchain(pGFXEngine);
    DestroyLogicalDevice(pGFXEngine);
    // Destroy vkPhysicsDevice
    DestroyVKSurface(pGFXEngine);
    DestroyVKInstance(pGFXEngine);
    DestroyGLFWWindow(pGFXEngine);
    TKNFree(pGFXEngine->vkCommandBuffers);
}