#import <Foundation/Foundation.h>
#import "lua.h"
#import "lualib.h"
#import "lauxlib.h"
#import "tickernelEngine.h"

@interface LuaBinding : NSObject

- (lua_State *)startLua:(NSString *)assetPath graphicContext:(GraphicContext *)graphicContext;
- (void)updateLua:(lua_State *)pLuaState keyCodes:(BOOL *)keyCodes keyCodesLength:(uint32_t)keyCodesLength;
- (void)endLua:(lua_State *)pLuaState;

@end
