#import "AudioBinding.h"
#import <AVFoundation/AVFoundation.h>

@implementation AudioBinding
- (instancetype)init {
    self = [super init];
    if (self)
    {
        self.audioEngine = [[AVAudioEngine alloc] init];
        self.fileNameToAudioFile = [NSMutableDictionary dictionary];
        self.unusedAudioPlayers = [[NSMutableArray alloc] init];
        self.usedAudioPlayers = [[NSMutableArray alloc] init];
        self.usedAudioFileNames = [[NSMutableArray alloc] init];
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
    for (NSString *identifier in self.fileNameToAudioFile.allKeys)
    {
        [self unloadAudio:identifier];
    }
    
    [self.usedAudioPlayers removeAllObjects];
    [self.usedAudioFileNames removeAllObjects];
    [self.unusedAudioPlayers removeAllObjects];
    [self.fileNameToAudioFile removeAllObjects];
    self.audioEngine = nil;
}


- (void)loadAudio:(NSString *)fileName {
    if ([self.fileNameToAudioFile objectForKey: fileName])
    {
        NSLog(@"Audio : %@ has been loaded!", fileName);
    }
    else
    {
        NSString *path = [[NSBundle mainBundle] pathForResource:[fileName stringByDeletingPathExtension] ofType:[fileName pathExtension]];
        NSURL *url = [NSURL fileURLWithPath:path];
        NSError *error = nil;
        AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:url error:&error];
        if (error)
        {
            NSLog(@"Failed to load audio: %@!", error.localizedDescription);
        }
        else
        {
            self.fileNameToAudioFile[fileName] = audioFile;
            NSLog(@"Load audio: %@ successfully.", fileName);
        }
    }
}

- (void)unloadAudio:(NSString *) fileName {
    //    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    AVAudioFile *audioFile = self.fileNameToAudioFile[fileName];
    if (!audioFile)
    {
        NSLog(@"Unloaded audio!: %@", fileName);
        return;
    }
    else
    {
        for (NSUInteger i = self.usedAudioPlayers.count - 1; i > -1 ; i--) {
            AVAudioPlayerNode *currentAudioPlayerNode = self.usedAudioPlayers[i];
            NSString *currentFileName = self.usedAudioFileNames[i];
            if ([currentFileName isEqualToString:fileName]) {
                if (currentAudioPlayerNode.isPlaying) {
                    [currentAudioPlayerNode stop];
                }
                
                [self.usedAudioPlayers removeObjectAtIndex:i];
                [self.usedAudioFileNames removeObjectAtIndex:i];
                NSLog(@"已移除文件名: %@ 对应的 AVAudioPlayerNode", fileName);
            }
        }        //      [audioPlayer stop];
        //      [self.audioEngine detachNode:audioPlayer];
        //      [self.audioPlayers removeObjectForKey:identifier];
        [self.fileNameToAudioFile removeObjectForKey:fileName];
        NSLog(@"Unloaded audio: %@", fileName);
    }
}


- (void)playAudio:(NSString *)identifier {
    
    //    AVAudioPlayerNode *audioPlayer = [[AVAudioPlayerNode alloc] init];
    //    [self.audioEngine attachNode:audioPlayer];
    //    [self.audioEngine connect:audioPlayer to:self.audioEngine.mainMixerNode format:audioFile.processingFormat];
    //    self.audioPlayers[identifier] = audioPlayer;
    AVAudioPlayerNode *audioPlayer = self.unusedAudioPlayers[identifier];
    AVAudioFile *audioFile = self.fileNameToAudioFile[identifier];
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
    //    for (AVAudioPlayerNode *audioPlayer in self.unusedAudioPlayers) {
    //        [audioPlayer stop];
    //        [self.audioEngine detachNode:audioPlayer];
    //    }
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
