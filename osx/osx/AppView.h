#import "EngineBinding.h"

@interface AppView : MTKView <MTKViewDelegate>

@property (nonatomic, assign) KeyCodeState *keyCodeStates;
@property (nonatomic, strong) EngineBinding* pEngineBinding;

- (void)setupView;
- (void)teardownView;
- (void)updateKeyCode:(NSEvent *)event keyCodeState:(KeyCodeState)keyCodeState;

@end
