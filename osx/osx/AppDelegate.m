#import "AppDelegate.h"

static NSString * const AppName = @"Tickernel";

@implementation AppDelegate

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    NSLog(@"[Vulkan Validation] %s", pCallbackData->pMessage);
    return VK_FALSE;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 1920, 1080)
                                              styleMask:(NSWindowStyleMaskTitled |
                                                         NSWindowStyleMaskResizable |
                                                         NSWindowStyleMaskClosable)
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    [self.window setTitle:AppName];
    [self.window makeKeyAndOrderFront:nil];

    if (![self createVulkanInstance]) {
        [self cleanupVulkanResources];
        return;
    }

    self.appView = [[AppView alloc] initWithFrame:self.window.contentView.bounds
                                           device:MTLCreateSystemDefaultDevice()
                                    vulkanInstance:_vkInstance];
    [self.window setContentView:self.appView];

    if (![self createVulkanSurface]) {
        [self cleanupVulkanResources];
        return;
    }

    NSString *resourcesPath = [[NSBundle mainBundle] resourcePath];
    _pTickernelEngine = TickernelStart([resourcesPath UTF8String], 2, VK_PRESENT_MODE_FIFO_KHR,
                                       _vkInstance, _vkSurface,
                                       self.appView.drawableSize.width,
                                       self.appView.drawableSize.height);
    if (_pTickernelEngine == NULL) {
        NSLog(@"Failed to start Tickernel Engine!");
        [self cleanupVulkanResources];
        return;
    } else {
        NSLog(@"Tickernel Engine started successfully!");
        self.appView.pTickernelEngine = _pTickernelEngine;
        self.appView.vkSurface = _vkSurface;
    }

    [self.window makeFirstResponder:self.appView];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    if (_pTickernelEngine != NULL) {
        TickernelEnd(_pTickernelEngine);
        _pTickernelEngine = NULL;
        NSLog(@"Tickernel Engine terminated!");
    }
    [self cleanupVulkanResources];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

- (BOOL)createVulkanInstance {
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
        return NO;
    }
    NSLog(@"Vulkan instance created successfully!");
    return YES;
}

- (BOOL)createVulkanSurface {
    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceCreateInfo.pView = (__bridge void*)self.appView;

    if (vkCreateMacOSSurfaceMVK(_vkInstance, &surfaceCreateInfo, NULL, &_vkSurface) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan surface!");
        return NO;
    }
    NSLog(@"Vulkan surface created successfully!");
    return YES;
}

- (void)cleanupVulkanResources {
    if (_vkSurface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_vkInstance, _vkSurface, NULL);
        _vkSurface = VK_NULL_HANDLE;
    }
    if (_vkInstance != VK_NULL_HANDLE) {
        vkDestroyInstance(_vkInstance, NULL);
        _vkInstance = VK_NULL_HANDLE;
    }
}

@end
