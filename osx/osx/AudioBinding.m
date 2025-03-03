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
        [self unloadAudio:identifier];
    }

    [self.audioPlayers removeAllObjects];
    [self.audioFiles removeAllObjects];


    self.audioEngine = nil;
}

- (void)loadAudio:(NSString *)identifier fileType:(NSString *)fileType {
    NSString *path = [[NSBundle mainBundle] pathForResource:identifier ofType:fileType];
    if (path) {
        NSURL *url = [NSURL fileURLWithPath:path];
        AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:url error:nil];
        if (audioFile) {
            self.audioFiles[identifier] = audioFile;
            AVAudioPlayerNode *audioPlayer = [[AVAudioPlayerNode alloc] init];
            [self.audioEngine attachNode:audioPlayer];
            [self.audioEngine connect:audioPlayer to:self.audioEngine.mainMixerNode format:audioFile.processingFormat];
            self.audioPlayers[identifier] = audioPlayer;
        }
    } else {
        NSLog(@"音频文件未找到: %@", path);
    }
}

- (void)playAudio:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    AVAudioFile *audioFile = self.audioFiles[identifier];
    if (audioPlayer && audioFile) {
        [audioPlayer scheduleFile:audioFile atTime:nil completionHandler:nil];
        [audioPlayer play];
    } else {
        NSLog(@"音频未加载: %@", identifier);
    }
}

- (void)pauseAudio:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    if (audioPlayer) {
        [audioPlayer pause];
    } else {
        NSLog(@"音频未加载: %@", identifier);
    }
}

- (void)stopAudio:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    if (audioPlayer) {
        [audioPlayer stop];
    } else {
        NSLog(@"音频未加载: %@", identifier);
    }
}

- (void)unloadAudio:(NSString *)identifier {
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

- (void)set3DPositionForAudio:(NSString *)identifier X:(float)x Y:(float)y Z:(float)z {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    if (audioPlayer) {
        AVAudio3DPoint position = AVAudioMake3DPoint(x, y, z);
        audioPlayer.position = position;
    } else {
        NSLog(@"音频未加载: %@", identifier);
    }
}


@end
