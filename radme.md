# Curve-net modeller

A magyar válozatért görgess lejjebb... // For the hungarian version scroll down...

## Introduction

This project is made for the BME Önálló laboratórium (Independent Laboratory) subject.

This program allows you to create and edit curves, and surfaces.

[The feeling when something works on the first try](https://youtube.com/shorts/AiARhlSTAeQ?si=o2nvnzgCfdInX-oA)


# For users

## Navigation

Hold right click, and move around with the WASD and QE keys.

Manipulate the objects by left clicking and dragging.

Workspaces can be resized and docked to your heart's content.

Detalied documentation can be found [here](https://cubebeaver.hu/curve-net-modeller/documentation).

**Happy editing!**

# For developers

## Used tools

This project uses cmake as the build system and vcpkg as the package manager

Used packages:
 - GLFW (window manager)
 - glad (OpenGL binging)
 - glm (math library for OpenGL)
 - ImGUI (UI framework)
 - Cereal (serialization)
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

# Görbe-háló modellező

## Bemutatkozás

Ez a projekt a BME Önálló laboratórium tárgy keretein belül készült el.

Ez a program lehetővé teszi görbék és felületek létrehozását és szerkesztését.

[Az érzés amikor valami elsőre működik](https://youtube.com/shorts/AiARhlSTAeQ?si=o2nvnzgCfdInX-oA)

# Felhasználóknak

## Navigáció

Tartsd lenyomva a jobb egérgombot, és mozogj a WASD és QE gombokkal.

Szerkeszd az objektumokat a bal egérgomb nyomvatartásával és húzásával.

A munkaterületeket szabadon méretezheted és dokkolhatod.

A részletes dokumentációt [itt](https://cubebeaver.hu/curve-net-modeller/documentation) találod.

**Boldog szerkesztést!**



# Fejlesztőknek

## Felhasznált eszközök

Ez a projekt Cmake-et használ build rendszernek, és vcpkg-t csomagkezelőnek.

Felhasznált csomagok:
 - GLFW (ablakkezelő)
 - glad (OpenGL kötés)
 - glm (matematikai könyvtár OpenGL-hez)
 - ImGUI (kezelőfelület)
 - Cereal (szerializáció)
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