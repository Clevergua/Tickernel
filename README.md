# Tickernel ⚙
A minimal game engine written in clean C, rendering voxels via point polygon mode with a deferred rendering pipeline. It exclusively uses Vulkan as its graphics API.
With a lean dependency footprint, it relies solely on C99 and the C standard library—ensuring broad compatibility across systems with a C99-compliant compiler and Vulkan support.

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

## Naming Conventions for For Loop Indices
In multi-level nesting, using i, j, k as indices may cause confusion. Therefore, the index of a for loop must be declared with a meaningful name (e.g., deviceIndex) to avoid confusion.

## Minimal Conditional Checks in Non-Essential Code
Unnecessary code will minimize conditional checks, as excessive checks can slow down runtime performance and increase code volume.

# Directory Structure and Introduction

