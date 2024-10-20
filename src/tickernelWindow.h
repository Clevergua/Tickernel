#pragma once
#include <vulkan/vulkan.h>
#include <tickernelCore.h>

typedef struct
{
    bool shouldClose;
#if PLATFORM_WINDOWS
    HWND hwnd;
    HINSTANCE hInstance;
#endif
} TickernelWindow;

void TickernelCreateWindow(uint32_t windowWidth, uint32_t windowHeight, const char *name, TickernelWindow *pTickernelWindow);
void TickernelDestroyWindow(TickernelWindow *pTickernelWindow);
void TickernelGetWindowExtensionCount(uint32_t *pWindowExtensionCount);
void TickernelGetWindowExtensions(char **windowExtensions);
VkResult CreateWindowVkSurface(TickernelWindow *pTickernelWindow, VkInstance vkInstance, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pVkSurface);
void TickernelGetWindowFramebufferSize(TickernelWindow *pTickernelWindow, uint32_t *pWidth, uint32_t *pHeight);
void TickernelWaitWindowEvent();
void TickernelPollWindowEvents();
bool TickernelWindowShouldClose(TickernelWindow *pTickernelWindow);
