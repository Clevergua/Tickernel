#import "EngineBinding.h"

@interface AppView : MTKView <MTKViewDelegate>

@property (nonatomic, assign) KeyState *keyCodeStates;
@property (nonatomic, strong) EngineBinding* pEngineBinding;

- (void)updateKeyCode:(NSEvent *)event keyState:(KeyState)keyState;

@end
