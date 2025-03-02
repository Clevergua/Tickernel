#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface AudioBinding : NSObject

@property (nonatomic, strong) AVAudioEngine *audioEngine;
@property (nonatomic, strong) NSMutableDictionary<NSString *, AVAudioPlayerNode *> *audioPlayers;
@property (nonatomic, strong) NSMutableDictionary<NSString *, AVAudioFile *> *audioFiles;

- (instancetype)init;
- (void)loadAudioWithIdentifier:(NSString *)identifier fileName:(NSString *)fileName fileType:(NSString *)fileType;
- (void)playAudioWithIdentifier:(NSString *)identifier;
- (void)pauseAudioWithIdentifier:(NSString *)identifier;
- (void)stopAudioWithIdentifier:(NSString *)identifier;
- (void)unloadAudioWithIdentifier:(NSString *)identifier;
- (void)set3DPositionForAudioWithIdentifier:(NSString *)identifier X:(float)x Y:(float)y Z:(float)z;


@end
