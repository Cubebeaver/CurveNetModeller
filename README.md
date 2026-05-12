# Curve-net modeller

> [**Magyar változatért görgess lejjebb // Hungarian version follows below**](#görbe-háló-modellező)

## Introduction

This project is made for the BME Önálló laboratórium (Independent Laboratory) subject.

This program allows you to create and edit curves, and surfaces.

[The feeling when something works on the first try](https://youtube.com/shorts/AiARhlSTAeQ?si=o2nvnzgCfdInX-oA)


# For users

## Navigation

Hold right click, and move around with the WASD and QE keys.

Manipulate the objects by left clicking and dragging, hold shift to multi-select.

Workspaces can be resized and docked to your heart's content.

There are two workspaces:
- Viewport: where you can see the curves and surfaces
- Properties: where you find the properties of the selected object/elements and can perform operations on them

Detalied documentation can be found [here](https://docs.cubebeaver.hu). (TBA)

**Happy editing!**

# For developers

## Used tools and licenses

This project uses cmake as the build system and vcpkg as the package manager

Used packages:
 - [GLFW](https://www.glfw.org/) (Zlib License) - Window manager.
 - [GLAD](https://github.com/Dav1dde/glad) (MIT License) - OpenGL binding.
 - [GLM](https://github.com/g-truc/glm) (Happy Bunny / MIT License) - Math library for OpenGL.
 - [Dear ImGui](https://github.com/ocornut/imgui) (MIT License) - UI framework.
 - [stb_image](https://github.com/nothings/stb) (Public Domain / MIT) - Image loader.
 - [cereal](https://uscilab.github.io/cereal/) (BSD License) - Serialization.
 - [Google Test](https://github.com/google/googletest) (BSD License) - Unit testing framework.

Used resources:
- [FreeStylized.com](https://freestylized.com/) (CC0 with limitations) - Textures and skyboxes

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



&nbsp;

&nbsp;

> **Magyar változat // Hungarian version**



# Görbe-háló modellező

## Bemutatkozás

Ez a projekt a BME Önálló laboratórium tárgy keretein belül készült el.

Ez a program lehetővé teszi görbék és felületek létrehozását és szerkesztését.

[Az érzés amikor valami elsőre működik](https://youtube.com/shorts/AiARhlSTAeQ?si=o2nvnzgCfdInX-oA)

# Felhasználóknak

## Navigáció

Tartsd lenyomva a jobb egérgombot, és mozogj a WASD és QE gombokkal.

Szerkeszd az objektumokat a bal egérgomb nyomvatartásával és húzásával, nyomd a shiftet, ha egyszerre több elemet szeretnél kiválasztani.

A munkaterületeket szabadon méretezheted és dokkolhatod.

Két munkaterület van:
- Viewport (3D nézetablak): Itt látod a görbéidet és felületeidet
- Properties (Tulajdonságok): Itt találod a kiválasztott elem tulajdonságait és itt tudsz műveleteket végrehajtani rajtuk.

A részletes dokumentációt [itt](https://docs.cubebeaver.hu) találod. (TBA)

**Boldog szerkesztést!**



# Fejlesztőknek

## Felhasznált eszközök és licencek

Ez a projekt Cmake-et használ build rendszernek, és vcpkg-t csomagkezelőnek.

Felhasznált csomagok:
 - [GLFW](https://www.glfw.org/) (Zlib License) - Ablakkezelő.
 - [GLAD](https://github.com/Dav1dde/glad) (MIT License) - OpenGL kötés.
 - [GLM](https://github.com/g-truc/glm) (Happy Bunny / MIT License) - Matematikai könyvtár OpenGL-hez.
 - [Dear ImGui](https://github.com/ocornut/imgui) (MIT License) - Kezelőfelület.
 - [stb_image](https://github.com/nothings/stb) (Public Domain / MIT) - Képbetöltés.
 - [cereal](https://uscilab.github.io/cereal/) (BSD License) - Szerializáció.
 - [Google Test](https://github.com/google/googletest) (BSD License) - Egységtesztelő keretrendszer.

Felhasznált anyagok:
 - [FreeStylized.com](https://freestylized.com/) (Módosított CC0 licenc) - Textúrák és skyboxok

## Fordítás és futtatás

Minden parancshoz legyél a projekt gyökérkönyvtárában (ahol a CMakeLists.txt is van)

#### Cmake és vcpkg beállítása

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="[vcpkg gyökér könyvtár]/scripts/buildsystems/vcpkg.cmake"
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