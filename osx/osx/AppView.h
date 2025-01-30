// AppView.h

#import <MetalKit/MetalKit.h>
#import <vulkan/vulkan.h>
#import "tickernelEngine.h"
NS_ASSUME_NONNULL_BEGIN

@interface AppView : MTKView <MTKViewDelegate>

- (instancetype)initWithFrame:(CGRect)frameRect
                       device:(id<MTLDevice>)device
               vulkanInstance:(VkInstance)vkInstance;

@property (nonatomic, assign) VkSurfaceKHR vkSurface;
@property (nonatomic, assign) void* pTickernelEngine;
@property (nonatomic, assign) VkInstance vkInstance;
@end

NS_ASSUME_NONNULL_END
