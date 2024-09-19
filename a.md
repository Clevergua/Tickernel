Android Studio 中 IL2CPP 调试技巧

在Unity 开发过程中，我们经常遇到需要调试脚本代码的情况，通常我们可以勾选 Build Settings 里的 Script Debugging 选项来开启脚本调试，这样可以很方便的在 IDE中调试C#代码。

如果是 IL2CPP Backend，IL=>CPP的转换已经让C++代码量剧增了，开启 Script Debugging 需要插入额外的代码，不仅进一步增加打包编译的时间，而且很容易导致CPP编译的可执行文件的大小超过编译器限制，出现类似如下的报错而无法调试。



此外如果我们遇到 IL2CPP 的 Bug，或者有进阶的需求想要调试 IL2CPP 生成的 CPP 代码，那么我们就需要通过各个平台提供的工具来进行了。我们以Android平台为例，来看一下在Android Studio中，如何去配置 IL2CPP 的调试环境并通过断点调试代码。

首先下载并安装好 Android Studio并下载相关的 SDK和工具。然后打开 Unity 工程，在 Player Setting里，选择 Android 平台，确保 Scripting Backend 是 IL2CPP，并设置好 CPU架构，Target API Level等选项。然后 Build Settings中勾选 Export Project 和 Development Build选项，这时原本的 Build按钮就变成了 Export，我们选择好目录导出。



Export 的过程相比原先的 Build 会快很多，因为它只导出IL 转换的 cpp 代码，编译的过程留到了 Android Studio里去执行。

下一步在 Android Studio 中打开我们导出的工程，通常会弹出提示框，告诉我们导出的这个工程用了和 Android Studio 默认不同的 sdk路径（就是 Unity Preference => External Tools 里对应的 sdk路径）。推荐选 Use Project's SDK，因为 Unity 的编译对 Gradle，SDK，NDK，JDK 等相关工具链的版本都是有一定要求的，Android Studio 默认环境如果不一致很容易造成失败。



这就引申出另一个问题，Android Studio的版本如果太新，会与老的 Gradle JDK等出现兼容性问题，所以建议使用2022 或 2021 的老版本来避免不必要的麻烦。



打开 AndroidStudio 之后，同步并构建好工程（这一步可能时间会较长，其中包含了编译 IL2CPP 的 CPP 代码），点击 Edit Configurations:



在 Debugger 选项卡中选择 Debug type 为 Native Only:



并将 Symbol Directories 设置为 unityLibrary/symbols/[你的abi类型]:



到这里我们的工程设置就基本完成了，随后我们可以按照常规的 CPP 调试流程去进行了，Unity导出的 IL2CPP代码的目录在 unityLibrary/src/main/Il2CppOutputProject 中，里面有两个目录，IL2CPP 里包含了虚拟机的运行时代码，Source 则是根据我们的 Assembly IL 转化得到的 CPP代码，二者一起编译链接得到 libil2cpp.so。



我们测试一下断点调试，我们打开 libil2cpp/vm/GlobalMetadata.cpp，在 InitializeRuntimeMetadata 开头设置断点，然后点击 Debug 按钮。





等设备上app启动完毕，debugger attach之后，就会触发断点，我们可以看到调用堆栈，以及各个变量的值。此外 LLDB 以及 memory view窗口也可以查看更多调试信息。



以上就是 Android 平台的 IL2CPP代码调试步骤，这个方法还适用于调试项目中的其他 native plugin 代码，只要设置好符号文件路径，即可很方便的调试 CPP 代码。
