#import "EngineBinding.h"

@implementation EngineBinding

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                                             VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) {
    NSLog(@"[Vulkan Validation] %s", pCallbackData->pMessage);
    return VK_FALSE;
}

- (void)createVkInstance {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = [AppName UTF8String];
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Tickernel Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    
#ifdef DEBUG
    const char* layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    createInfo.enabledLayerCount = sizeof(layers) / sizeof(layers[0]);
    createInfo.ppEnabledLayerNames = layers;
    
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    createInfo.pNext = &debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
    createInfo.pNext = NULL;
#endif
    
    const char* extensions[] = {
        "VK_KHR_surface",
        "VK_MVK_macos_surface",
        "VK_KHR_portability_enumeration",
#ifdef DEBUG
        "VK_EXT_debug_utils",
#endif
    };
    createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
    createInfo.ppEnabledExtensionNames = extensions;
    
    if (vkCreateInstance(&createInfo, NULL, &_vkInstance) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan instance!");
        exit(EXIT_FAILURE);
    }
    NSLog(@"Vulkan instance created successfully!");
}

- (void)createVkSurface: (const void *)pView  {
    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceCreateInfo.pView = pView;
    
    if (vkCreateMacOSSurfaceMVK(_vkInstance, &surfaceCreateInfo, NULL, &_vkSurface) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan surface!");
        exit(EXIT_FAILURE);
    }
    NSLog(@"Vulkan surface created successfully!");
}

- (void)destroyVkInstance {
    if (_vkInstance != VK_NULL_HANDLE) {
        vkDestroyInstance(_vkInstance, NULL);
        _vkInstance = VK_NULL_HANDLE;
    }
}

- (void)destroyVkSurface {
    if (_vkSurface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_vkInstance, _vkSurface, NULL);
        _vkSurface = VK_NULL_HANDLE;
    }
}

- (void)setupEngine: (uint32_t)width height:(uint32_t)height assetPath:(NSString*)assetPath pView:(void *) pView; {
    [self createVkInstance];
    [self createVkSurface :pView];
    self.pTickernelEngine = tickernelStart([assetPath UTF8String], 2, VK_PRESENT_MODE_FIFO_KHR,
                                           _vkInstance, _vkSurface, width, height);
}

- (void)teardownEngine {
    tickernelEnd(self.pTickernelEngine);
    [self destroyVkSurface];
    [self destroyVkInstance];
}

- (void)updateEngine: (uint32_t)width height:(uint32_t)height; {
    tickernelUpdate(self.pTickernelEngine, width, height);
}


@end
