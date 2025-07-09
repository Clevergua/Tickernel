# Tickernel ⚙
 It is a minimal game engine written in Clean C language, rendering voxels using point polygon mode, and employing a deferred rendering pipeline. It uses Vulkan as the graphics API and supports only Vulkan. It is compatible with Posix standards and the Windows platform.

``` mermaid
flowchart TD
    A(["Engine Sources 📘"]) -- "Depends on Engine Sources" --> B(["Game Sources 📙"]) & C(["Engine Tools 🛠️"])
    C -- "Generates platform-specific resources & libraries via CMake" --> D(["Platform Projects 💻"])
    D -- "Builds game application from platform projects" --> E(["Game Application 🎮"])
    E -- "Builds game editor based on game application" --> F(["Game Editor 🖊️"])
    B -- "Integrates game sources into platform projects" --> D
```


代码风格设计原则:
1. 函数名称采用驼峰命名法.
2. 为了兼容多返回值函数,因此所有函数的返回值均为void.参数中前面的为输入,后面的为输出.
3. 所有针对对象操作的函数输入均为create(T *) update(T *) destroy(T)