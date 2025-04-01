#import "AppView.h"

@implementation AppView

- (void)setupView {
    self.delegate = self;
    self.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
    self.preferredFramesPerSecond = 60;
    self.keyCodeStates = calloc(KEY_CODE_MAX_ENUM, sizeof(BOOL));
    self.pEngineBinding = [[EngineBinding alloc] init];
    self.pLuaBinding = [[LuaBinding alloc] init];
    NSString *resourcesPath = [[NSBundle mainBundle] resourcePath];
    
    [self.pEngineBinding setupEngine:self.drawableSize.width height:self.drawableSize.height assetPath:resourcesPath pView:(__bridge void *)self];
    [self.pLuaBinding setupLua:resourcesPath graphicContext:self.pEngineBinding.pTickernelEngine->pGraphicContext];
}

- (void)teardownView {
    [self.pLuaBinding teardownLua];
    [self.pEngineBinding teardownEngine];
    free(self.keyCodeStates);
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    [self.pLuaBinding updateLua:self.keyCodeStates keyCodesLength: KEY_CODE_MAX_ENUM];
    [self.pEngineBinding updateEngine:size.width height:size.height];
    [self resetKeyCodes];
}

- (void)drawInMTKView:(MTKView *)view {
    NSCAssert([NSThread isMainThread], @"Rendering must be on main thread!");
    [self.pLuaBinding updateLua:self.keyCodeStates keyCodesLength: KEY_CODE_MAX_ENUM];
    [self.pEngineBinding updateEngine:view.drawableSize.width height:view.drawableSize.height];
    [self resetKeyCodes];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)keyDown:(NSEvent *)event {
    //    NSLog(@"Key Down: %@", characters);
    [self updateKeyCode:event keyCodeState:KEY_CODE_STATE_DOWN];
}

- (void)keyUp:(NSEvent *)event {
    //    NSLog(@"Key Up: %@", characters);
    [self updateKeyCode:event keyCodeState:KEY_CODE_STATE_UP];
}

- (void)resetKeyCodes{
    for (uint32_t i = 0; i < KEY_CODE_MAX_ENUM; i++) {
        if (self.keyCodeStates[i] == KEY_CODE_STATE_UP) {
            self.keyCodeStates[i] = KEY_CODE_STATE_IDLE;
        }
    }
}

- (void)updateKeyCode:(NSEvent *)event keyCodeState:(KeyCodeState)keyCodeState {
    // Map NSEvent key codes to KeyCode enum values
    switch (event.keyCode) {
        case 0: self.keyCodeStates[KEY_CODE_A] = keyCodeState; break;
        case 1: self.keyCodeStates[KEY_CODE_S] = keyCodeState; break;
        case 2: self.keyCodeStates[KEY_CODE_D] = keyCodeState; break;
        case 3: self.keyCodeStates[KEY_CODE_F] = keyCodeState; break;
        case 4: self.keyCodeStates[KEY_CODE_H] = keyCodeState; break;
        case 5: self.keyCodeStates[KEY_CODE_G] = keyCodeState; break;
        case 6: self.keyCodeStates[KEY_CODE_Z] = keyCodeState; break;
        case 7: self.keyCodeStates[KEY_CODE_X] = keyCodeState; break;
        case 8: self.keyCodeStates[KEY_CODE_C] = keyCodeState; break;
        case 9: self.keyCodeStates[KEY_CODE_V] = keyCodeState; break;
        case 11: self.keyCodeStates[KEY_CODE_B] = keyCodeState; break;
        case 12: self.keyCodeStates[KEY_CODE_Q] = keyCodeState; break;
        case 13: self.keyCodeStates[KEY_CODE_W] = keyCodeState; break;
        case 14: self.keyCodeStates[KEY_CODE_E] = keyCodeState; break;
        case 15: self.keyCodeStates[KEY_CODE_R] = keyCodeState; break;
        case 16: self.keyCodeStates[KEY_CODE_Y] = keyCodeState; break;
        case 17: self.keyCodeStates[KEY_CODE_T] = keyCodeState; break;
        case 18: self.keyCodeStates[KEY_CODE_1] = keyCodeState; break;
        case 19: self.keyCodeStates[KEY_CODE_2] = keyCodeState; break;
        case 20: self.keyCodeStates[KEY_CODE_3] = keyCodeState; break;
        case 21: self.keyCodeStates[KEY_CODE_4] = keyCodeState; break;
        case 22: self.keyCodeStates[KEY_CODE_6] = keyCodeState; break;
        case 23: self.keyCodeStates[KEY_CODE_5] = keyCodeState; break;
        case 24: self.keyCodeStates[KEY_CODE_EQUAL] = keyCodeState; break;
        case 25: self.keyCodeStates[KEY_CODE_9] = keyCodeState; break;
        case 26: self.keyCodeStates[KEY_CODE_7] = keyCodeState; break;
        case 27: self.keyCodeStates[KEY_CODE_MINUS] = keyCodeState; break;
        case 28: self.keyCodeStates[KEY_CODE_8] = keyCodeState; break;
        case 29: self.keyCodeStates[KEY_CODE_0] = keyCodeState; break;
        case 30: self.keyCodeStates[KEY_CODE_RIGHT_BRACKET] = keyCodeState; break;
        case 31: self.keyCodeStates[KEY_CODE_O] = keyCodeState; break;
        case 32: self.keyCodeStates[KEY_CODE_U] = keyCodeState; break;
        case 33: self.keyCodeStates[KEY_CODE_LEFT_BRACKET] = keyCodeState; break;
        case 34: self.keyCodeStates[KEY_CODE_I] = keyCodeState; break;
        case 35: self.keyCodeStates[KEY_CODE_P] = keyCodeState; break;
        case 36: self.keyCodeStates[KEY_CODE_ENTER] = keyCodeState; break;
        case 37: self.keyCodeStates[KEY_CODE_L] = keyCodeState; break;
        case 38: self.keyCodeStates[KEY_CODE_J] = keyCodeState; break;
        case 39: self.keyCodeStates[KEY_CODE_APOSTROPHE] = keyCodeState; break;
        case 40: self.keyCodeStates[KEY_CODE_K] = keyCodeState; break;
        case 41: self.keyCodeStates[KEY_CODE_SEMICOLON] = keyCodeState; break;
        case 42: self.keyCodeStates[KEY_CODE_BACKSLASH] = keyCodeState; break;
        case 43: self.keyCodeStates[KEY_CODE_COMMA] = keyCodeState; break;
        case 44: self.keyCodeStates[KEY_CODE_SLASH] = keyCodeState; break;
        case 45: self.keyCodeStates[KEY_CODE_N] = keyCodeState; break;
        case 46: self.keyCodeStates[KEY_CODE_M] = keyCodeState; break;
        case 47: self.keyCodeStates[KEY_CODE_PERIOD] = keyCodeState; break;
        case 48: self.keyCodeStates[KEY_CODE_TAB] = keyCodeState; break;
        case 49: self.keyCodeStates[KEY_CODE_SPACE] = keyCodeState; break;
        case 50: self.keyCodeStates[KEY_CODE_GRAVE] = keyCodeState; break;
        case 51: self.keyCodeStates[KEY_CODE_BACKSPACE] = keyCodeState; break;
        case 53: self.keyCodeStates[KEY_CODE_ESCAPE] = keyCodeState; break;
        case 65: self.keyCodeStates[KEY_CODE_PERIOD] = keyCodeState; break;
        case 67: self.keyCodeStates[KEY_CODE_NUMPAD_MULTIPLY] = keyCodeState; break;
        case 69: self.keyCodeStates[KEY_CODE_NUMPAD_ADD] = keyCodeState; break;
            //        case 71: self.keyCodes[KEY_CODE_NUMPAD_CLEAR] = keyDown; break;
        case 75: self.keyCodeStates[KEY_CODE_NUMPAD_DIVIDE] = keyCodeState; break;
        case 76: self.keyCodeStates[KEY_CODE_NUMPAD_ENTER] = keyCodeState; break;
        case 78: self.keyCodeStates[KEY_CODE_NUMPAD_SUBTRACT] = keyCodeState; break;
            //        case 81: self.keyCodes[KEY_CODE_NUMPAD_EQUAL] = keyDown; break;
        case 82: self.keyCodeStates[KEY_CODE_NUMPAD_0] = keyCodeState; break;
        case 83: self.keyCodeStates[KEY_CODE_NUMPAD_1] = keyCodeState; break;
        case 84: self.keyCodeStates[KEY_CODE_NUMPAD_2] = keyCodeState; break;
        case 85: self.keyCodeStates[KEY_CODE_NUMPAD_3] = keyCodeState; break;
        case 86: self.keyCodeStates[KEY_CODE_NUMPAD_4] = keyCodeState; break;
        case 87: self.keyCodeStates[KEY_CODE_NUMPAD_5] = keyCodeState; break;
        case 88: self.keyCodeStates[KEY_CODE_NUMPAD_6] = keyCodeState; break;
        case 89: self.keyCodeStates[KEY_CODE_NUMPAD_7] = keyCodeState; break;
        case 91: self.keyCodeStates[KEY_CODE_NUMPAD_8] = keyCodeState; break;
        case 92: self.keyCodeStates[KEY_CODE_NUMPAD_9] = keyCodeState; break;
        case 96: self.keyCodeStates[KEY_CODE_F5] = keyCodeState; break;
        case 97: self.keyCodeStates[KEY_CODE_F6] = keyCodeState; break;
        case 98: self.keyCodeStates[KEY_CODE_F7] = keyCodeState; break;
        case 99: self.keyCodeStates[KEY_CODE_F3] = keyCodeState; break;
        case 100: self.keyCodeStates[KEY_CODE_F8] = keyCodeState; break;
        case 101: self.keyCodeStates[KEY_CODE_F9] = keyCodeState; break;
        case 103: self.keyCodeStates[KEY_CODE_F11] = keyCodeState; break;
            //        case 105: self.keyCodes[KEY_CODE_F13] = keyDown; break;
            //        case 106: self.keyCodes[KEY_CODE_F16] = keyDown; break;
            //        case 107: self.keyCodes[KEY_CODE_F14] = keyDown; break;
        case 109: self.keyCodeStates[KEY_CODE_F10] = keyCodeState; break;
        case 111: self.keyCodeStates[KEY_CODE_F12] = keyCodeState; break;
            //        case 113: self.keyCodes[KEY_CODE_F15] = keyDown; break;
            //        case 114: self.keyCodes[KEY_CODE_HELP] = keyDown; break;
        case 115: self.keyCodeStates[KEY_CODE_HOME] = keyCodeState; break;
        case 116: self.keyCodeStates[KEY_CODE_PAGE_UP] = keyCodeState; break;
        case 117: self.keyCodeStates[KEY_CODE_DELETE] = keyCodeState; break;
        case 118: self.keyCodeStates[KEY_CODE_F4] = keyCodeState; break;
        case 119: self.keyCodeStates[KEY_CODE_END] = keyCodeState; break;
        case 120: self.keyCodeStates[KEY_CODE_F2] = keyCodeState; break;
        case 121: self.keyCodeStates[KEY_CODE_PAGE_DOWN] = keyCodeState; break;
        case 122: self.keyCodeStates[KEY_CODE_F1] = keyCodeState; break;
        case 123: self.keyCodeStates[KEY_CODE_LEFT] = keyCodeState; break;
        case 124: self.keyCodeStates[KEY_CODE_RIGHT] = keyCodeState; break;
        case 125: self.keyCodeStates[KEY_CODE_DOWN] = keyCodeState; break;
        case 126: self.keyCodeStates[KEY_CODE_UP] = keyCodeState; break;
        default: break;
    }
}

- (instancetype)initWithFrame:(CGRect)frameRect device:(id<MTLDevice>)device {
    self = [super initWithFrame:frameRect device:device];
    if (self) {
        [self setupView];
    }
    return self;
}

- (void)dealloc {
    [self teardownView];
}

@end
