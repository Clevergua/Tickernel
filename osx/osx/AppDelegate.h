#import <Cocoa/Cocoa.h>
#import <vulkan/vulkan.h>
#import <vulkan/vulkan_macos.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (assign, nonatomic) VkInstance vkInstance;
@property (assign, nonatomic) VkSurfaceKHR vkSurface;
@property (strong, nonatomic) NSWindow *window;
@end
