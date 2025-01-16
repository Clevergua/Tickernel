#include <Cocoa/Cocoa.h>
#import "tickernelEngine.h"
#import <vulkan/vulkan_macos.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (assign, nonatomic) TickernelEngine* pTickernelEngine;
@property (assign, nonatomic) VkInstance vkInstance;
@property (assign, nonatomic) VkSurfaceKHR vkSurface;
@property (strong, nonatomic) NSWindow *window;
@end
