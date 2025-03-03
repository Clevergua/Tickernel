#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>



@interface AudioBinding : NSObject

@property (nonatomic, strong) AVAudioEngine *audioEngine;
@property (nonatomic, strong) NSMutableDictionary<NSString *, AVAudioPlayerNode *> *audioPlayers;
@property (nonatomic, strong) NSMutableDictionary<NSString *, AVAudioFile *> *audioFiles;

- (instancetype)init;

- (void)loadAudio:(NSString *)identifier fileType:(NSString *)fileType;
- (void)playAudio:(NSString *)identifier;
- (void)pauseAudio:(NSString *)identifier;
- (void)stopAudio:(NSString *)identifier;
- (void)unloadAudio:(NSString *)identifier;
- (void)set3DPositionForAudio:(NSString *)identifier X:(float)x Y:(float)y Z:(float)z;

@end
