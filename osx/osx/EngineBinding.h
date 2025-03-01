#import <vulkan/vulkan.h>
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>
#import <vulkan/vulkan_macos.h>
#import "tickernelEngine.h"

static NSString * const AppName = @"Tickernel";

@interface EngineBinding : NSObject

@property (nonatomic, assign) VkSurfaceKHR vkSurface;
@property (nonatomic, assign) TickernelEngine* pTickernelEngine;
@property (nonatomic, assign) VkInstance vkInstance;

- (void)setupEngine: (uint32_t)width height:(uint32_t)height assetPath:(NSString*)assetPath pView:(void*)pView;
- (void)teardownEngine;
- (void)updateEngine: (uint32_t)width height:(uint32_t)height;

@end
