#include <Cocoa/Cocoa.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>
#include <tickernelWindow.h>
#include <QuartzCore/CAMetalLayer.h>

struct TickernelWindowStruct {
    NSView *nsView;
    bool shouldClose;
};

@interface TickernelWindowDelegate : NSObject <NSWindowDelegate>
@property (assign) TickernelWindow *pTickernelWindow;
@end

@implementation TickernelWindowDelegate

- (instancetype)initWithTickernelWindow:(TickernelWindow *)pTickernelWindow {
    self = [super init];
    if (self) {
        self.pTickernelWindow = pTickernelWindow;
    }
    return self;
}

- (BOOL)windowShouldClose:(id)sender {
    self.pTickernelWindow->shouldClose = true;
    return YES;
}

@end


void TickernelGetWindowExtensionCount(uint32_t * pCount) {
    *pCount = 2;
}

void TickernelGetWindowExtensions(char **windowExtensions) {
    windowExtensions[0] = "VK_KHR_surface";
    windowExtensions[1] = "VK_MVK_macos_surface";
}

VkResult CreateWindowVkSurface(TickernelWindow *pTickernelWindow, VkInstance vkInstance, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pVkSurface) {
    if (!pTickernelWindow || !pTickernelWindow->nsView) {
        fprintf(stderr, "Error: Invalid TickernelWindow or nsView is null\n");
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    CAMetalLayer *metalLayer = [CAMetalLayer layer];
    metalLayer.frame = pTickernelWindow->nsView.bounds;
    [pTickernelWindow->nsView setLayer:metalLayer];
    [pTickernelWindow->nsView setWantsLayer:YES];

    NSLog(@"Creating Vulkan Surface");
    VkMacOSSurfaceCreateInfoMVK createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    createInfo.pView = pTickernelWindow->nsView;

    VkResult result = vkCreateMacOSSurfaceMVK(vkInstance, &createInfo, pAllocator, pVkSurface);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan surface: %d\n", result);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    
    return result;
}

void TickernelGetWindowFramebufferSize(TickernelWindow *pTickernelWindow, uint32_t *pWidth, uint32_t *pHeight) {
    NSRect frame = [pTickernelWindow->nsView frame];
    *pWidth = frame.size.width;
    *pHeight = frame.size.height;
}

void TickernelWaitWindowEvent() {
    NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantFuture] inMode:NSDefaultRunLoopMode dequeue:YES];
    [NSApp sendEvent:event];
}

void TickernelPollWindowEvents() {
    while (true) {
        NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
        if (!event) {
            break;
        }
        [NSApp sendEvent:event];
    }
}

bool TickernelWindowShouldClose(TickernelWindow *pTickernelWindow) {
    return pTickernelWindow->shouldClose;
}

void TickernelDestroyWindow(TickernelWindow *pTickernelWindow) {
    if (pTickernelWindow->nsView) {
        NSWindow *window = [pTickernelWindow->nsView window];
        [window close];
        pTickernelWindow->nsView = nil;
    }
    free(pTickernelWindow);
}

void TickernelCreateWindow(uint32_t width, uint32_t height, const char *name,TickernelWindow **ppTickernelWindow) {
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        id menubar = [[NSMenu alloc] init];
        id appMenuItem = [[NSMenuItem alloc] init];
        [menubar addItem:appMenuItem];
        [NSApp setMainMenu:menubar];

        id appMenu = [[NSMenu alloc] init];
        id appName = [[NSProcessInfo processInfo] processName];
        id quitTitle = [@"Quit " stringByAppendingString:appName];
        id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];
        [appMenu addItem:quitMenuItem];
        [appMenuItem setSubmenu:appMenu];

        id window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, width, height)
                                                 styleMask:(NSWindowStyleMaskTitled |
                                                            NSWindowStyleMaskClosable |
                                                            NSWindowStyleMaskResizable)
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
        [window setTitle:@(name)];
        [window makeKeyAndOrderFront:nil];

        *ppTickernelWindow = (TickernelWindow *)malloc(sizeof(TickernelWindow));
        if(!(*ppTickernelWindow)){
            abort();
        }
        (*ppTickernelWindow)->nsView = [window contentView];
        (*ppTickernelWindow)->shouldClose = false;
        // Set the delegate to handle the window close event
        TickernelWindowDelegate *delegate = [[TickernelWindowDelegate alloc] initWithTickernelWindow:*ppTickernelWindow];
        [window setDelegate:delegate];

        [NSApp activateIgnoringOtherApps:YES];
    }
}
