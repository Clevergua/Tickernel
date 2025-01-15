//
//  AppDelegate.h
//  osx
//
//  Created by ForgemasterGua on 15/01/25.
//

#import <Cocoa/Cocoa.h>
#import <vulkan/vulkan.h>
@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (assign, nonatomic) VkInstance vkInstance;
@end

