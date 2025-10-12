#import "EngineBinding.h"

@interface AppView : MTKView <MTKViewDelegate>

@property (nonatomic, assign) BOOL *keyCodeStates;
@property (nonatomic, strong) EngineBinding* pEngineBinding;

- (void)updateKeyCode:(NSEvent *)event keyPressed:(BOOL)keyPressed;

@end
