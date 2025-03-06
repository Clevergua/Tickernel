#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface AudioBinding : NSObject
@property (nonatomic, strong) AVAudioEngine *audioEngine;

@property (nonatomic, strong) NSMutableArray<NSString *> *usedAudioFileNames;
@property (nonatomic, strong) NSMutableArray<AVAudioPlayerNode *> *usedAudioPlayers;
@property (nonatomic, strong) NSMutableArray<AVAudioPlayerNode *> *unusedAudioPlayers;
@property (nonatomic, strong) NSMutableDictionary<NSString *, AVAudioFile *> *fileNameToAudioFile;

- (instancetype)init;
- (void)loadAudio:(NSString *)fileName;
- (void)unloadAudio:(NSString *)fileName;

- (void)playAudio:(NSString *)fileName;
- (void)pauseAudio:(NSString *)fileName;
- (void)stopAudio:(NSString *)fileName;

- (void)set3DPositionForAudio:(NSString *)identifier X:(float)x Y:(float)y Z:(float)z;

@end
