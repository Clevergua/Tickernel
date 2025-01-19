
#import <Cocoa/Cocoa.h>
#import "tickernelEngine.h"
#import <vulkan/vulkan_macos.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (assign, nonatomic) TickernelEngine* pTickernelEngine;
@property (assign, nonatomic) VkInstance vkInstance;
@property (assign, nonatomic) VkSurfaceKHR vkSurface;
@property (strong, nonatomic) NSWindow *window;
@property (nonatomic, strong) NSTimer *updateTimer;

@end

@interface VulkanView : NSView
@end
