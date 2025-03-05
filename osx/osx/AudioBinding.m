#import "AudioBinding.h"
#import <AVFoundation/AVFoundation.h>

@implementation AudioBinding

- (instancetype)init {
    self = [super init];
    if (self) {
        self.audioEngine = [[AVAudioEngine alloc] init];
        self.id2AudioFile = [NSMutableDictionary dictionary];
        self.unusedAudioPlayers = [[NSMutableArray alloc] init];
        NSError *error = nil;
        if (![self.audioEngine startAndReturnError:&error])
        {
            NSLog(@"AudioEngine failed to start!(%@)", error.localizedDescription);
        }
        else
        {
            NSLog(@"Engine started successfully.");
        }
    }
    return self;
}

- (void)dealloc {
    [self.audioEngine stop];
    for (NSString *identifier in self.id2AudioFile.allKeys)
    {
        [self unloadAudio:identifier];
    }
//    for (AVAudioPlayerNode *audioPlayer in self.unusedAudioPlayers) {
//        [audioPlayer stop];
//        [self.audioEngine detachNode:audioPlayer];
//    }
    [self.unusedAudioPlayers removeAllObjects];
    [self.id2AudioFile removeAllObjects];
    self.audioEngine = nil;
}


- (void)loadAudio:(NSString *)identifier fileType:(NSString *)fileType {
    NSLog(@"Load audio: %@.%@ .", identifier, fileType);
    NSString *path = [[NSBundle mainBundle] pathForResource:identifier ofType:fileType];
    if (!path) {
        NSLog(@"Audio: %@.%@ not found!", identifier, fileType);
        return;
    }
    NSURL *url = [NSURL fileURLWithPath:path];
    NSError *error = nil;
    AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:url error:&error];
    if (error) {
        NSLog(@"Failed to load audio: %@!", error.localizedDescription);
        return;
    }
    
    //    AVAudioPlayerNode *audioPlayer = [[AVAudioPlayerNode alloc] init];
    //    [self.audioEngine attachNode:audioPlayer];
    //    [self.audioEngine connect:audioPlayer to:self.audioEngine.mainMixerNode format:audioFile.processingFormat];
    //    self.audioPlayers[identifier] = audioPlayer;
    
    self.id2AudioFile[identifier] = audioFile;
    NSLog(@"Load audio: %@ successfully.", identifier);
}

- (void)unloadAudio:(NSString *)identifier {
    //    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    AVAudioFile *audioFile = self.id2AudioFile[identifier];
    if (!audioFile)
    {
        NSLog(@"Unloaded audio!: %@", identifier);
        return;
    }
    else
    {
//      [audioPlayer stop];
//      [self.audioEngine detachNode:audioPlayer];
//      [self.audioPlayers removeObjectForKey:identifier];
        [self.id2AudioFile removeObjectForKey:identifier];
        NSLog(@"Unloaded audio: %@", identifier);
    }
}


- (void)playAudio:(NSString *)identifier {
    
    AVAudioPlayerNode *audioPlayer = self.unusedAudioPlayers[identifier];
    AVAudioFile *audioFile = self.id2AudioFile[identifier];
    if (!audioPlayer || !audioFile) {
        NSLog(@"音频未加载: %@", identifier);
        return;
    }
    
    // 播放音频
    [audioPlayer scheduleFile:audioFile atTime:nil completionHandler:^{
        NSLog(@"音频播放完成: %@", identifier);
    }];
    [audioPlayer play];
    
    NSLog(@"音频播放: %@", identifier);
}

- (void)pauseAudio:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.unusedAudioPlayers[identifier];
    if (!audioPlayer) {
        NSLog(@"音频未加载: %@", identifier);
        return;
    }
    
    // 暂停音频
    [audioPlayer pause];
    
    NSLog(@"音频暂停: %@", identifier);
}

- (void)stopAudio:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.unusedAudioPlayers[identifier];
    if (!audioPlayer) {
        NSLog(@"音频未加载: %@", identifier);
        return;
    }
    
    // 停止音频
    [audioPlayer stop];
    
    NSLog(@"音频停止: %@", identifier);
}


- (void)set3DPositionForAudio:(NSString *)identifier X:(float)x Y:(float)y Z:(float)z {
    AVAudioPlayerNode *audioPlayer = self.unusedAudioPlayers[identifier];
    if (!audioPlayer) {
        NSLog(@"音频未加载: %@", identifier);
        return;
    }
    
    // 设置 3D 位置
    AVAudio3DPoint position = AVAudioMake3DPoint(x, y, z);
    audioPlayer.position = position;
    
    NSLog(@"设置音频 3D 位置: %@ -> (%.2f, %.2f, %.2f)", identifier, x, y, z);
}

@end
