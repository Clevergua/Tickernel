// Simple FreeType Hello World Example
// Compilation: gcc -lfreetype -o hello hello.c

#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>
#include <stdlib.h>

int main()
{
    FT_Library library;
    FT_Face face;
    FT_Error error;

    // Initialize FreeType library
    error = FT_Init_FreeType(&library);
    if (error)
    {
        fprintf(stderr, "Failed to initialize FreeType\n");
        return 1;
    }
    // Load font (try system font, fallback to error message)
    error = FT_New_Face(library, "/System/Library/Fonts/Arial.ttf", 0, &face);
    if (error)
    {
        printf("Cannot load font, but FreeType is working!\n");
        printf("Try: ./hello [font_path]\n");
        FT_Done_FreeType(library);
        return 0;
    }
    // Set font size (width=0 means auto-calculate width based on height)
    FT_Set_Pixel_Sizes(face, 0, 48);
    printf("Font size set to: width=auto, height=48 pixels\n");
    
    // Test different characters to show size variations
    const char test_chars[] = {'i', 'H', 'W', 'g', 'j'};
    printf("\nCharacter size comparison (font height=48):\n");
    printf("Char | Width×Height | Advance | BearingY | Notes\n");
    printf("-----|-------------|---------|----------|----------------\n");
    
    for (int i = 0; i < 5; i++) {
        error = FT_Load_Char(face, test_chars[i], FT_LOAD_RENDER);
        if (!error) {
            FT_Bitmap *bitmap = &face->glyph->bitmap;
            printf(" %c   | %2d×%-2d       | %2ld      | %2d       | ",
                   test_chars[i], 
                   bitmap->width, bitmap->rows,
                   face->glyph->advance.x >> 6,
                   face->glyph->bitmap_top);
            
            if (test_chars[i] == 'i') printf("Narrow char\n");
            else if (test_chars[i] == 'H') printf("Standard cap\n");
            else if (test_chars[i] == 'W') printf("Wide char\n");
            else if (test_chars[i] == 'g') printf("Has descender\n");
            else if (test_chars[i] == 'j') printf("Narrow+descender\n");
        }
    }
    
    printf("\n💡 Atlas planning: Use max height ≈ 48px per row\n");
    printf("💡 Different chars have different widths but similar heights\n");
    // Cleanup
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    printf("\nFreeType Hello World complete!\n");
    return 0;
}