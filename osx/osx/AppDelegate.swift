import Cocoa
import Foundation

@main
class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!
    var metalView: MetalView!

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        window = NSWindow(contentRect: NSMakeRect(0, 0, 800, 600),
                          styleMask: [.titled, .closable, .resizable, .miniaturizable],
                          backing: .buffered,
                          defer: false)
        window.center()
        window.setFrameAutosaveName("Main Window")

        metalView = MetalView(frame: window.contentView!.bounds)
        window.contentView!.addSubview(metalView)

        window.makeKeyAndOrderFront(nil)

        setupVulkanSurface()
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // 插入代码进行应用清理

    }

    func setupVulkanSurface() {
//        guard let layer = metalView.layer as? CAMetalLayer else {
//            fatalError("视图没有 CAMetalLayer")
//        }
//        
//        var surface: VkSurfaceKHR?
//
//        var createInfo = VkMacOSSurfaceCreateInfoMVK()
//        createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK
//        createInfo.pNext = nil
//        createInfo.flags = 0
//        createInfo.pView = Unmanaged.passUnretained(layer).toOpaque()
//
//        if vkCreateMacOSSurfaceMVK(instance, &createInfo, nil, &surface) != VK_SUCCESS {
//            fatalError("创建 Vulkan 表面失败")
//        }
//        
        // 保存 Surface 以供 Vulkan 设置使用
    }
}

class MetalView: NSView {
    override init(frame frameRect: NSRect) {
        super.init(frame: frameRect)
        self.wantsLayer = true
        self.layer = CAMetalLayer()
    }

    required init?(coder: NSCoder) {
        fatalError("init(coder:) 未被实现")
    }
}
