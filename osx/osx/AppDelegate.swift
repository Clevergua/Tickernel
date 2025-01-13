import Cocoa

@main
class AppDelegate: NSObject, NSApplicationDelegate, NSWindowDelegate {

    var window: NSWindow?
    var instance: VkInstance?
    var surface: VkSurfaceKHR?

    func applicationWillFinishLaunching(_ notification: Notification) {
        // print("Application will finish launching")
    }

    func applicationDidFinishLaunching(_ notification: Notification) {
        // print("Application did finish launching")
        createWindow()
        createVulkanInstance()
        createVulkanSurface()
    }

    func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
        // print("Application should terminate")
        return .terminateNow
    }

    func applicationWillTerminate(_ notification: Notification) {
        // print("Application will terminate")
        destroyVulkanSurface()
        destroyVulkanInstance()
        destroyWindow()
    }

    func applicationDidBecomeActive(_ notification: Notification) {
        // print("Application did become active")
    }

    func applicationWillResignActive(_ notification: Notification) {
        // print("Application will resign active")
    }

    func applicationDidHide(_ notification: Notification) {
        // print("Application did hide")
    }

    func applicationDidUnhide(_ notification: Notification) {
        // print("Application did unhide")
    }

    func applicationWillBecomeActive(_ notification: Notification) {
        // print("Application will become active")
    }

    func applicationWillUpdate(_ notification: Notification) {
        // print("Application will update")
    }

    func applicationDidUpdate(_ notification: Notification) {
        // print("Application did update")
    }

    func createWindow() {
        let screenSize = NSScreen.main?.frame.size ?? CGSize(width: 800, height: 600)
        window = NSWindow(contentRect: NSMakeRect(0, 0, screenSize.width, screenSize.height),
                          styleMask: [.titled, .closable, .resizable, .miniaturizable],
                          backing: .buffered, defer: false)
        window?.center()
        window?.delegate = self
        window?.makeKeyAndOrderFront(nil)
    }

    func destroyWindow() {
        if let window = window {
            window.orderOut(nil)
            window.close()
            self.window = nil
        }
    }

    func createVulkanInstance() {
        var createInfo = VkInstanceCreateInfo()
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO

        if vkCreateInstance(&createInfo, nil, &instance) != VK_SUCCESS {
            fatalError("Failed to create Vulkan instance")
        }
    }

    func destroyVulkanInstance() {
        if let instance = instance {
            vkDestroyInstance(instance, nil)
            self.instance = nil
        }
    }

    func createVulkanSurface() {
        var surfaceCreateInfo = VkMacOSSurfaceCreateInfoMVK()
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK

        guard let nsView = window?.contentView else {
            fatalError("Failed to get main window content view")
        }

        surfaceCreateInfo.pView = UnsafeRawPointer(Unmanaged.passUnretained(nsView).toOpaque())

        if vkCreateMacOSSurfaceMVK(instance, &surfaceCreateInfo, nil, &surface) != VK_SUCCESS {
            fatalError("Failed to create Vulkan surface")
        }
    }

    func destroyVulkanSurface() {
        if let surface = surface {
            vkDestroySurfaceKHR(instance, surface, nil)
            self.surface = nil
        }
    }

    func windowWillClose(_ notification: Notification) {
        NSApplication.shared.terminate(self)
    }
}
