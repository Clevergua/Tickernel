#import "AppView.h"

@implementation AppView

- (instancetype)initWithFrame:(CGRect)frameRect
                       device:(id<MTLDevice>)device
               vulkanInstance:(VkInstance)vkInstance
{
    self = [super initWithFrame:frameRect device:device];
    if (self) {
        _vkInstance = vkInstance;
        [self setupView];
    }
    return self;
}

- (void)setupView {
    self.delegate = self;
    self.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
    self.preferredFramesPerSecond = 60;
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    if (self.pTickernelEngine) {
        TickernelUpdate(self.pTickernelEngine, (uint32_t)size.width, (uint32_t)size.height);
    }
}

- (void)drawInMTKView:(MTKView *)view {
    NSCAssert([NSThread isMainThread], @"Rendering must be on main thread!");
    if (self.pTickernelEngine) {
        TickernelUpdate(self.pTickernelEngine, (uint32_t)view.drawableSize.width, (uint32_t)view.drawableSize.height);
    }
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)keyDown:(NSEvent *)event {
}

- (void)keyUp:(NSEvent *)event {
}

- (void)dealloc {
    if (self.pTickernelEngine != NULL) {
        TickernelEnd(self.pTickernelEngine);
    }
    if (self.vkSurface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_vkInstance, self.vkSurface, NULL);

    }
}

@end
