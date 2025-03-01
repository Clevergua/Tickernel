#import "EngineBinding.h"
#import "LuaBinding.h"

@interface AppView : MTKView <MTKViewDelegate>

@property (nonatomic, assign) _Bool *keyCodes;
@property (nonatomic, strong) LuaBinding* pLuaBinding;
@property (nonatomic, strong) EngineBinding* pEngineBinding;

- (void)setupView;
- (void)teardownView;
- (void)updateKeyCode:(NSEvent *)event keyDown:(BOOL)keyDown;

@end
