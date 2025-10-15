#pragma once
#include "tkn.h"

// ASTC image data
typedef struct
{
    uint32_t width;    // Image width
    uint32_t height;   // Image height
    VkFormat vkFormat; // Corresponding Vulkan ASTC format
    uint32_t dataSize; // Compressed data size
    uint8_t *data;     // Compressed ASTC data
} ASTCImage;

ASTCImage *createASTCFromMemory(const uint8_t *buffer, size_t bufferSize);
void destroyASTCImage(ASTCImage *astcImage);