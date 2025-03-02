#import "AudioBinding.h"

@implementation AudioBinding

- (instancetype)init {
    self = [super init];
    if (self) {
        self.audioEngine = [[AVAudioEngine alloc] init];
        self.audioPlayers = [NSMutableDictionary dictionary];
        self.audioFiles = [NSMutableDictionary dictionary];
        
        NSError *error;
        [self.audioEngine startAndReturnError:&error];
        if (error) {
            NSLog(@"音频引擎启动失败: %@", error.localizedDescription);
        }
    }
    return self;
}

- (void)dealloc {
    [self.audioEngine stop];

    for (NSString *identifier in self.audioPlayers.allKeys) {
        [self unloadAudioWithIdentifier:identifier];
    }

    [self.audioPlayers removeAllObjects];
    [self.audioFiles removeAllObjects];


    self.audioEngine = nil;
}

- (void)loadAudioWithIdentifier:(NSString *)identifier fileName:(NSString *)fileName fileType:(NSString *)fileType {
    NSString *path = [[NSBundle mainBundle] pathForResource:fileName ofType:fileType];
    if (path) {
        NSURL *url = [NSURL fileURLWithPath:path];
        AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:url error:nil];
        if (audioFile) {
            self.audioFiles[identifier] = audioFile; // 存储音频文件
            AVAudioPlayerNode *audioPlayer = [[AVAudioPlayerNode alloc] init];
            [self.audioEngine attachNode:audioPlayer];
            [self.audioEngine connect:audioPlayer to:self.audioEngine.mainMixerNode format:audioFile.processingFormat];
            self.audioPlayers[identifier] = audioPlayer; // 存储音频播放器
        }
    } else {
        NSLog(@"音频文件未找到: %@", fileName);
    }
}

- (void)playAudioWithIdentifier:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    AVAudioFile *audioFile = self.audioFiles[identifier];
    if (audioPlayer && audioFile) {
        [audioPlayer scheduleFile:audioFile atTime:nil completionHandler:nil];
        [audioPlayer play];
    } else {
        NSLog(@"音频未加载: %@", identifier);
    }
}

- (void)pauseAudioWithIdentifier:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    if (audioPlayer) {
        [audioPlayer pause];
    } else {
        NSLog(@"音频未加载: %@", identifier);
    }
}

- (void)stopAudioWithIdentifier:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    if (audioPlayer) {
        [audioPlayer stop];
    } else {
        NSLog(@"音频未加载: %@", identifier);
    }
}

- (void)unloadAudioWithIdentifier:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    AVAudioFile *audioFile = self.audioFiles[identifier];
    if (audioPlayer) {
        [audioPlayer stop];
        [self.audioEngine detachNode:audioPlayer];
        [self.audioPlayers removeObjectForKey:identifier];
    }
    if (audioFile) {
        [self.audioFiles removeObjectForKey:identifier];
    }
}

- (void)set3DPositionForAudioWithIdentifier:(NSString *)identifier X:(float)x Y:(float)y Z:(float)z {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    if (audioPlayer) {
        AVAudio3DPoint position = AVAudioMake3DPoint(x, y, z);
        audioPlayer.position = position;
    } else {
        NSLog(@"音频未加载: %@", identifier);
    }
}


@end
