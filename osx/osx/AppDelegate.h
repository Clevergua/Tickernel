#import <Cocoa/Cocoa.h>
#import <vulkan/vulkan.h>
#import <vulkan/vulkan_macos.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong, nonatomic) NSWindow *window;
@property (assign, nonatomic) VkInstance vkInstance;
@property (assign, nonatomic) VkSurfaceKHR vkSurface;
@end
