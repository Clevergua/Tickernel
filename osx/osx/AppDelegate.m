#import "AppDelegate.h"

@interface AppDelegate ()
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // 创建窗口
    NSRect frame = NSMakeRect(0, 0, 800, 600);
    self.window = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:(NSWindowStyleMaskTitled |
                                                         NSWindowStyleMaskClosable |
                                                         NSWindowStyleMaskResizable)
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    [self.window setTitle:@"My Application Window"];
    [self.window makeKeyAndOrderFront:nil];

    // 初始化Vulkan实例
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (vkCreateInstance(&createInfo, NULL, &_vkInstance) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan instance!");
        return;
    }

    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceCreateInfo.pView = (__bridge void *)[self.window contentView];

    if (vkCreateMacOSSurfaceMVK(_vkInstance, &surfaceCreateInfo, NULL, &_vkSurface) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan surface!");
        vkDestroyInstance(_vkInstance, NULL);
        return;
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // 销毁Vulkan Surface和实例
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
