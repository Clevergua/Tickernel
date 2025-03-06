#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface AudioBinding : NSObject
@property (nonatomic, strong) AVAudioEngine *audioEngine;
@property (nonatomic, strong) NSMutableArray<AVAudioPlayerNode *> *audioPlayerPool;
@property (nonatomic, strong) NSMutableDictionary<NSString *, AVAudioFile *> *fileNameToAudioFile;

- (instancetype)init;
- (void)loadAudio:(NSString *)fileName;
- (void)unloadAudio:(NSString *)fileName;

- (AVAudioPlayerNode *)createAudioPlayer;
- (void)destroyAudioPlayer:(AVAudioPlayerNode *)audioPlayer;
- (void)playAudio:(AVAudioPlayerNode *)audioPlayer fileName:(NSString *)fileName;
- (void)pauseAudio:(AVAudioPlayerNode *)audioPlayer;
- (void)stopAudio:(AVAudioPlayerNode *)audioPlayer;
- (void)set3DPositionForAudio:(NSString *)identifier X:(float)x Y:(float)y Z:(float)z;

@end
