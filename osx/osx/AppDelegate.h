#import <Cocoa/Cocoa.h>
#import <vulkan/vulkan.h>
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>
#import <vulkan/vulkan_macos.h>
#import "AppView.h"


@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign, nonatomic) TickernelEngine* pTickernelEngine;
@property (assign, nonatomic) VkInstance vkInstance;
@property (assign, nonatomic) VkSurfaceKHR vkSurface;
@property (strong, nonatomic) NSWindow *window;
@property (strong, nonatomic) AppView *appView;

@end
