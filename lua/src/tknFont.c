#include "tknFont.h"

static void assertFTError(FT_Error error)
{
    tknAssert(error == 0, "FreeType error: %d", error);
}

TknChar *loadTknChar(TknFont *pTknFont, uint32_t unicode)
{
    uint32_t index = unicode % pTknFont->tknCharCapacity;
    TknChar *pTknChar = pTknFont->tknCharPtrs[index];
    while (pTknChar)
    {
        if (pTknChar->unicode == unicode)
        {
            return pTknChar;
        }
        pTknChar = pTknChar->pNext;
    }
    if (pTknChar == NULL)
    {
        // Load character using FreeType
        assertFTError(FT_Load_Char(pTknFont->ftFace, unicode, FT_LOAD_RENDER));
        FT_GlyphSlot glyph = pTknFont->ftFace->glyph;

        FT_Bitmap *ftBitmap = &pTknFont->ftFace->glyph->bitmap;
        if (pTknFont->penX + ftBitmap->width > pTknFont->maxAtlasLength)
        {
            pTknFont->penX = 0;
            pTknFont->penY += pTknFont->maxRowHeight;
            pTknFont->maxRowHeight = 0;
        }

        if (pTknFont->penY + ftBitmap->rows > pTknFont->atlasHeight)
        {
            // Attempt to expand atlas
            if (pTknFont->atlasHeight == pTknFont->maxAtlasLength)
            {
                tknWarning("Font atlas is full, cannot load character U+%04X.\n", unicode);
                return NULL;
            }
            else
            {
                uint32_t newHeight = pTknFont->atlasHeight * 2 > pTknFont->maxAtlasLength ? pTknFont->maxAtlasLength : pTknFont->atlasHeight * 2;
                char *newAtlasData = tknMalloc(sizeof(char) * pTknFont->maxAtlasLength * newHeight);
                memcpy(newAtlasData, pTknFont->atlasData, sizeof(char) * pTknFont->maxAtlasLength * pTknFont->atlasHeight);
                memset(newAtlasData + pTknFont->maxAtlasLength * pTknFont->atlasHeight, 0, sizeof(char) * pTknFont->maxAtlasLength * (newHeight - pTknFont->atlasHeight));
                tknFree(pTknFont->atlasData);
                pTknFont->atlasData = newAtlasData;
                pTknFont->atlasHeight = newHeight;
            }
        }

        for (uint32_t y = 0; y < ftBitmap->rows; y++)
        {
            for (uint32_t x = 0; x < ftBitmap->width; x++)
            {
                uint32_t atlasIndex = (pTknFont->penY + y) * pTknFont->maxAtlasLength + (pTknFont->penX + x);
                uint32_t bitmapIndex = y * ftBitmap->pitch + x;
                pTknFont->atlasData[atlasIndex] = ftBitmap->buffer[bitmapIndex];
            }
        }

        TknChar *pNewChar = tknMalloc(sizeof(TknChar));
        pNewChar->unicode = unicode;
        pNewChar->x = pTknFont->penX;
        pNewChar->y = pTknFont->penY;
        pNewChar->width = glyph->bitmap.width;
        pNewChar->height = glyph->bitmap.rows;
        pNewChar->bearingX = glyph->bitmap_left;
        pNewChar->bearingY = glyph->bitmap_top;
        pNewChar->advance = glyph->advance.x >> 6;

        pNewChar->pNext = pTknFont->tknCharPtrs[index];
        pTknFont->tknCharPtrs[index] = pNewChar;
        pTknFont->tknCharCount++;

        pTknFont->penX += glyph->bitmap.width + 1;
        if (glyph->bitmap.rows + 1 > pTknFont->maxRowHeight)
            pTknFont->maxRowHeight = glyph->bitmap.rows + 1;

        return pNewChar;
    }
    else
    {
        return pTknChar;
    }
}

TknFont *createTknFontPtr(TknFontLibrary *pTknFontLibrary, const char *fontPath, uint32_t fontSize, uint32_t initialCharCapacity, uint32_t maxAtlasLength)
{
    TknFont *pTknFont = tknMalloc(sizeof(TknFont));
    // Initialize the TknFont structure
    pTknFont->tknCharCapacity = initialCharCapacity;
    pTknFont->tknCharCount = 0;
    pTknFont->tknCharPtrs = tknMalloc(sizeof(TknChar *) * initialCharCapacity);
    memset(pTknFont->tknCharPtrs, 0, sizeof(TknChar *) * initialCharCapacity);
    pTknFont->maxAtlasLength = maxAtlasLength;
    pTknFont->atlasHeight = fontSize; // Initial height is fontSize
    pTknFont->atlasData = tknMalloc(sizeof(char) * maxAtlasLength * fontSize);
    memset(pTknFont->atlasData, 0, sizeof(char) * maxAtlasLength * fontSize);
    pTknFont->penX = 0;
    pTknFont->penY = 0;
    pTknFont->maxRowHeight = 0;
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
        TknChar *pCurrent = pTknFont->tknCharPtrs[i];
        while (pCurrent)
        {
            TknChar *pNext = pCurrent->pNext;
            tknFree(pCurrent);
            pCurrent = pNext;
        }
    }

    tknFree(pTknFont->tknCharPtrs);
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
