#include "tkn.h"
#include "lualib.h"
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct TknChar
{
    uint32_t unicode;
    uint32_t x, y;
    uint32_t width, height;
    int32_t bearingX, bearingY;
    uint32_t advance;
    struct TknChar *pNext;
} TknChar;

typedef struct TknFont
{
    FT_Face ftFace;
    uint32_t tknCharCapacity;
    uint32_t tknCharCount;
    TknChar **tknCharPtrs;

    char *atlasData;
    uint32_t maxAtlasLength; // Maximum texture dimension (width is fixed, height expands up to this)
    uint32_t atlasHeight;    // Current texture height (dynamically expandable)
    uint32_t penX, penY;
    uint32_t maxRowHeight;

    struct TknFont *pNext;
} TknFont;

typedef struct
{
    FT_Library ftLibrary;
    TknFont *pTknFont;
} TknFontLibrary;

TknFontLibrary *createTknFontLibraryPtr();
void destroyTknFontLibraryPtr(TknFontLibrary *pTknFontLibrary);

TknChar *loadTknChar(TknFont *pTknFont, uint32_t unicode);

TknFont *createTknFontPtr(TknFontLibrary *pTknFontLibrary, const char *fontPath, uint32_t fontSize, uint32_t initialCharCapacity, uint32_t maxAtlasLength);
void destroyTknFontPtr(TknFont *pTknFont);
