#import "AppDelegate.h"

@interface AppDelegate ()
@end

@implementation AppDelegate

// Function to handle Vulkan debug messages
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    NSLog(@"validation layer: %s", pCallbackData->pMessage);
    return VK_FALSE;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Tickernel";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Tickernel Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#ifdef DEBUG
    // Enable validation layers in debug mode
    const char* layers[] = {
        "VK_LAYER_KHRONOS_validation"
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
    // Clean up Vulkan surface and instance
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

@end
