#include <gfxDevice.h>

#define GETARRAYCOUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))

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

static void CreateVkInstance(GFXDevice *pGFXDevice)
{
    FILE *logStream = pGFXDevice->logStream;
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
    if (pGFXDevice->enableValidationLayers)
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
                .pUserData = logStream,
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

    char **extensionNames = malloc(extensionCount * sizeof(char *));
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
            .pApplicationName = pGFXDevice->name,
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
    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, NULL, &pGFXDevice->vkInstance);
    free(extensionNames);
}
static void DestroyVKInstance(GFXDevice *pGFXDevice)
{
    vkDestroyInstance(pGFXDevice->vkInstance, NULL);
}

static void CreateGLFWWindow(GFXDevice *pGFXDevice)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwInitHint(GLFW_RESIZABLE, GLFW_FALSE);
    pGFXDevice->pGLFWWindow = glfwCreateWindow(pGFXDevice->width, pGFXDevice->height, pGFXDevice->name, NULL, NULL);
}

static void DestroyGLFWWindow(GFXDevice *pGFXDevice)
{
    glfwDestroyWindow(pGFXDevice->pGLFWWindow);
    glfwTerminate();
}

static void CreateVKSurface(GFXDevice *pGFXDevice)
{
    VkResult result = glfwCreateWindowSurface(pGFXDevice->vkInstance, pGFXDevice->pGLFWWindow, NULL, &pGFXDevice->vkSurface);
    TRY_THROW_VULKAN_ERROR(result);
}

static void DestroyVKSurface(GFXDevice *pGFXDevice)
{
    vkDestroySurfaceKHR(pGFXDevice->vkInstance, pGFXDevice->vkSurface, NULL);
}

static void HasAllRequiredExtensions(GFXDevice *pGFXDevice, VkPhysicalDevice vkPhysicalDevice, bool *pHasAllRequiredExtensions)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    char *requiredExtensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    uint32_t requiredExtensionCount = GETARRAYCOUNT(requiredExtensionNames);

    uint32_t extensionCount = 0;
    result = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, NULL);
    TRY_THROW_VULKAN_ERROR(result);
    VkExtensionProperties *extensionProperties = malloc(extensionCount * sizeof(VkExtensionProperties));
    result = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &extensionCount, extensionProperties);
    TRY_THROW_VULKAN_ERROR(result);

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
    free(extensionProperties);
}

static void HasGraphicsAndPresentQueueFamilies(GFXDevice *pGFXDevice, VkPhysicalDevice vkPhysicalDevice, bool *pHasGraphicsAndPresentQueueFamilies, uint32_t *pGraphicsQueueFamilyIndex, uint32_t *pPresentQueueFamilyIndex)
{
    FILE *logStream = pGFXDevice->logStream;
    VkSurfaceKHR vkSurface = pGFXDevice->vkSurface;
    uint32_t *pQueueFamilyPropertyCount = &pGFXDevice->queueFamilyPropertyCount;

    VkResult result = VK_SUCCESS;

    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, pQueueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *vkQueueFamilyPropertiesList = malloc(*pQueueFamilyPropertyCount * sizeof(VkQueueFamilyProperties));
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
        TRY_THROW_VULKAN_ERROR(result);
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
    free(vkQueueFamilyPropertiesList);
}

static void PickPhysicalDevice(GFXDevice *pGFXDevice)
{
    FILE *logStream = pGFXDevice->logStream;
    VkResult result = VK_SUCCESS;
    uint32_t deviceCount = -1;
    result = vkEnumeratePhysicalDevices(pGFXDevice->vkInstance, &deviceCount, NULL);
    TRY_THROW_VULKAN_ERROR(result);

    if (deviceCount <= 0)
    {
        printf("failed to find GPUs with Vulkan support!");
    }
    else
    {
        VkPhysicalDevice *devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
        result = vkEnumeratePhysicalDevices(pGFXDevice->vkInstance, &deviceCount, devices);
        TRY_THROW_VULKAN_ERROR(result);

        uint32_t graphicQueueFamilyIndex = -1;
        uint32_t presentQueueFamilyIndex = -1;
        VkPhysicalDevice targetDevice = NULL;
        uint32_t maxScore = 0;
        char *targetDeviceName;

        VkSurfaceKHR vkSurface = pGFXDevice->vkSurface;
        for (uint32_t i = 0; i < deviceCount; i++)
        {
            VkPhysicalDevice vkPhysicalDevice = devices[i];

            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

            bool hasAllRequiredExtensions;
            HasAllRequiredExtensions(pGFXDevice, vkPhysicalDevice, &hasAllRequiredExtensions);
            TRY_THROW_VULKAN_ERROR(result);
            bool hasGraphicsAndPresentQueueFamilies;
            HasGraphicsAndPresentQueueFamilies(pGFXDevice, vkPhysicalDevice, &hasGraphicsAndPresentQueueFamilies, &graphicQueueFamilyIndex, &presentQueueFamilyIndex);

            uint32_t surfaceFormatCount;
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, NULL);
            TRY_THROW_VULKAN_ERROR(result);
            bool hasSurfaceFormat = surfaceFormatCount > 0;
            uint32_t presentModeCount;
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, NULL);
            TRY_THROW_VULKAN_ERROR(result);
            bool hasPresentMode = presentModeCount > 0;
            if (hasAllRequiredExtensions && hasGraphicsAndPresentQueueFamilies && hasSurfaceFormat && hasPresentMode)
            {
                uint32_t formatCount = 0;
                uint32_t modeCount = 0;
                result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &formatCount, NULL);
                TRY_THROW_VULKAN_ERROR(result);
                result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &modeCount, NULL);
                TRY_THROW_VULKAN_ERROR(result);

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
        free(devices);
        if (NULL == targetDevice)
        {
            printf("failed to find GPUs with Vulkan support!");
        }
        else
        {
            pGFXDevice->vkPhysicalDevice = targetDevice;
            pGFXDevice->graphicQueueFamilyIndex = graphicQueueFamilyIndex;
            pGFXDevice->presentQueueFamilyIndex = presentQueueFamilyIndex;
            printf("Selected target physical device named %s\n", targetDeviceName);
        }
    }
}

static void CreateLogicalDevice(GFXDevice *pGFXDevice)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    VkPhysicalDevice vkPhysicalDevice = pGFXDevice->vkPhysicalDevice;
    uint32_t graphicQueueFamilyIndex = pGFXDevice->graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGFXDevice->presentQueueFamilyIndex;
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo *queueCreateInfos;
    uint32_t queueCount;
    if (graphicQueueFamilyIndex == presentQueueFamilyIndex)
    {
        queueCount = 1;
        queueCreateInfos = malloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
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
        queueCreateInfos = malloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
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

    if (pGFXDevice->enableValidationLayers)
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
    result = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, NULL, &pGFXDevice->vkDevice);
    TRY_THROW_VULKAN_ERROR(result);
    vkGetDeviceQueue(pGFXDevice->vkDevice, graphicQueueFamilyIndex, 0, &pGFXDevice->vkGraphicQueue);
    vkGetDeviceQueue(pGFXDevice->vkDevice, presentQueueFamilyIndex, 0, &pGFXDevice->vkPresentQueue);
    free(queueCreateInfos);
}

static void DestroyLogicalDevice(GFXDevice *pGFXDevice)
{
    vkDestroyDevice(pGFXDevice->vkDevice, NULL);
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

static void CreateImageView(GFXDevice *pGFXDevice, VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkImageView *pImageView)
{
    FILE *logStream = pGFXDevice->logStream;
    VkDevice vkDevice = pGFXDevice->vkDevice;
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
    TRY_THROW_VULKAN_ERROR(result);
}

static void CreateSwapchain(GFXDevice *pGFXDevice)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    VkPhysicalDevice vkPhysicalDevice = pGFXDevice->vkPhysicalDevice;
    VkSurfaceKHR vkSurface = pGFXDevice->vkSurface;
    VkDevice vkDevice = pGFXDevice->vkDevice;
    uint32_t graphicQueueFamilyIndex = pGFXDevice->graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex = pGFXDevice->presentQueueFamilyIndex;

    VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
    uint32_t supportSurfaceFormatCount;
    uint32_t supportPresentModeCount;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurface, &vkSurfaceCapabilities);
    TRY_THROW_VULKAN_ERROR(result);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &supportSurfaceFormatCount, NULL);
    TRY_THROW_VULKAN_ERROR(result);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &supportPresentModeCount, NULL);
    TRY_THROW_VULKAN_ERROR(result);
    VkSurfaceFormatKHR *supportSurfaceFormats = malloc(supportSurfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    VkPresentModeKHR *supportPresentModes = malloc(supportPresentModeCount * sizeof(VkPresentModeKHR));

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &supportSurfaceFormatCount, supportSurfaceFormats);
    TRY_THROW_VULKAN_ERROR(result);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &supportPresentModeCount, supportPresentModes);
    TRY_THROW_VULKAN_ERROR(result);

    VkPresentModeKHR presentMode;
    ChooseSurfaceFormat(supportSurfaceFormats, supportSurfaceFormatCount, &pGFXDevice->surfaceFormat);
    ChoosePresentMode(supportPresentModes, supportPresentModeCount, pGFXDevice->targetPresentMode, &presentMode);
    uint32_t swapchainImageCount = pGFXDevice->targetSwapchainImageCount;
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

    pGFXDevice->swapchainExtent = vkSurfaceCapabilities.currentExtent;
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
            .imageFormat = pGFXDevice->surfaceFormat.format,
            .imageColorSpace = pGFXDevice->surfaceFormat.colorSpace,
            .imageExtent = pGFXDevice->swapchainExtent,
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

    result = vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, NULL, &pGFXDevice->vkSwapchain);
    TRY_THROW_VULKAN_ERROR(result);

    free(supportSurfaceFormats);
    free(supportPresentModes);

    result = vkGetSwapchainImagesKHR(vkDevice, pGFXDevice->vkSwapchain, &pGFXDevice->swapchainImageCount, NULL);
    TRY_THROW_VULKAN_ERROR(result);
    pGFXDevice->swapchainImages = malloc(swapchainImageCount * sizeof(VkImage));
    result = vkGetSwapchainImagesKHR(vkDevice, pGFXDevice->vkSwapchain, &pGFXDevice->swapchainImageCount, pGFXDevice->swapchainImages);
    TRY_THROW_VULKAN_ERROR(result);
    pGFXDevice->swapchainImageViews = malloc(swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        CreateImageView(pGFXDevice, pGFXDevice->swapchainImages[i], pGFXDevice->surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &pGFXDevice->swapchainImageViews[i]);
    }

    if (pGFXDevice->targetWaitFrameCount > pGFXDevice->swapchainImageCount)
    {
        pGFXDevice->waitFrameCount = pGFXDevice->swapchainImageCount;
    }
    else if (pGFXDevice->targetWaitFrameCount < 1)
    {
        pGFXDevice->waitFrameCount = 1;
    }
    else
    {
        pGFXDevice->waitFrameCount = pGFXDevice->targetWaitFrameCount;
    }
}
static void DestroySwapchain(GFXDevice *pGFXDevice)
{
    for (uint32_t i = 0; i < pGFXDevice->swapchainImageCount; i++)
    {
        vkDestroyImageView(pGFXDevice->vkDevice, pGFXDevice->swapchainImageViews[i], NULL);
    }
    free(pGFXDevice->swapchainImageViews);
    free(pGFXDevice->swapchainImages);
    vkDestroySwapchainKHR(pGFXDevice->vkDevice, pGFXDevice->vkSwapchain, NULL);
}

static void FindSupportedFormat(GFXDevice *pGFXDevice, VkFormat *candidates, uint32_t candidatesCount, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat *vkFormat)
{
    VkResult result = VK_SUCCESS;
    VkPhysicalDevice vkPhysicalDevice = pGFXDevice->vkPhysicalDevice;
    FILE *logStream = pGFXDevice->logStream;
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

static void FindDepthFormat(GFXDevice *pGFXDevice, VkFormat *pDepthFormat)
{
    uint32_t candidatesCount = 3;
    VkFormat *candidates = (VkFormat[]){VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    FindSupportedFormat(pGFXDevice, candidates, candidatesCount, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, pDepthFormat);
}

void FindMemoryType(GFXDevice *pGFXDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t *memoryTypeIndex)
{
    FILE *logStream = pGFXDevice->logStream;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(pGFXDevice->vkPhysicalDevice, &physicalDeviceMemoryProperties);
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

static void CreateImage(GFXDevice *pGFXDevice, int32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *pImage, VkDeviceMemory *pImageMemory)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    VkDevice vkDevice = pGFXDevice->vkDevice;
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
    TRY_THROW_VULKAN_ERROR(result);
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, *pImage, &memoryRequirements);
    uint32_t memoryTypeIndex;
    FindMemoryType(pGFXDevice, memoryRequirements.memoryTypeBits, properties, &memoryTypeIndex);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };

    result = vkAllocateMemory(vkDevice, &memoryAllocateInfo, NULL, pImageMemory);
    TRY_THROW_VULKAN_ERROR(result);
    result = vkBindImageMemory(vkDevice, *pImage, *pImageMemory, 0);
    TRY_THROW_VULKAN_ERROR(result);
}

static void CreateDepthResources(GFXDevice *pGFXDevice)
{
    FindDepthFormat(pGFXDevice, &pGFXDevice->depthFormat);
    CreateImage(pGFXDevice, pGFXDevice->swapchainExtent.width, pGFXDevice->swapchainExtent.height, pGFXDevice->depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pGFXDevice->depthImage, &pGFXDevice->depthImageMemory);

    CreateImageView(pGFXDevice, pGFXDevice->depthImage, pGFXDevice->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &pGFXDevice->depthImageView);
}
static void DestroyDepthResources(GFXDevice *pGFXDevice)
{
    VkDevice vkDevice = pGFXDevice->vkDevice;
    vkDestroyImageView(vkDevice, pGFXDevice->depthImageView, NULL);
    vkDestroyImage(vkDevice, pGFXDevice->depthImage, NULL);
    vkFreeMemory(vkDevice, pGFXDevice->depthImageMemory, NULL);
}

// static void CreateRenderPass(GFXDevice *pGFXDevice)
// {
//     VkResult result = VK_SUCCESS;
//     VkAttachmentDescription colorAttachmentDescription = {
//         .flags = 0,
//         .format = pGFXDevice->surfaceFormat.format,
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
//         .format = pGFXDevice->depthFormat,
//         .samples = VK_SAMPLE_COUNT_1_BIT,
//         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
//         .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
//         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
//         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
//         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//         .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
//     };
//     VkSubpassDependency subpassDependency = {
//         .srcSubpass = VK_SUBPASS_EXTERNAL,
//         .dstSubpass = 0,
//         .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
//         .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
//         .srcAccessMask = 0,
//         .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
//         .dependencyFlags = 0,
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

//     VkRenderPassCreateInfo renderPassCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .attachmentCount = 2,
//         .pAttachments = (VkAttachmentDescription[]){colorAttachmentDescription, depthAttachmentDescription},
//         .subpassCount = 1,
//         .pSubpasses = &subpassDescription,
//         .dependencyCount = 1,
//         .pDependencies = &subpassDependency,
//     };

//     result = vkCreateRenderPass(pGFXDevice->vkDevice, &renderPassCreateInfo, NULL, &pGFXDevice->vkRenderPass);
//     TRY_THROW_VULKAN_ERROR(pGFXDevice->logStream, result)
// }

// static void DestroyRenderPass(GFXDevice *pGFXDevice)
// {
//     vkDestroyRenderPass(pGFXDevice->vkDevice, pGFXDevice->vkRenderPass, NULL);
// }

// static void CreateFramebuffers(GFXDevice *pGFXDevice)
// {
//     VkResult result = VK_SUCCESS;
//     pGFXDevice->vkFramebuffers = malloc(sizeof(VkFramebuffer) * pGFXDevice->swapchainImageCount);
//     for (int32_t i = 0; i < pGFXDevice->swapchainImageCount; i++)
//     {
//         uint32_t attachmentCount = 2;
//         VkImageView attachments[] = {pGFXDevice->swapchainImageViews[i], pGFXDevice->depthImageView};
//         VkFramebufferCreateInfo framebufferCreateInfo = {
//             .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
//             .pNext = NULL,
//             .flags = 0,
//             .renderPass = pGFXDevice->vkRenderPass,
//             .attachmentCount = attachmentCount,
//             .pAttachments = attachments,
//             .width = pGFXDevice->width,
//             .height = pGFXDevice->height,
//             .layers = 1,
//         };
//         result = vkCreateFramebuffer(pGFXDevice->vkDevice, &framebufferCreateInfo, NULL, &pGFXDevice->vkFramebuffers[i]);
//         TRY_THROW_VULKAN_ERROR(pGFXDevice->logStream, result);
//     }
// }

// static void DestroyFramebuffers(GFXDevice *pGFXDevice)
// {
//     for (int32_t i = 0; i < pGFXDevice->swapchainImageCount; i++)
//     {
//         vkDestroyFramebuffer(pGFXDevice->vkDevice, pGFXDevice->vkFramebuffers[i], NULL);
//     }
//     free(pGFXDevice->vkFramebuffers);
// }

static void CreateSemaphores(GFXDevice *pGFXDevice)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };
    VkDevice vkDevice = pGFXDevice->vkDevice;

    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    pGFXDevice->imageAvailableSemaphores = malloc(sizeof(VkSemaphore) * pGFXDevice->waitFrameCount);
    pGFXDevice->renderFinishedSemaphores = malloc(sizeof(VkSemaphore) * pGFXDevice->waitFrameCount);
    pGFXDevice->renderFinishedFences = malloc(sizeof(VkFence) * pGFXDevice->waitFrameCount);
    for (uint32_t i = 0; i < pGFXDevice->waitFrameCount; i++)
    {
        result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGFXDevice->imageAvailableSemaphores[i]);
        TRY_THROW_VULKAN_ERROR(result);
        result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGFXDevice->renderFinishedSemaphores[i]);
        TRY_THROW_VULKAN_ERROR(result);
        result = vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGFXDevice->renderFinishedFences[i]);
        TRY_THROW_VULKAN_ERROR(result);
    }
}

static void DestroySemaphores(GFXDevice *pGFXDevice)
{
    VkDevice vkDevice = pGFXDevice->vkDevice;
    for (uint32_t i = 0; i < pGFXDevice->waitFrameCount; i++)
    {
        vkDestroySemaphore(vkDevice, pGFXDevice->imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(vkDevice, pGFXDevice->renderFinishedSemaphores[i], NULL);
        vkDestroyFence(vkDevice, pGFXDevice->renderFinishedFences[i], NULL);
    }
    free(pGFXDevice->imageAvailableSemaphores);
    free(pGFXDevice->renderFinishedSemaphores);
    free(pGFXDevice->renderFinishedFences);
}

static void RecreateSwapchain(GFXDevice *pGFXDevice)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    int width, height;
    glfwGetFramebufferSize(pGFXDevice->pGLFWWindow, &width, &height);
    while (0 == width || 0 == height)
    {
        glfwGetFramebufferSize(pGFXDevice->pGLFWWindow, &width, &height);
        glfwWaitEvents();
    }

    result = vkDeviceWaitIdle(pGFXDevice->vkDevice);
    TRY_THROW_VULKAN_ERROR(result);

    DestroyDepthResources(pGFXDevice);
    DestroySwapchain(pGFXDevice);

    CreateSwapchain(pGFXDevice);
    CreateDepthResources(pGFXDevice);

    pGFXDevice->hasRecreateSwapchain = true;
}

static void CreateCommandPools(GFXDevice *pGFXDevice)
{
    FILE *logStream = pGFXDevice->logStream;
    pGFXDevice->vkCommandPools = malloc(sizeof(VkCommandBuffer) * pGFXDevice->queueFamilyPropertyCount);

    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = pGFXDevice->graphicQueueFamilyIndex,
    };
    VkResult result = vkCreateCommandPool(pGFXDevice->vkDevice, &vkCommandPoolCreateInfo, NULL, &pGFXDevice->vkCommandPools[pGFXDevice->graphicQueueFamilyIndex]);
    TRY_THROW_VULKAN_ERROR(result);
}

static void DestroyCommandPools(GFXDevice *pGFXDevice)
{
    for (size_t i = 0; i < pGFXDevice->queueFamilyPropertyCount; i++)
    {
        VkCommandPool vkCommandPool = pGFXDevice->vkCommandPools[i];
        if (vkCommandPool != NULL)
        {
            vkDestroyCommandPool(pGFXDevice->vkDevice, pGFXDevice->vkCommandPools[i], NULL);
        }
    }
    free(pGFXDevice->vkCommandPools);
}

static void WaitForGPU(GFXDevice *pGFXDevice)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    // Wait for gpu
    uint32_t frameIndex = pGFXDevice->frameIndex;
    result = vkWaitForFences(pGFXDevice->vkDevice, 1, &pGFXDevice->renderFinishedFences[frameIndex], VK_TRUE, UINT64_MAX);
    TRY_THROW_VULKAN_ERROR(result);
    result = vkResetFences(pGFXDevice->vkDevice, 1, &pGFXDevice->renderFinishedFences[frameIndex]);
    TRY_THROW_VULKAN_ERROR(result);
}

static void AcquireImage(GFXDevice *pGFXDevice)
{
    FILE *logStream = pGFXDevice->logStream;
    VkResult result = VK_SUCCESS;
    VkDevice vkDevice = pGFXDevice->vkDevice;
    uint32_t frameIndex = pGFXDevice->frameIndex;
    // Acquire next image
    result = vkAcquireNextImageKHR(vkDevice, pGFXDevice->vkSwapchain, UINT64_MAX, pGFXDevice->imageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &pGFXDevice->acquiredImageIndex);
    if (VK_SUCCESS == result || VK_SUBOPTIMAL_KHR == result)
    {
        return;
    }
    else if (VK_ERROR_OUT_OF_DATE_KHR == result)
    {
        RecreateSwapchain(pGFXDevice);
        return;
    }
    else
    {
        printf("Try return engine gfx error because of vulkan error code: %d\n", result);
    }
}

static void SubmitCommandBuffers(GFXDevice *pGFXDevice)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    VkDevice vkDevice = pGFXDevice->vkDevice;
    uint32_t frameIndex = pGFXDevice->frameIndex;
    // Submit workflow...
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGFXDevice->imageAvailableSemaphores[frameIndex]},
        .pWaitDstStageMask = (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        .commandBufferCount = pGFXDevice->commandBufferCount,
        .pCommandBuffers = pGFXDevice->commandBufferList,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = (VkSemaphore[]){pGFXDevice->renderFinishedSemaphores[frameIndex]},
    };
    result = vkQueueSubmit(pGFXDevice->vkGraphicQueue, 1, &submitInfo, pGFXDevice->renderFinishedFences[frameIndex]);
    TRY_THROW_VULKAN_ERROR(result);
}

static void Present(GFXDevice *pGFXDevice)
{
    VkResult result = VK_SUCCESS;
    FILE *logStream = pGFXDevice->logStream;
    uint32_t frameIndex = pGFXDevice->frameIndex;
    // Present
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGFXDevice->renderFinishedSemaphores[frameIndex]},
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]){pGFXDevice->vkSwapchain},
        .pImageIndices = &pGFXDevice->acquiredImageIndex,
        .pResults = NULL,
    };
    result = vkQueuePresentKHR(pGFXDevice->vkPresentQueue, &presentInfo);
    TRY_THROW_VULKAN_ERROR(result);
}

void StartGFXDevice(GFXDevice *pGFXDevice)
{
    pGFXDevice->commandBufferList = malloc(sizeof(VkCommandBuffer) * pGFXDevice->maxCommandBufferListCount);
    CreateGLFWWindow(pGFXDevice);
    CreateVkInstance(pGFXDevice);
    CreateVKSurface(pGFXDevice);
    PickPhysicalDevice(pGFXDevice);
    CreateLogicalDevice(pGFXDevice);
    CreateSwapchain(pGFXDevice);
    CreateDepthResources(pGFXDevice);
    // CreateRenderPass(pGFXDevice);
    // CreateFramebuffers(pGFXDevice);
    CreateSemaphores(pGFXDevice);
    CreateCommandPools(pGFXDevice);
}

void UpdateGFXDevice(GFXDevice *pGFXDevice, bool *pShouldQuit)
{
    pGFXDevice->frameIndex = pGFXDevice->frameCount % pGFXDevice->waitFrameCount;

    WaitForGPU(pGFXDevice);
    AcquireImage(pGFXDevice);
    SubmitCommandBuffers(pGFXDevice);
    Present(pGFXDevice);

    pGFXDevice->frameCount++;
}

void EndGFXDevice(GFXDevice *pGFXDevice)
{
    DestroyCommandPools(pGFXDevice);
    DestroySemaphores(pGFXDevice);
    // DestroyFramebuffers(pGFXDevice);
    // DestroyRenderPass(pGFXDevice);
    DestroyDepthResources(pGFXDevice);
    DestroySwapchain(pGFXDevice);
    DestroyLogicalDevice(pGFXDevice);
    // Destroy vkPhysicsDevice
    DestroyVKSurface(pGFXDevice);
    DestroyVKInstance(pGFXDevice);
    DestroyGLFWWindow(pGFXDevice);
    free(pGFXDevice->commandBufferList);
}