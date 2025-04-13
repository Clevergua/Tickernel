#import "EngineBinding.h"
#import "LuaBinding.h"

@interface AppView : MTKView <MTKViewDelegate>

@property (nonatomic, assign) KeyCodeState *keyCodeStates;
@property (nonatomic, strong) LuaBinding* pLuaBinding;
@property (nonatomic, strong) EngineBinding* pEngineBinding;

- (void)setupView;
- (void)teardownView;
- (void)updateKeyCode:(NSEvent *)event keyCodeState:(KeyCodeState)keyCodeState;

@end
