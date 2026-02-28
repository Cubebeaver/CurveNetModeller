# Curve net modeller

## Introduction

This project is made for the BME Önálló laboratórium (Independent Laboratory) subject.

[The feeling when something works on the first try](https://youtube.com/shorts/AiARhlSTAeQ?si=o2nvnzgCfdInX-oA)



## Used tools

This project uses cmake as the build system and vcpkg as the package manager

Used packages:
 - GLFW (window manager)
 - glad (OpenGL binging)
 - glm (math library for OpenGL)
 - ImGUI (UI framework)
 - GTest (Unit testing framework)



## Building and running

For every command, be in the project root directory (where the CMakeLists.txt file is)

#### Initial setup for cmake and vcpkg

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="[vcpkg root directory]/scripts/buildsystems/vcpkg.cmake"
```

Example: `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"`

### Debug

#### Build program

```
cmake --build build
```

#### Run program

```
.\build\Debug\CurveNetModeller.exe
```

#### Run tests

```
.\build\Debug\CurveNetModellerTests.exe
```

#### Build and run program (for Windows PowerShell)

```
cmake --build build ; if ($?) { .\build\Debug\CurveNetModeller.exe }
```

#### Build and run tests (for Windows PowerShell)

```
cmake --build build ; if ($?) { .\build\Debug\CurveNetModellerTests.exe }
```

### Release

#### Build program

```
cmake --build build --config Release
```

#### Run program

```
.\build\Release\CurveNetModeller.exe
```

#### Build and run program (for Windows PowerShell)

```
cmake --build build --config Release ; if ($?) { .\build\Release\CurveNetModeller.exe }
```







# ~ Magyar változat ~

## Bemutatkozás

Ez a projekt a BME Önálló laboratórium tárgy keretein belül készült el.

[Az érzés amikor valami elsőre működik](https://youtube.com/shorts/AiARhlSTAeQ?si=o2nvnzgCfdInX-oA)



## Felhasznált eszközök

Ez a projekt Cmake-et használ build rendszernek, és vcpkg-t csomagkezelőnek.

Felhasznált csomagok:
 - GLFW (ablakkezelő)
 - glad (OpenGL kötés)
 - glm (matematikai könyvtár OpenGL-hez)
 - ImGUI (kezelőfelület)
 - GTest (egységtesztelő keretrendszer)



## Fordítás és futtatás

Minden parancshoz legyél a projekt gyökérkönyvtárában (ahol a CMakeLists.txt is van)

#### Cmake és vcpkg beállítása

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="[vcpkg root directory]/scripts/buildsystems/vcpkg.cmake"
```

Például: `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"`

### Debug

#### Program fordítása

```
cmake --build build
```

#### Program futtatása

```
.\build\Debug\CurveNetModeller.exe
```

#### Tesztek futtatása

```
.\build\Debug\CurveNetModellerTests.exe
```

#### Program fordítása és futtatása (Windows PowerShell-ben)

```
cmake --build build ; if ($?) { .\build\Debug\CurveNetModeller.exe }
```

#### Fordítása és tesztek futtatása (Windows PowerShell-ben)

```
cmake --build build ; if ($?) { .\build\Debug\CurveNetModellerTests.exe }
```

### Release

#### Program fordítása

```
cmake --build build --config Release
```

#### Program futtatása

```
.\build\Release\CurveNetModeller.exe
```

#### Program fordítása és futtatása (Windows PowerShell-ben)

```
cmake --build build --config Release ; if ($?) { .\build\Release\CurveNetModeller.exe }
```