# Curve net modeller

## Introduction

This project is made for the BME Önálló laboratórium (Independent Laboratory) subject.



## Used tools

This project uses cmake as the build system and vcpkg as the package manager

Used packages:
 - GLFW (window manager)
 - glad (OpenGL binging)
 - glm (math library for OpenGL)
 - ImGUI (UI framework)



## Building and running

For every command, be in the project root directory (where the CMakeLists.txt file is)

### Initial setup for cmake and vcpkg

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="[vcpkg root directory]/scripts/buildsystems/vcpkg.cmake"
```

### Normal build

```
cmake --build build
```

### Run program

```
.\build\Debug\CurveNetModeller.exe
```

### Run tests

```
.\build\Debug\CurveNetModellerTests.exe
```

### Build and run program (for Windows PowerShell)

```
cmake --build build ; if ($?) { .\build\Debug\CurveNetModeller.exe }
```

### Build and run tests (for Windows PowerShell)

```
cmake --build build ; if ($?) { .\build\Debug\CurveNetModellerTests.exe }
```