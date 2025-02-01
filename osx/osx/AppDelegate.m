#import "AppDelegate.h"

static NSString * const AppName = @"Tickernel";

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 1920, 1080)
                                              styleMask:(NSWindowStyleMaskTitled |
                                                         NSWindowStyleMaskResizable |
                                                         NSWindowStyleMaskClosable)
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    [self.window setTitle:AppName];
    [self.window makeKeyAndOrderFront:nil];
    self.appView = [[AppView alloc] initWithFrame:self.window.contentView.bounds
                                           device:MTLCreateSystemDefaultDevice()];
    [self.window setContentView:self.appView];
    [self.window makeFirstResponder:self.appView];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

@end
