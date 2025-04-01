# Tickernel ⚙
 It is a minimal game engine written in Clean C language, rendering voxels using point polygon mode, and employing a deferred rendering pipeline. It uses Vulkan as the graphics API and supports only Vulkan. It is compatible with Posix standards and the Windows platform.

``` mermaid
    flowchart TD
    A(["Engine Sources"]) --> B(["Game Sources"])
    B --> C(["Engine Tools"])
    C --> n1(["Platform Projects"])
    n1 --> n2(["Game Application"])
    n2 --> n3(["Game Editor"])
```