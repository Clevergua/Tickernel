#import "AudioBinding.h"
#import <AVFoundation/AVFoundation.h>

@implementation AudioBinding

- (instancetype)init {
    self = [super init];
    if (self) {
        // 初始化音频引擎和字典
        self.audioEngine = [[AVAudioEngine alloc] init];
        self.audioPlayers = [NSMutableDictionary dictionary];
        self.audioFiles = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void)dealloc {
    // 停止音频引擎并释放资源
    [self.audioEngine stop];
    for (NSString *identifier in self.audioPlayers.allKeys) {
        [self unloadAudio:identifier];
    }
    [self.audioPlayers removeAllObjects];
    [self.audioFiles removeAllObjects];
    self.audioEngine = nil;
}

- (void)startAudioEngine {
    // 启动音频引擎
    NSError *error = nil;
    if (![self.audioEngine startAndReturnError:&error]) {
        NSLog(@"音频引擎启动失败: %@", error.localizedDescription);
    } else {
        NSLog(@"音频引擎启动成功");
    }
}

- (void)loadAudio:(NSString *)identifier fileType:(NSString *)fileType {
    NSLog(@"加载音频: %@.%@", identifier, fileType);

    // 获取音频文件路径
    NSString *path = [[NSBundle mainBundle] pathForResource:identifier ofType:fileType];
    if (!path) {
        NSLog(@"音频文件未找到: %@.%@", identifier, fileType);
        return;
    }

    // 创建音频文件对象
    NSURL *url = [NSURL fileURLWithPath:path];
    NSError *error = nil;
    AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:url error:&error];
    if (error) {
        NSLog(@"音频文件加载失败: %@", error.localizedDescription);
        return;
    }

    // 创建音频播放器节点
    AVAudioPlayerNode *audioPlayer = [[AVAudioPlayerNode alloc] init];
    [self.audioEngine attachNode:audioPlayer];
    [self.audioEngine connect:audioPlayer to:self.audioEngine.mainMixerNode format:audioFile.processingFormat];

    // 存储音频文件和播放器节点
    self.audioFiles[identifier] = audioFile;
    self.audioPlayers[identifier] = audioPlayer;

    // 启动音频引擎
    [self startAudioEngine];

    NSLog(@"音频加载成功: %@", identifier);
}

- (void)playAudio:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    AVAudioFile *audioFile = self.audioFiles[identifier];
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
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    if (!audioPlayer) {
        NSLog(@"音频未加载: %@", identifier);
        return;
    }

    // 暂停音频
    [audioPlayer pause];

    NSLog(@"音频暂停: %@", identifier);
}

- (void)stopAudio:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    if (!audioPlayer) {
        NSLog(@"音频未加载: %@", identifier);
        return;
    }

    // 停止音频
    [audioPlayer stop];

    NSLog(@"音频停止: %@", identifier);
}

- (void)unloadAudio:(NSString *)identifier {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
    AVAudioFile *audioFile = self.audioFiles[identifier];
    if (!audioPlayer || !audioFile) {
        NSLog(@"音频未加载: %@", identifier);
        return;
    }

    // 停止并释放音频播放器节点
    [audioPlayer stop];
    [self.audioEngine detachNode:audioPlayer];
    [self.audioPlayers removeObjectForKey:identifier];
    [self.audioFiles removeObjectForKey:identifier];

    NSLog(@"音频卸载: %@", identifier);
}

- (void)set3DPositionForAudio:(NSString *)identifier X:(float)x Y:(float)y Z:(float)z {
    AVAudioPlayerNode *audioPlayer = self.audioPlayers[identifier];
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
