#import <Cocoa/Cocoa.h>
#import <vulkan/vulkan.h>
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>
#import <vulkan/vulkan_macos.h>
#import "tickernelEngine.h"
NS_ASSUME_NONNULL_BEGIN

@interface AppView : MTKView <MTKViewDelegate>
@property (nonatomic, assign) VkSurfaceKHR vkSurface;
@property (nonatomic, assign) void* pTickernelEngine;
@property (nonatomic, assign) VkInstance vkInstance;
@end

NS_ASSUME_NONNULL_END
