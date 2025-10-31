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
    uint32_t u1, v1, u2, v2;
    struct TknChar *pNext;
} TknChar;

typedef struct TknFont
{
    FT_Face ftFace;
    uint32_t tknCharCapacity;
    uint32_t tknCharCount;
    TknChar *tknChars;

    char *atlasData;
    uint32_t atlasSize;

    struct TknFont *pNext;
} TknFont;

typedef struct
{
    FT_Library ftLibrary;
    TknFont *pTknFont;
} TknFontLibrary;

static void assertFTError(FT_Error error)
{
    tknAssert(error == 0, "FreeType error: %d", error);
}

TknFont *createTknFontPtr(TknFontLibrary *pTknFontLibrary, const char *fontPath, int fontSize, uint32_t initialCharCapacity, uint32_t initialAtlasSize)
{
    TknFont *pTknFont = tknMalloc(sizeof(TknFont));
    // Initialize the TknFont structure
    pTknFont->tknCharCapacity = initialCharCapacity;
    pTknFont->tknCharCount = 0;
    pTknFont->tknChars = tknMalloc(sizeof(TknChar) * initialCharCapacity);
    pTknFont->atlasSize = initialAtlasSize;
    pTknFont->atlasData = tknMalloc(initialAtlasSize * initialAtlasSize);
    memset(pTknFont->atlasData, 0, initialAtlasSize * initialAtlasSize);
    pTknFont->pNext = NULL;
    // Load the font using FreeType
    assertFTError(FT_New_Face(pTknFontLibrary->ftLibrary, fontPath, 0, &pTknFont->ftFace));
    assertFTError(FT_Set_Pixel_Sizes(pTknFont->ftFace, 0, fontSize));

    // Store the TknFont in the library
    pTknFont->pNext = pTknFontLibrary->pTknFont;
    pTknFontLibrary->pTknFont = pTknFont;
    return pTknFont;
}

void destroyTknFontPtr(TknFont *pTknFont)
{
    for (uint32_t i = 0; i < pTknFont->tknCharCapacity; i++)
    {
        TknChar *pCurrent = &pTknFont->tknChars[i];
        while (pCurrent)
        {
            TknChar *pNext = pCurrent->pNext;
            tknFree(pCurrent);
            pCurrent = pNext;
        }
    }

    tknFree(pTknFont->tknChars);
    tknFree(pTknFont->atlasData);
    tknFree(pTknFont);
}

TknFontLibrary *createTknFontLibraryPtr()
{
    TknFontLibrary *pLibrary = tknMalloc(sizeof(TknFontLibrary));
    if (pLibrary)
    {
        FT_Init_FreeType(&pLibrary->ftLibrary);
        pLibrary->pTknFont = NULL;
    }
    return pLibrary;
}

void destroyTknFontLibraryPtr(TknFontLibrary *pTknFontLibrary)
{
    TknFont *pCurrent = pTknFontLibrary->pTknFont;
    while (pCurrent)
    {
        TknFont *pNext = pCurrent->pNext;
        destroyTknFontPtr(pCurrent);
        pCurrent = pNext;
    }
    FT_Done_FreeType(pTknFontLibrary->ftLibrary);
    tknFree(pTknFontLibrary);
}
