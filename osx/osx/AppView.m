#import "AppView.h"

@implementation AppView
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                                             VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) {
    NSLog(@"[Vulkan Validation] %s", pCallbackData->pMessage);
    return VK_FALSE;
}

- (void)createVkInstance {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = [AppName UTF8String];
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Tickernel Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    
#ifdef DEBUG
    const char* layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    createInfo.enabledLayerCount = sizeof(layers) / sizeof(layers[0]);
    createInfo.ppEnabledLayerNames = layers;
    
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    createInfo.pNext = &debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
    createInfo.pNext = NULL;
#endif
    
    const char* extensions[] = {
        "VK_KHR_surface",
        "VK_MVK_macos_surface",
        "VK_KHR_portability_enumeration",
#ifdef DEBUG
        "VK_EXT_debug_utils",
#endif
    };
    createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
    createInfo.ppEnabledExtensionNames = extensions;
    
    if (vkCreateInstance(&createInfo, NULL, &_vkInstance) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan instance!");
        exit(EXIT_FAILURE);
    }
    NSLog(@"Vulkan instance created successfully!");
}

- (void)createVkSurface {
    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceCreateInfo.pView = (__bridge void*)self;
    
    if (vkCreateMacOSSurfaceMVK(_vkInstance, &surfaceCreateInfo, NULL, &_vkSurface) != VK_SUCCESS) {
        NSLog(@"Failed to create Vulkan surface!");
        exit(EXIT_FAILURE);
    }
    NSLog(@"Vulkan surface created successfully!");
}

- (void)destroyVkInstance {
    if (_vkInstance != VK_NULL_HANDLE) {
        vkDestroyInstance(_vkInstance, NULL);
        _vkInstance = VK_NULL_HANDLE;
    }
}

- (void)destroyVkSurface {
    if (_vkSurface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_vkInstance, _vkSurface, NULL);
        _vkSurface = VK_NULL_HANDLE;
    }
}

- (void)setupView {
    self.delegate = self;
    self.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
    self.preferredFramesPerSecond = 60;
    [self createVkInstance];
    [self createVkSurface];
    NSString *resourcesPath = [[NSBundle mainBundle] resourcePath];
    _pTickernelEngine = tickernelStart([resourcesPath UTF8String], 2, VK_PRESENT_MODE_FIFO_KHR,
                                       _vkInstance, _vkSurface,
                                       self.drawableSize.width,
                                       self.drawableSize.height);
    self.keyCodes = calloc(KEY_CODE_MAX_ENUM, sizeof(BOOL));
}

- (void)teardownView {
    free(self.keyCodes);
    tickernelEnd(self.pTickernelEngine);
    [self destroyVkSurface];
    [self destroyVkInstance];
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    if (self.pTickernelEngine) {
        tickernelUpdate(self.pTickernelEngine, (uint32_t)size.width, (uint32_t)size.height);
    }
}

- (void)drawInMTKView:(MTKView *)view {
    NSCAssert([NSThread isMainThread], @"Rendering must be on main thread!");
    if (self.pTickernelEngine) {
        tickernelUpdate(self.pTickernelEngine, (uint32_t)view.drawableSize.width, (uint32_t)view.drawableSize.height);
    }
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)keyDown:(NSEvent *)event {
//    NSLog(@"Key Down: %@", characters);
    [self updateKeyCode:event keyDown:YES];
}

- (void)keyUp:(NSEvent *)event {
//    NSLog(@"Key Up: %@", characters);
    [self updateKeyCode:event keyDown:NO];
}

- (void)updateKeyCode:(NSEvent *)event keyDown:(BOOL)keyDown {
    // Map NSEvent key codes to KeyCode enum values
    switch (event.keyCode) {
        case 0: self.keyCodes[KEY_CODE_A] = keyDown; break;
        case 1: self.keyCodes[KEY_CODE_S] = keyDown; break;
        case 2: self.keyCodes[KEY_CODE_D] = keyDown; break;
        case 3: self.keyCodes[KEY_CODE_F] = keyDown; break;
        case 4: self.keyCodes[KEY_CODE_H] = keyDown; break;
        case 5: self.keyCodes[KEY_CODE_G] = keyDown; break;
        case 6: self.keyCodes[KEY_CODE_Z] = keyDown; break;
        case 7: self.keyCodes[KEY_CODE_X] = keyDown; break;
        case 8: self.keyCodes[KEY_CODE_C] = keyDown; break;
        case 9: self.keyCodes[KEY_CODE_V] = keyDown; break;
        case 11: self.keyCodes[KEY_CODE_B] = keyDown; break;
        case 12: self.keyCodes[KEY_CODE_Q] = keyDown; break;
        case 13: self.keyCodes[KEY_CODE_W] = keyDown; break;
        case 14: self.keyCodes[KEY_CODE_E] = keyDown; break;
        case 15: self.keyCodes[KEY_CODE_R] = keyDown; break;
        case 16: self.keyCodes[KEY_CODE_Y] = keyDown; break;
        case 17: self.keyCodes[KEY_CODE_T] = keyDown; break;
        case 18: self.keyCodes[KEY_CODE_1] = keyDown; break;
        case 19: self.keyCodes[KEY_CODE_2] = keyDown; break;
        case 20: self.keyCodes[KEY_CODE_3] = keyDown; break;
        case 21: self.keyCodes[KEY_CODE_4] = keyDown; break;
        case 22: self.keyCodes[KEY_CODE_6] = keyDown; break;
        case 23: self.keyCodes[KEY_CODE_5] = keyDown; break;
        case 24: self.keyCodes[KEY_CODE_EQUAL] = keyDown; break;
        case 25: self.keyCodes[KEY_CODE_9] = keyDown; break;
        case 26: self.keyCodes[KEY_CODE_7] = keyDown; break;
        case 27: self.keyCodes[KEY_CODE_MINUS] = keyDown; break;
        case 28: self.keyCodes[KEY_CODE_8] = keyDown; break;
        case 29: self.keyCodes[KEY_CODE_0] = keyDown; break;
        case 30: self.keyCodes[KEY_CODE_RIGHT_BRACKET] = keyDown; break;
        case 31: self.keyCodes[KEY_CODE_O] = keyDown; break;
        case 32: self.keyCodes[KEY_CODE_U] = keyDown; break;
        case 33: self.keyCodes[KEY_CODE_LEFT_BRACKET] = keyDown; break;
        case 34: self.keyCodes[KEY_CODE_I] = keyDown; break;
        case 35: self.keyCodes[KEY_CODE_P] = keyDown; break;
        case 36: self.keyCodes[KEY_CODE_ENTER] = keyDown; break;
        case 37: self.keyCodes[KEY_CODE_L] = keyDown; break;
        case 38: self.keyCodes[KEY_CODE_J] = keyDown; break;
        case 39: self.keyCodes[KEY_CODE_APOSTROPHE] = keyDown; break;
        case 40: self.keyCodes[KEY_CODE_K] = keyDown; break;
        case 41: self.keyCodes[KEY_CODE_SEMICOLON] = keyDown; break;
        case 42: self.keyCodes[KEY_CODE_BACKSLASH] = keyDown; break;
        case 43: self.keyCodes[KEY_CODE_COMMA] = keyDown; break;
        case 44: self.keyCodes[KEY_CODE_SLASH] = keyDown; break;
        case 45: self.keyCodes[KEY_CODE_N] = keyDown; break;
        case 46: self.keyCodes[KEY_CODE_M] = keyDown; break;
        case 47: self.keyCodes[KEY_CODE_PERIOD] = keyDown; break;
        case 48: self.keyCodes[KEY_CODE_TAB] = keyDown; break;
        case 49: self.keyCodes[KEY_CODE_SPACE] = keyDown; break;
        case 50: self.keyCodes[KEY_CODE_GRAVE] = keyDown; break;
        case 51: self.keyCodes[KEY_CODE_BACKSPACE] = keyDown; break;
        case 53: self.keyCodes[KEY_CODE_ESCAPE] = keyDown; break;
        case 65: self.keyCodes[KEY_CODE_PERIOD] = keyDown; break;
        case 67: self.keyCodes[KEY_CODE_NUMPAD_MULTIPLY] = keyDown; break;
        case 69: self.keyCodes[KEY_CODE_NUMPAD_ADD] = keyDown; break;
//        case 71: self.keyCodes[KEY_CODE_NUMPAD_CLEAR] = keyDown; break;
        case 75: self.keyCodes[KEY_CODE_NUMPAD_DIVIDE] = keyDown; break;
        case 76: self.keyCodes[KEY_CODE_NUMPAD_ENTER] = keyDown; break;
        case 78: self.keyCodes[KEY_CODE_NUMPAD_SUBTRACT] = keyDown; break;
//        case 81: self.keyCodes[KEY_CODE_NUMPAD_EQUAL] = keyDown; break;
        case 82: self.keyCodes[KEY_CODE_NUMPAD_0] = keyDown; break;
        case 83: self.keyCodes[KEY_CODE_NUMPAD_1] = keyDown; break;
        case 84: self.keyCodes[KEY_CODE_NUMPAD_2] = keyDown; break;
        case 85: self.keyCodes[KEY_CODE_NUMPAD_3] = keyDown; break;
        case 86: self.keyCodes[KEY_CODE_NUMPAD_4] = keyDown; break;
        case 87: self.keyCodes[KEY_CODE_NUMPAD_5] = keyDown; break;
        case 88: self.keyCodes[KEY_CODE_NUMPAD_6] = keyDown; break;
        case 89: self.keyCodes[KEY_CODE_NUMPAD_7] = keyDown; break;
        case 91: self.keyCodes[KEY_CODE_NUMPAD_8] = keyDown; break;
        case 92: self.keyCodes[KEY_CODE_NUMPAD_9] = keyDown; break;
        case 96: self.keyCodes[KEY_CODE_F5] = keyDown; break;
        case 97: self.keyCodes[KEY_CODE_F6] = keyDown; break;
        case 98: self.keyCodes[KEY_CODE_F7] = keyDown; break;
        case 99: self.keyCodes[KEY_CODE_F3] = keyDown; break;
        case 100: self.keyCodes[KEY_CODE_F8] = keyDown; break;
        case 101: self.keyCodes[KEY_CODE_F9] = keyDown; break;
        case 103: self.keyCodes[KEY_CODE_F11] = keyDown; break;
//        case 105: self.keyCodes[KEY_CODE_F13] = keyDown; break;
//        case 106: self.keyCodes[KEY_CODE_F16] = keyDown; break;
//        case 107: self.keyCodes[KEY_CODE_F14] = keyDown; break;
        case 109: self.keyCodes[KEY_CODE_F10] = keyDown; break;
        case 111: self.keyCodes[KEY_CODE_F12] = keyDown; break;
//        case 113: self.keyCodes[KEY_CODE_F15] = keyDown; break;
//        case 114: self.keyCodes[KEY_CODE_HELP] = keyDown; break;
        case 115: self.keyCodes[KEY_CODE_HOME] = keyDown; break;
        case 116: self.keyCodes[KEY_CODE_PAGE_UP] = keyDown; break;
        case 117: self.keyCodes[KEY_CODE_DELETE] = keyDown; break;
        case 118: self.keyCodes[KEY_CODE_F4] = keyDown; break;
        case 119: self.keyCodes[KEY_CODE_END] = keyDown; break;
        case 120: self.keyCodes[KEY_CODE_F2] = keyDown; break;
        case 121: self.keyCodes[KEY_CODE_PAGE_DOWN] = keyDown; break;
        case 122: self.keyCodes[KEY_CODE_F1] = keyDown; break;
        case 123: self.keyCodes[KEY_CODE_LEFT] = keyDown; break;
        case 124: self.keyCodes[KEY_CODE_RIGHT] = keyDown; break;
        case 125: self.keyCodes[KEY_CODE_DOWN] = keyDown; break;
        case 126: self.keyCodes[KEY_CODE_UP] = keyDown; break;
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
