#pragma once
#include "graphicCore.h"

typedef struct
{
    Buffer vertexBuffer;
    uint32_t vertexCount;

    MappedBuffer instanceBuffer;
    uint32_t instanceCount;
    uint32_t maxInstanceCount;
} GeometryMesh;

typedef struct
{
    vec3 position;
    vec3 color;
    vec3 normal;
} GeometrySubpassVertex;

typedef struct
{
    mat4 model;
} GeometrySubpassInstance;

void createGeometrySubpass(Subpass *pGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t geometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void destroyGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice);
void recordGeometrySubpass(Subpass *pGeometrySubpass, VkCommandBuffer vkCommandBuffer);