#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 字符信息结构体
typedef struct {
    uint32_t unicode;      // Unicode码点
    uint32_t x, y;         // 在atlas中的像素坐标
    uint32_t width, height; // 字符bitmap尺寸
    int32_t bearingX, bearingY; // 字形度量
    uint32_t advance;      // 前进距离
    float u1, v1, u2, v2;  // UV坐标
} CharInfo;

// 字体atlas结构体
typedef struct {
    uint8_t* data;         // atlas像素数据
    uint32_t width, height; // atlas尺寸
    CharInfo* charInfos;   // 字符信息数组
    uint32_t charCount;    // 字符数量
} FontAtlas;

// 顶点结构体（用于渲染四边形）
typedef struct {
    float x, y;            // 屏幕位置
    float u, v;            // UV坐标
} Vertex;

// 拷贝bitmap到atlas
void copyBitmapToAtlas(uint8_t* atlas, uint32_t atlasWidth, uint32_t atlasHeight,
                       FT_Bitmap* bitmap, uint32_t dstX, uint32_t dstY) {
    for (uint32_t y = 0; y < bitmap->rows; y++) {
        for (uint32_t x = 0; x < bitmap->width; x++) {
            uint32_t atlasIdx = (dstY + y) * atlasWidth + (dstX + x);
            uint32_t bmpIdx = y * bitmap->pitch + x;
            atlas[atlasIdx] = bitmap->buffer[bmpIdx];
        }
    }
}

// 创建字体atlas
FontAtlas* createFontAtlas(const char* fontPath, float fontSize, const char* chars) {
    FT_Library ftLib;
    FT_Face face;
    FT_Error err;

    // 初始化FreeType
    err = FT_Init_FreeType(&ftLib);
    if (err) { printf("FT_Init_FreeType failed\n"); return NULL; }

    // 加载字体
    err = FT_New_Face(ftLib, fontPath, 0, &face);
    if (err) { printf("FT_New_Face failed\n"); FT_Done_FreeType(ftLib); return NULL; }

    // 设置像素大小
    FT_Set_Pixel_Sizes(face, 0, (FT_UInt)fontSize);

    // 创建atlas
    uint32_t atlasWidth = 2048, atlasHeight = 2048;
    uint8_t* atlasData = (uint8_t*)calloc(atlasWidth * atlasHeight, 1);
    uint32_t charCount = (uint32_t)strlen(chars);
    CharInfo* charInfos = (CharInfo*)malloc(sizeof(CharInfo) * charCount);

    // 行优先布局
    uint32_t penX = 0, penY = 0, rowH = 0;
    for (uint32_t i = 0; i < charCount; i++) {
        uint32_t charCode = (uint32_t)chars[i];
        err = FT_Load_Char(face, charCode, FT_LOAD_RENDER);
        if (err) continue;

        FT_Bitmap* bmp = &face->glyph->bitmap;
        if (penX + bmp->width > atlasWidth) {
            penX = 0;
            penY += rowH;
            rowH = 0;
        }
        if (penY + bmp->rows > atlasHeight) break; // atlas满了

        // 拷贝bitmap到atlas
        copyBitmapToAtlas(atlasData, atlasWidth, atlasHeight, bmp, penX, penY);

        // 记录字符信息
        charInfos[i].unicode = charCode;
        charInfos[i].x = penX;
        charInfos[i].y = penY;
        charInfos[i].width = bmp->width;
        charInfos[i].height = bmp->rows;
        charInfos[i].bearingX = face->glyph->bitmap_left;
        charInfos[i].bearingY = face->glyph->bitmap_top;
        charInfos[i].advance = face->glyph->advance.x >> 6;

        // 计算UV坐标
        charInfos[i].u1 = (float)penX / atlasWidth;
        charInfos[i].v1 = (float)penY / atlasHeight;
        charInfos[i].u2 = (float)(penX + bmp->width) / atlasWidth;
        charInfos[i].v2 = (float)(penY + bmp->rows) / atlasHeight;

        penX += bmp->width + 2; // 字符间隔
        if (bmp->rows > rowH) rowH = bmp->rows;
    }

    // 保存atlas为PGM文件（用于调试）
    FILE* f = fopen("font_atlas.pgm", "wb");
    fprintf(f, "P5\n%d %d\n255\n", atlasWidth, atlasHeight);
    fwrite(atlasData, 1, atlasWidth * atlasHeight, f);
    fclose(f);

    // 创建FontAtlas结构体
    FontAtlas* atlas = (FontAtlas*)malloc(sizeof(FontAtlas));
    atlas->data = atlasData;
    atlas->width = atlasWidth;
    atlas->height = atlasHeight;
    atlas->charInfos = charInfos;
    atlas->charCount = charCount;

    // 清理FreeType
    FT_Done_Face(face);
    FT_Done_FreeType(ftLib);

    return atlas;
}

// 查找字符信息
CharInfo* findCharInfo(FontAtlas* atlas, uint32_t unicode) {
    for (uint32_t i = 0; i < atlas->charCount; i++) {
        if (atlas->charInfos[i].unicode == unicode) {
            return &atlas->charInfos[i];
        }
    }
    return NULL;
}

// 生成字符串的vertex数据
Vertex* generateStringVertices(FontAtlas* atlas, const char* text, uint32_t* vertexCount) {
    uint32_t textLen = (uint32_t)strlen(text);
    *vertexCount = textLen * 6; // 每个字符6个顶点（两个三角形）
    Vertex* vertices = (Vertex*)malloc(sizeof(Vertex) * (*vertexCount));

    float penX = 0.0f, penY = 0.0f; // 字符串起始位置
    uint32_t vertexIdx = 0;

    for (uint32_t i = 0; i < textLen; i++) {
        uint32_t charCode = (uint32_t)text[i];
        CharInfo* info = findCharInfo(atlas, charCode);
        if (!info) continue;

        // 计算字符四边形位置（左上角为基准）
        float x1 = penX + info->bearingX;
        float y1 = penY - info->bearingY;
        float x2 = x1 + info->width;
        float y2 = y1 + info->height;

        // 生成两个三角形的顶点
        // 三角形1
        vertices[vertexIdx++] = (Vertex){x1, y1, info->u1, info->v1}; // 左上
        vertices[vertexIdx++] = (Vertex){x2, y1, info->u2, info->v1}; // 右上
        vertices[vertexIdx++] = (Vertex){x1, y2, info->u1, info->v2}; // 左下
        // 三角形2
        vertices[vertexIdx++] = (Vertex){x2, y1, info->u2, info->v1}; // 右上
        vertices[vertexIdx++] = (Vertex){x2, y2, info->u2, info->v2}; // 右下
        vertices[vertexIdx++] = (Vertex){x1, y2, info->u1, info->v2}; // 左下

        // 前进到下一个字符位置
        penX += info->advance;
    }

    return vertices;
}

// 清理atlas
void destroyFontAtlas(FontAtlas* atlas) {
    free(atlas->data);
    free(atlas->charInfos);
    free(atlas);
}

int main() {
    // 创建atlas
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ";
    FontAtlas* atlas = createFontAtlas("/System/Library/Fonts/Helvetica.ttc", 32.0f, chars);
    if (!atlas) return 1;

    // 生成字符串vertex
    const char* text = "Hello World!";
    uint32_t vertexCount;
    Vertex* vertices = generateStringVertices(atlas, text, &vertexCount);

    // 输出vertex信息（示例）
    printf("Generated %u vertices for text: %s\n", vertexCount, text);
    for (uint32_t i = 0; i < vertexCount; i++) {
        printf("Vertex %u: pos(%.1f, %.1f) uv(%.3f, %.3f)\n", 
               i, vertices[i].x, vertices[i].y, vertices[i].u, vertices[i].v);
    }

    // 清理
    free(vertices);
    destroyFontAtlas(atlas);

    printf("Font atlas saved as font_atlas.pgm\n");
    return 0;
}