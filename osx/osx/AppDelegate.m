#import "AppDelegate.h"

static NSString * const AppName = @"Tickernel";

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
    @autoreleasepool {
        self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 800, 600)
                                                  styleMask:(NSWindowStyleMaskTitled |
                                                             NSWindowStyleMaskResizable |
                                                             NSWindowStyleMaskClosable)
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO];
        [self.window setTitle:AppName];
        [self.window makeKeyAndOrderFront:nil];
        
        NSView *contentView = [[NSView alloc] initWithFrame:[self.window contentRectForFrameRect:self.window.frame]];
        [self.window setContentView:contentView];
        
        CAMetalLayer *metalLayer = [CAMetalLayer layer];
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metalLayer.framebufferOnly = YES;
        contentView.layer = metalLayer;
        contentView.wantsLayer = YES;
        
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
            "VK_EXT_debug_utils",
        };
        createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
        createInfo.ppEnabledExtensionNames = extensions;
        
        if (vkCreateInstance(&createInfo, NULL, &_vkInstance) != VK_SUCCESS) {
            NSLog(@"Failed to create Vulkan instance!");
            return;
        }
        NSLog(@"Vulkan instance created successfully with validation layers enabled!");
        
        VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        surfaceCreateInfo.pView = (__bridge void*)self.window.contentView.layer;
        
        if (vkCreateMacOSSurfaceMVK(_vkInstance, &surfaceCreateInfo, NULL, &_vkSurface) != VK_SUCCESS) {
            NSLog(@"Failed to create Vulkan surface!");
            return;
        }
        
        NSLog(@"Vulkan surface created successfully!");
        NSRect frame = self.window.frame;
        
        NSString *resourcesPath = [[NSBundle mainBundle] resourcePath];
        
        self.pTickernelEngine = TickernelStart([resourcesPath UTF8String], 2, VK_PRESENT_MODE_FIFO_KHR, _vkInstance, _vkSurface, frame.size.width, frame.size.height);
        if (self.pTickernelEngine == NULL) {
            NSLog(@"Failed to start Tickernel Engine!");
        } else {
            NSLog(@"Tickernel Engine started successfully!");
        }
        
        self.updateTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / 60)
                                                            target:self
                                                          selector:@selector(update)
                                                          userInfo:nil
                                                           repeats:YES];
    }
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
    
    [self.updateTimer invalidate];
    self.updateTimer = nil;
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

- (void)update {
    NSRect frame = self.window.frame;
    while (true)
    {
        NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
        if (!event && frame.size.width > 0 && frame.size.height > 0)
        {
            break;
        }
        [NSApp sendEvent:event];
    }
    if (self.pTickernelEngine != NULL)
    {    
        TickernelUpdate(self.pTickernelEngine, frame.size.width, frame.size.height);
    }
}

@end
