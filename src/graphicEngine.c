#include <graphicEngine.h>

#define GETARRAYCOUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))

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
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    };
    if (pGraphicEngine->enableValidationLayers)
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

    char **extensionNames = TickernelMalloc(extensionCount * sizeof(char *));
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
            .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = enabledLayerCount,
            .ppEnabledLayerNames = (const char *const *)enabledLayerNames,
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = (const char *const *)extensionNames,
        };
    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, NULL, &pGraphicEngine->vkInstance);
    TickernelFree(extensionNames);
}
static void DestroyVKInstance(GraphicEngine *pGraphicEngine)
{
    vkDestroyInstance(pGraphicEngine->vkInstance, NULL);
}

static void CreateGLFWWindow(GraphicEngine *pGraphicEngine)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwInitHint(GLFW_RESIZABLE, GLFW_FALSE);
    pGraphicEngine->pGLFWWindow = glfwCreateWindow(pGraphicEngine->width, pGraphicEngine->height, pGraphicEngine->name, NULL, NULL);
}

static void DestroyGLFWWindow(GraphicEngine *pGraphicEngine)
{
    glfwDestroyWindow(pGraphicEngine->pGLFWWindow);
    glfwTerminate();
}

static void CreateVKSurface(GraphicEngine *pGraphicEngine)
{
    VkResult result = glfwCreateWindowSurface(pGraphicEngine->vkInstance, pGraphicEngine->pGLFWWindow, NULL, &pGraphicEngine->vkSurface);
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
    uint32_t requiredExtensionCount = GETARRAYCOUNT(requiredExtensionNames);

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

    pGraphicEngine->swapchainExtent = vkSurfaceCapabilities.currentExtent;
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
            .imageExtent = pGraphicEngine->swapchainExtent,
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

static void CreateSemaphores(GraphicEngine *pGraphicEngine)
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
    pGraphicEngine->imageAvailableSemaphores = TickernelMalloc(sizeof(VkSemaphore) * pGraphicEngine->swapchainImageCount);
    pGraphicEngine->renderFinishedSemaphores = TickernelMalloc(sizeof(VkSemaphore) * pGraphicEngine->swapchainImageCount);
    pGraphicEngine->renderFinishedFences = TickernelMalloc(sizeof(VkFence) * pGraphicEngine->swapchainImageCount);
    for (uint32_t i = 0; i < pGraphicEngine->swapchainImageCount; i++)
    {
        result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicEngine->imageAvailableSemaphores[i]);
        TryThrowVulkanError(result);
        result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &pGraphicEngine->renderFinishedSemaphores[i]);
        TryThrowVulkanError(result);
        result = vkCreateFence(vkDevice, &fenceCreateInfo, NULL, &pGraphicEngine->renderFinishedFences[i]);
        TryThrowVulkanError(result);
    }
}

static void DestroySemaphores(GraphicEngine *pGraphicEngine)
{
    VkDevice vkDevice = pGraphicEngine->vkDevice;
    for (uint32_t i = 0; i < pGraphicEngine->swapchainImageCount; i++)
    {
        vkDestroySemaphore(vkDevice, pGraphicEngine->imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(vkDevice, pGraphicEngine->renderFinishedSemaphores[i], NULL);
        vkDestroyFence(vkDevice, pGraphicEngine->renderFinishedFences[i], NULL);
    }
    TickernelFree(pGraphicEngine->imageAvailableSemaphores);
    TickernelFree(pGraphicEngine->renderFinishedSemaphores);
    TickernelFree(pGraphicEngine->renderFinishedFences);
}

// static void CreateDepthResources(GraphicEngine *pGraphicEngine)
// {
//     FindDepthFormat(pGraphicEngine, &pGraphicEngine->depthFormat);
//     CreateImage(pGraphicEngine, pGraphicEngine->swapchainExtent.width, pGraphicEngine->swapchainExtent.height, pGraphicEngine->depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pGraphicEngine->depthImage, &pGraphicEngine->depthImageMemory);
//     CreateImageView(pGraphicEngine, pGraphicEngine->depthImage, pGraphicEngine->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, &pGraphicEngine->depthImageView);
// }

// static void DestroyDepthResources(GraphicEngine *pGraphicEngine)
// {
//     VkDevice vkDevice = pGraphicEngine->vkDevice;
//     vkDestroyImageView(vkDevice, pGraphicEngine->albedoImageView, NULL);
//     vkDestroyImage(vkDevice, pGraphicEngine->albedoImage, NULL);
//     vkFreeMemory(vkDevice, pGraphicEngine->albedoImageMemory, NULL);
// }
// static void CreateAlbedoResources(GraphicEngine *pGraphicEngine)
// {
//     pGraphicEngine->albedoFormat = VK_FORMAT_R8G8B8A8_UNORM;
//     CreateImage(pGraphicEngine, pGraphicEngine->swapchainExtent.width, pGraphicEngine->swapchainExtent.height, pGraphicEngine->albedoFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pGraphicEngine->albedoImage, &pGraphicEngine->albedoImageMemory);
//     CreateImageView(pGraphicEngine, pGraphicEngine->albedoImage, pGraphicEngine->albedoFormat, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicEngine->albedoImageView);
// }

// static void DestroyAlbedoResources(GraphicEngine *pGraphicEngine)
// {
//     VkDevice vkDevice = pGraphicEngine->vkDevice;
//     vkDestroyImageView(vkDevice, pGraphicEngine->depthImageView, NULL);
//     vkDestroyImage(vkDevice, pGraphicEngine->depthImage, NULL);
//     vkFreeMemory(vkDevice, pGraphicEngine->depthImageMemory, NULL);
// }

static void RecreateResources(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;

    int width, height;
    glfwGetFramebufferSize(pGraphicEngine->pGLFWWindow, &width, &height);
    while (0 == width || 0 == height)
    {
        glfwGetFramebufferSize(pGraphicEngine->pGLFWWindow, &width, &height);
        glfwWaitEvents();
    }

    result = vkDeviceWaitIdle(pGraphicEngine->vkDevice);
    TryThrowVulkanError(result);

    DestroySwapchain(pGraphicEngine);
    CreateSwapchain(pGraphicEngine);

    // if (pGraphicEngine->depthReferenceCount > 0)
    // {
    //     DestroyDepthResources(pGraphicEngine);
    //     CreateDepthResources(pGraphicEngine);
    // }
    // else
    // {
    //     // continue;
    // }
    // if (pGraphicEngine->albedoReferenceCount > 0)
    // {
    //     DestroyAlbedoResources(pGraphicEngine);
    //     CreateAlbedoResources(pGraphicEngine);
    // }
    // else
    // {
    //     // continue;
    // }
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
    uint32_t frameIndex = pGraphicEngine->frameIndex;
    result = vkWaitForFences(pGraphicEngine->vkDevice, 1, &pGraphicEngine->renderFinishedFences[frameIndex], VK_TRUE, UINT64_MAX);
    TryThrowVulkanError(result);
    result = vkResetFences(pGraphicEngine->vkDevice, 1, &pGraphicEngine->renderFinishedFences[frameIndex]);
    TryThrowVulkanError(result);
}

static void AcquireImage(GraphicEngine *pGraphicEngine, bool *pHasRecreateSwapchain)
{
    VkResult result = VK_SUCCESS;
    VkDevice vkDevice = pGraphicEngine->vkDevice;
    // Acquire next image
    result = vkAcquireNextImageKHR(vkDevice, pGraphicEngine->vkSwapchain, UINT64_MAX, pGraphicEngine->imageAvailableSemaphores[pGraphicEngine->frameIndex], VK_NULL_HANDLE, &pGraphicEngine->acquiredImageIndex);
    if (VK_SUCCESS == result || VK_SUBOPTIMAL_KHR == result)
    {
        return;
    }
    else if (VK_ERROR_OUT_OF_DATE_KHR == result)
    {
        RecreateResources(pGraphicEngine);
        *pHasRecreateSwapchain = true;
        return;
    }
    else
    {
        TryThrowVulkanError(result);
    }
}

static void SubmitCommandBuffers(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;

    VkDevice vkDevice = pGraphicEngine->vkDevice;
    uint32_t frameIndex = pGraphicEngine->frameIndex;
    // Submit workflow...
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGraphicEngine->imageAvailableSemaphores[frameIndex]},
        .pWaitDstStageMask = (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        .commandBufferCount = 1,
        .pCommandBuffers = &pGraphicEngine->graphicVkCommandBuffers[frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = (VkSemaphore[]){pGraphicEngine->renderFinishedSemaphores[frameIndex]},
    };

    result = vkQueueSubmit(pGraphicEngine->vkGraphicQueue, 1, &submitInfo, pGraphicEngine->renderFinishedFences[frameIndex]);
    TryThrowVulkanError(result);
}

static void Present(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;
    uint32_t frameIndex = pGraphicEngine->frameIndex;
    // Present
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){pGraphicEngine->renderFinishedSemaphores[frameIndex]},
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]){pGraphicEngine->vkSwapchain},
        .pImageIndices = &pGraphicEngine->acquiredImageIndex,
        .pResults = NULL,
    };
    result = vkQueuePresentKHR(pGraphicEngine->vkPresentQueue, &presentInfo);
    TryThrowVulkanError(result);
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

static void DestroyVkCommandBuffers(GraphicEngine *pGraphicEngine)
{
    vkFreeCommandBuffers(pGraphicEngine->vkDevice, pGraphicEngine->graphicVkCommandPool, pGraphicEngine->swapchainImageCount, pGraphicEngine->graphicVkCommandBuffers);
    TickernelFree(pGraphicEngine->graphicVkCommandBuffers);
}

// void ReferenceDepth(GraphicEngine *pGraphicEngine)
// {
//     if (pGraphicEngine->depthReferenceCount > 0)
//     {
//         // continue;
//     }
//     else
//     {
//         CreateDepthResources(pGraphicEngine);
//     }
//     pGraphicEngine->depthReferenceCount++;
// }
// void DereferenceDepth(GraphicEngine *pGraphicEngine)
// {
//     pGraphicEngine->depthReferenceCount--;
//     if (pGraphicEngine->depthReferenceCount > 0)
//     {
//         // continue;
//     }
//     else
//     {
//         DestroyDepthResources(pGraphicEngine);
//     }
// }
// void ReferenceAlbedo(GraphicEngine *pGraphicEngine)
// {
//     if (pGraphicEngine->albedoReferenceCount > 0)
//     {
//         // continue;
//     }
//     else
//     {
//         CreateAlbedoResources(pGraphicEngine);
//     }
//     pGraphicEngine->albedoReferenceCount++;
// }
// void DereferenceAlbedo(GraphicEngine *pGraphicEngine)
// {
//     pGraphicEngine->albedoReferenceCount--;
//     if (pGraphicEngine->albedoReferenceCount > 0)
//     {
//         // continue;
//     }
//     else
//     {
//         DestroyAlbedoResources(pGraphicEngine);
//     }
// }
void StartGraphicEngine(GraphicEngine *pGraphicEngine)
{
    pGraphicEngine->frameCount = 0;
    CreateGLFWWindow(pGraphicEngine);
    CreateVkInstance(pGraphicEngine);
    CreateVKSurface(pGraphicEngine);
    PickPhysicalDevice(pGraphicEngine);
    CreateLogicalDevice(pGraphicEngine);
    CreateSwapchain(pGraphicEngine);
    CreateSemaphores(pGraphicEngine);
    CreateCommandPools(pGraphicEngine);
    CreateVkCommandBuffers(pGraphicEngine);

    VkExtent3D vkExtent3D = {
        .width = pGraphicEngine->swapchainExtent.width,
        .height = pGraphicEngine->swapchainExtent.height,
        .depth = 0,
    };
    VkFormat depthVkFormat;
    FindDepthFormat(pGraphicEngine, &depthVkFormat);
    CreateGraphicImage(pGraphicEngine, vkExtent3D, depthVkFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, &pGraphicEngine->depthGraphicImage);

    CreateGraphicImage(pGraphicEngine, vkExtent3D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, &pGraphicEngine->albedoGraphicImage);
}

void UpdateGraphicEngine(GraphicEngine *pGraphicEngine)
{
    pGraphicEngine->frameIndex = pGraphicEngine->frameCount % pGraphicEngine->swapchainImageCount;
    bool hasRecreateSwapchain = false;
    WaitForGPU(pGraphicEngine);
    AcquireImage(pGraphicEngine, &hasRecreateSwapchain);
    // RecordCommandBuffer(pGraphicEngine);
    // UpdateVkCommandBuffers(pGraphicEngine, hasRecreateSwapchain);
    SubmitCommandBuffers(pGraphicEngine);
    Present(pGraphicEngine);
    pGraphicEngine->frameCount++;
}

void EndGraphicEngine(GraphicEngine *pGraphicEngine)
{
    DestroyVkCommandBuffers(pGraphicEngine);
    DestroyCommandPools(pGraphicEngine);
    DestroySemaphores(pGraphicEngine);
    // DestroyDepthResources(pGraphicEngine);
    DestroySwapchain(pGraphicEngine);
    DestroyLogicalDevice(pGraphicEngine);
    // Destroy vkPhysicsDevice
    DestroyVKSurface(pGraphicEngine);
    DestroyVKInstance(pGraphicEngine);
    DestroyGLFWWindow(pGraphicEngine);
}