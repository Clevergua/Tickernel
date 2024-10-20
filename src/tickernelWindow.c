#include <tickernelWindow.h>

#if PLATFORM_WINDOWS
#include <windows.h>
#include <vulkan/vulkan_win32.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    TickernelWindow *pTickernelWindow = (TickernelWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (uMsg)
    {
    case WM_CLOSE:
        pTickernelWindow->shouldClose = true;
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void TickernelCreateWindow(uint32_t windowWidth, uint32_t windowHeight, const char *name, TickernelWindow *pTickernelWindow)
{
    pTickernelWindow->shouldClose = false;
    pTickernelWindow->hInstance = GetModuleHandle(NULL);
    const char className[] = "TickernelWindowClass";

    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = pTickernelWindow->hInstance;
    windowClass.lpszClassName = className;

    if (!RegisterClass(&windowClass))
    {
        DWORD error = GetLastError();
        printf("Failed to register window class. Error code: %lu\n", error);
        abort();
    }

    pTickernelWindow->hwnd = CreateWindowEx(
        0,
        className,
        name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        NULL,
        NULL,
        pTickernelWindow->hInstance,
        NULL);

    if (pTickernelWindow->hwnd == NULL)
    {
        DWORD error = GetLastError();
        printf("Failed to create window. Error code: %lu\n", error);
        abort();
    }

    SetWindowLongPtr(pTickernelWindow->hwnd, GWLP_USERDATA, (LONG_PTR)pTickernelWindow);

    ShowWindow(pTickernelWindow->hwnd, SW_SHOWDEFAULT);
}

void TickernelDestroyWindow(TickernelWindow *pTickernelWindow)
{
    DestroyWindow(pTickernelWindow->hwnd);
    UnregisterClass("TickernelWindowClass", pTickernelWindow->hInstance);
}

void TickernelGetWindowExtensionCount(uint32_t *pWindowExtensionCount)
{
    *pWindowExtensionCount = 2;
}

void TickernelGetWindowExtensions(char **windowExtensions)
{
    windowExtensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
    windowExtensions[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

VkResult CreateWindowVkSurface(TickernelWindow *pTickernelWindow, VkInstance vkInstance, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pVkSurface)
{
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = pTickernelWindow->hwnd;
    createInfo.hinstance = pTickernelWindow->hInstance;
    return vkCreateWin32SurfaceKHR(vkInstance, &createInfo, pAllocator, pVkSurface);
}

void TickernelGetWindowFramebufferSize(TickernelWindow *pTickernelWindow, uint32_t *pWidth, uint32_t *pHeight)
{
    RECT rect;
    GetClientRect(pTickernelWindow->hwnd, &rect);
    *pWidth = rect.right - rect.left;
    *pHeight = rect.bottom - rect.top;
}

void TickernelWaitWindowEvent()
{
    MSG msg;
    if (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void TickernelPollWindowEvents()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

bool TickernelWindowShouldClose(TickernelWindow *pTickernelWindow)
{
    return pTickernelWindow->shouldClose;
}

#endif
