#pragma once
#include "cglm.h"
#include "tknCore.h"
#include "spirv_reflect.h"

typedef struct
{

} GraphicsContext;

void createGraphicsContext(int targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, GraphicsContext *pGraphicsContext);
void updateGraphicsContext(GraphicsContext *pGraphicsContext, VkExtent2D swapchainExtent);
void destroyGraphicsContext(GraphicsContext graphicsContext);