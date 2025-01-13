import Cocoa

@main
class AppDelegate: NSObject, NSApplicationDelegate {

    func applicationWillFinishLaunching(_ notification: Notification) {
        print("Application will finish launching")
    }
    
    func applicationDidFinishLaunching(_ notification: Notification) {
        print("Application did finish launching")
    }
    
    func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
        print("Application should terminate")
        return .terminateNow
    }
    
    func applicationWillTerminate(_ notification: Notification) {
        print("Application will terminate")
    }
    
    func applicationDidBecomeActive(_ notification: Notification) {
        print("Application did become active")
    }
    
    func applicationWillResignActive(_ notification: Notification) {
        print("Application will resign active")
    }
    
    func applicationDidHide(_ notification: Notification) {
        print("Application did hide")
    }
    
    func applicationDidUnhide(_ notification: Notification) {
        print("Application did unhide")
    }
    
    func applicationWillBecomeActive(_ notification: Notification) {
        print("Application will become active")
    }
    
    func applicationWillUpdate(_ notification: Notification) {
        print("Application will update")
    }
    
    func applicationDidUpdate(_ notification: Notification) {
        print("Application did update")
    }
}
