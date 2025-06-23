#pragma once
#include "graphics.h"
typedef struct
{
    uint32_t frameCount;
    uint32_t targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;
    GraphicsContext graphicsContext;
} TickernelEngine;

void tickernelStart(uint32_t targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight, TickernelEngine **ppTickernelEngine);
void tickernelUpdate(TickernelEngine *pTickernelEngine, uint32_t swapchainWidth, uint32_t swapchainHeight);
void tickernelEnd(TickernelEngine *pTickernelEngine);
