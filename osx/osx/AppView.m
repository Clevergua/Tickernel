#import "AppView.h"

@implementation AppView

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    [self.pEngineBinding updateEngine:size.width height:size.height];
}

- (void)drawInMTKView:(MTKView *)view {
    NSCAssert([NSThread isMainThread], @"Rendering must be on main thread!");
    [self.pEngineBinding updateEngine:view.drawableSize.width
                               height:view.drawableSize.height];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)keyDown:(NSEvent *)event {
    //    NSLog(@"Key Down: %@", characters);
    [self updateKeyCode:event keyPressed:YES];
}

- (void)keyUp:(NSEvent *)event {
    //    NSLog(@"Key Up: %@", characters);
    [self updateKeyCode:event keyPressed:NO];
}



- (void)updateKeyCode:(NSEvent *)event keyPressed:(BOOL)keyPressed {
    // Map NSEvent key codes to KeyCode enum values
    switch (event.keyCode) {
    case 0:
        self.keyCodeStates[KEY_CODE_A] = keyPressed;
        break;
    case 1:
        self.keyCodeStates[KEY_CODE_S] = keyPressed;
        break;
    case 2:
        self.keyCodeStates[KEY_CODE_D] = keyPressed;
        break;
    case 3:
        self.keyCodeStates[KEY_CODE_F] = keyPressed;
        break;
    case 4:
        self.keyCodeStates[KEY_CODE_H] = keyPressed;
        break;
    case 5:
        self.keyCodeStates[KEY_CODE_G] = keyPressed;
        break;
    case 6:
        self.keyCodeStates[KEY_CODE_Z] = keyPressed;
        break;
    case 7:
        self.keyCodeStates[KEY_CODE_X] = keyPressed;
        break;
    case 8:
        self.keyCodeStates[KEY_CODE_C] = keyPressed;
        break;
    case 9:
        self.keyCodeStates[KEY_CODE_V] = keyPressed;
        break;
    case 11:
        self.keyCodeStates[KEY_CODE_B] = keyPressed;
        break;
    case 12:
        self.keyCodeStates[KEY_CODE_Q] = keyPressed;
        break;
    case 13:
        self.keyCodeStates[KEY_CODE_W] = keyPressed;
        break;
    case 14:
        self.keyCodeStates[KEY_CODE_E] = keyPressed;
        break;
    case 15:
        self.keyCodeStates[KEY_CODE_R] = keyPressed;
        break;
    case 16:
        self.keyCodeStates[KEY_CODE_Y] = keyPressed;
        break;
    case 17:
        self.keyCodeStates[KEY_CODE_T] = keyPressed;
        break;
    case 18:
        self.keyCodeStates[KEY_CODE_1] = keyPressed;
        break;
    case 19:
        self.keyCodeStates[KEY_CODE_2] = keyPressed;
        break;
    case 20:
        self.keyCodeStates[KEY_CODE_3] = keyPressed;
        break;
    case 21:
        self.keyCodeStates[KEY_CODE_4] = keyPressed;
        break;
    case 22:
        self.keyCodeStates[KEY_CODE_6] = keyPressed;
        break;
    case 23:
        self.keyCodeStates[KEY_CODE_5] = keyPressed;
        break;
    case 24:
        self.keyCodeStates[KEY_CODE_EQUAL] = keyPressed;
        break;
    case 25:
        self.keyCodeStates[KEY_CODE_9] = keyPressed;
        break;
    case 26:
        self.keyCodeStates[KEY_CODE_7] = keyPressed;
        break;
    case 27:
        self.keyCodeStates[KEY_CODE_MINUS] = keyPressed;
        break;
    case 28:
        self.keyCodeStates[KEY_CODE_8] = keyPressed;
        break;
    case 29:
        self.keyCodeStates[KEY_CODE_0] = keyPressed;
        break;
    case 30:
        self.keyCodeStates[KEY_CODE_RIGHT_BRACKET] = keyPressed;
        break;
    case 31:
        self.keyCodeStates[KEY_CODE_O] = keyPressed;
        break;
    case 32:
        self.keyCodeStates[KEY_CODE_U] = keyPressed;
        break;
    case 33:
        self.keyCodeStates[KEY_CODE_LEFT_BRACKET] = keyPressed;
        break;
    case 34:
        self.keyCodeStates[KEY_CODE_I] = keyPressed;
        break;
    case 35:
        self.keyCodeStates[KEY_CODE_P] = keyPressed;
        break;
    case 36:
        self.keyCodeStates[KEY_CODE_ENTER] = keyPressed;
        break;
    case 37:
        self.keyCodeStates[KEY_CODE_L] = keyPressed;
        break;
    case 38:
        self.keyCodeStates[KEY_CODE_J] = keyPressed;
        break;
    case 39:
        self.keyCodeStates[KEY_CODE_APOSTROPHE] = keyPressed;
        break;
    case 40:
        self.keyCodeStates[KEY_CODE_K] = keyPressed;
        break;
    case 41:
        self.keyCodeStates[KEY_CODE_SEMICOLON] = keyPressed;
        break;
    case 42:
        self.keyCodeStates[KEY_CODE_BACKSLASH] = keyPressed;
        break;
    case 43:
        self.keyCodeStates[KEY_CODE_COMMA] = keyPressed;
        break;
    case 44:
        self.keyCodeStates[KEY_CODE_SLASH] = keyPressed;
        break;
    case 45:
        self.keyCodeStates[KEY_CODE_N] = keyPressed;
        break;
    case 46:
        self.keyCodeStates[KEY_CODE_M] = keyPressed;
        break;
    case 47:
        self.keyCodeStates[KEY_CODE_PERIOD] = keyPressed;
        break;
    case 48:
        self.keyCodeStates[KEY_CODE_TAB] = keyPressed;
        break;
    case 49:
        self.keyCodeStates[KEY_CODE_SPACE] = keyPressed;
        break;
    case 50:
        self.keyCodeStates[KEY_CODE_GRAVE] = keyPressed;
        break;
    case 51:
        self.keyCodeStates[KEY_CODE_BACKSPACE] = keyPressed;
        break;
    case 53:
        self.keyCodeStates[KEY_CODE_ESCAPE] = keyPressed;
        break;
    case 65:
        self.keyCodeStates[KEY_CODE_PERIOD] = keyPressed;
        break;
    case 67:
        self.keyCodeStates[KEY_CODE_NUMPAD_MULTIPLY] = keyPressed;
        break;
    case 69:
        self.keyCodeStates[KEY_CODE_NUMPAD_ADD] = keyPressed;
        break;
        //        case 71: self.keyCodes[KEY_CODE_NUMPAD_CLEAR] = keyDown;
        //        break;
    case 75:
        self.keyCodeStates[KEY_CODE_NUMPAD_DIVIDE] = keyPressed;
        break;
    case 76:
        self.keyCodeStates[KEY_CODE_NUMPAD_ENTER] = keyPressed;
        break;
    case 78:
        self.keyCodeStates[KEY_CODE_NUMPAD_SUBTRACT] = keyPressed;
        break;
        //        case 81: self.keyCodes[KEY_CODE_NUMPAD_EQUAL] = keyDown;
        //        break;
    case 82:
        self.keyCodeStates[KEY_CODE_NUMPAD_0] = keyPressed;
        break;
    case 83:
        self.keyCodeStates[KEY_CODE_NUMPAD_1] = keyPressed;
        break;
    case 84:
        self.keyCodeStates[KEY_CODE_NUMPAD_2] = keyPressed;
        break;
    case 85:
        self.keyCodeStates[KEY_CODE_NUMPAD_3] = keyPressed;
        break;
    case 86:
        self.keyCodeStates[KEY_CODE_NUMPAD_4] = keyPressed;
        break;
    case 87:
        self.keyCodeStates[KEY_CODE_NUMPAD_5] = keyPressed;
        break;
    case 88:
        self.keyCodeStates[KEY_CODE_NUMPAD_6] = keyPressed;
        break;
    case 89:
        self.keyCodeStates[KEY_CODE_NUMPAD_7] = keyPressed;
        break;
    case 91:
        self.keyCodeStates[KEY_CODE_NUMPAD_8] = keyPressed;
        break;
    case 92:
        self.keyCodeStates[KEY_CODE_NUMPAD_9] = keyPressed;
        break;
    case 96:
        self.keyCodeStates[KEY_CODE_F5] = keyPressed;
        break;
    case 97:
        self.keyCodeStates[KEY_CODE_F6] = keyPressed;
        break;
    case 98:
        self.keyCodeStates[KEY_CODE_F7] = keyPressed;
        break;
    case 99:
        self.keyCodeStates[KEY_CODE_F3] = keyPressed;
        break;
    case 100:
        self.keyCodeStates[KEY_CODE_F8] = keyPressed;
        break;
    case 101:
        self.keyCodeStates[KEY_CODE_F9] = keyPressed;
        break;
    case 103:
        self.keyCodeStates[KEY_CODE_F11] = keyPressed;
        break;
        //        case 105: self.keyCodes[KEY_CODE_F13] = keyDown; break;
        //        case 106: self.keyCodes[KEY_CODE_F16] = keyDown; break;
        //        case 107: self.keyCodes[KEY_CODE_F14] = keyDown; break;
    case 109:
        self.keyCodeStates[KEY_CODE_F10] = keyPressed;
        break;
    case 111:
        self.keyCodeStates[KEY_CODE_F12] = keyPressed;
        break;
        //        case 113: self.keyCodes[KEY_CODE_F15] = keyDown; break;
        //        case 114: self.keyCodes[KEY_CODE_HELP] = keyDown; break;
    case 115:
        self.keyCodeStates[KEY_CODE_HOME] = keyPressed;
        break;
    case 116:
        self.keyCodeStates[KEY_CODE_PAGE_UP] = keyPressed;
        break;
    case 117:
        self.keyCodeStates[KEY_CODE_DELETE] = keyPressed;
        break;
    case 118:
        self.keyCodeStates[KEY_CODE_F4] = keyPressed;
        break;
    case 119:
        self.keyCodeStates[KEY_CODE_END] = keyPressed;
        break;
    case 120:
        self.keyCodeStates[KEY_CODE_F2] = keyPressed;
        break;
    case 121:
        self.keyCodeStates[KEY_CODE_PAGE_DOWN] = keyPressed;
        break;
    case 122:
        self.keyCodeStates[KEY_CODE_F1] = keyPressed;
        break;
    case 123:
        self.keyCodeStates[KEY_CODE_LEFT] = keyPressed;
        break;
    case 124:
        self.keyCodeStates[KEY_CODE_RIGHT] = keyPressed;
        break;
    case 125:
        self.keyCodeStates[KEY_CODE_DOWN] = keyPressed;
        break;
    case 126:
        self.keyCodeStates[KEY_CODE_UP] = keyPressed;
        break;
    default:
        break;
    }
}

- (instancetype)initWithFrame:(CGRect)frameRect device:(id<MTLDevice>)device {
    self = [super initWithFrame:frameRect device:device];
    self.delegate = self;
    self.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
    self.preferredFramesPerSecond = 1;
    self.keyCodeStates = calloc(KEY_CODE_MAX_ENUM, sizeof(BOOL));
    self.pEngineBinding = [[EngineBinding alloc] init];

    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    [self.pEngineBinding setupEngine:self.drawableSize.width
                              height:self.drawableSize.height
                        resourcePath:resourcePath
                               pView:(__bridge void *)self];
    
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowWillClose:)
                                                 name:NSWindowWillCloseNotification
                                               object:nil];
    return self;
}

- (void)windowWillClose:(NSNotification *)notification {
    [self.pEngineBinding teardownEngine];
    free(self.keyCodeStates);
}

@end
