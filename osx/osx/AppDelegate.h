#import <Cocoa/Cocoa.h>
#import "tickernelEngine.h"
#import <vulkan/vulkan_macos.h>
#import <MetalKit/MetalKit.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, MTKViewDelegate>
@property (assign, nonatomic) TickernelEngine* pTickernelEngine;
@property (assign, nonatomic) VkInstance vkInstance;
@property (assign, nonatomic) VkSurfaceKHR vkSurface;
@property (strong, nonatomic) NSWindow *window;
@property (strong, nonatomic) MTKView *mtkView;
@end
