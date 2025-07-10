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


# Code Style Design Principles

## Function Naming
Function names use camelCase.

## Return Value and Parameters
To be compatible with functions that return multiple values, all functions have a return type of void. Among the parameters, the ones at the front are inputs, and the ones at the back are outputs.

## Object Operation Functions and Special Cases
All functions that operate on objects follow the forms of `create(T *)`, `update(T *)`, and `destroy(T)`, and other functions should be consistent with these as much as possible. However, the `malloc` function uses a method signature with a return value, consistent with the system's `malloc`.

## # Naming Conventions for For Loop Indices
In multi-level nesting, using i, j, k as indices may cause confusion. Therefore, the index of a for loop must be declared with a meaningful name (e.g., deviceIndex) to avoid confusion.