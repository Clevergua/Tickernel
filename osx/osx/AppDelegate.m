#import "AppDelegate.h"

@implementation AppDelegate

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                                             VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) {
    NSLog(@"validation layer msg: %s", pCallbackData->pMessage);
    return VK_FALSE;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Tickernel";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Tickernel Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
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
    NSLog(@"VK_LAYER_PATH is set to: %s", getenv("VK_LAYER_PATH"));
    NSLog(@"VK_ICD_FILENAMES is set to: %s", getenv("VK_ICD_FILENAMES"));
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    
    if (layerCount == 0) {
        NSLog(@"No available Vulkan layers found.");
    } else {
        VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
        
        NSLog(@"Available Vulkan layers:");
        for (uint32_t i = 0; i < layerCount; i++) {
            NSLog(@"  %s", availableLayers[i].layerName);
        }
        
        free(availableLayers);
    }
    
    const char* extensions[] = {
        "VK_KHR_surface",
        "VK_MVK_macos_surface"
    };
    createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
    createInfo.ppEnabledExtensionNames = extensions;
    
    if (vkCreateInstance(&createInfo, NULL, &_vkInstance) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan instance!");
        return;
    }
    
    NSLog(@"Vulkan instance created successfully with validation layers enabled!");
    [self createSurface];
    
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    NSRect frame = self.window.frame;
    self.pTickernelEngine = TickernelStart([resourcePath UTF8String], 2, VK_PRESENT_MODE_FIFO_KHR, _vkInstance, _vkSurface, frame.size.width, frame.size.height);
    if (self.pTickernelEngine == NULL) {
        NSLog(@"Failed to start Tickernel Engine!");
    } else {
        NSLog(@"Tickernel Engine started successfully!");
    }
}

- (void)createSurface {
    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.pView = (__bridge void*)self.window.contentView;
    
    if (vkCreateMacOSSurfaceMVK(_vkInstance, &surfaceCreateInfo, NULL, &_vkSurface) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan surface!");
        return;
    }
    
    NSLog(@"Vulkan surface created successfully!");
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    if (self.pTickernelEngine != NULL) {
        TickernelEnd(self.pTickernelEngine);
        NSLog(@"Tickernel Engine terminated!");
    }
    if (_vkSurface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_vkInstance, _vkSurface, NULL);
    }
    if (_vkInstance != VK_NULL_HANDLE) {
        vkDestroyInstance(_vkInstance, NULL);
    }
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

- (void)update {
    if (self.pTickernelEngine != NULL) {
        NSRect frame = self.window.frame;
        TickernelUpdate(self.pTickernelEngine, frame.size.width, frame.size.height);
    }
}

@end
