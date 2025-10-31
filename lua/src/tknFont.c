#include "tknLuaBinding.h"
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct
{
    uint32_t unicode;
} CharInfo;

typedef struct
{
    CharInfo *unicodeToCharInfoPtr;
    uint32_t charCount;
} FontAtlas;

void createFontAtlas(uint32_t width, uint32_t height)
{
}