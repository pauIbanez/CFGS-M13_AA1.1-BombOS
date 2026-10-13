# AA1.1 BombOS

Welcome to the BombOS repo, a CFGS exercise.

BombOS is a C++23 console app that reads Bomberman-style level files. It supports two formats, `bos` and `pbos`. See [Level formats](#level-formats).

> **AI:** This repo used AI in the generation of this README, and as a general aid. it was `NOT` used for Debugging, code gen or design of the program.

> **Note:** You can safely IGNORE `PBOS`. Readers are decoupled and independent. `BOS` is fully supported.

### Project structure

```
├── assets/          Level files
├── include/         Public headers
├── release/         Windows icon resources
├── scripts/         Build / dev / release helpers
├── src/
│   ├── main.cpp
│   ├── reader/      Reader (format detection) + BOS and PBOS readers
│   └── version/     Version parsing
├── CMakeLists.txt
└── CMakePresets.json
```

### Requirements

To build and run this app you need:

- [CMake](https://cmake.org/) 3.25 or newer
- [Ninja](https://ninja-build.org/) (the presets use the Ninja generator)
- Visual Studio Build Tools (MSVC)
- bash, e.g. Git Bash (optional, to run the helper scripts)
- [watchexec](https://github.com/watchexec/watchexec) (optional, only for `watch.sh`)

### How to compile

You need the MSVC environment variables loaded before you compile. The easiest way is to open a **Developer Command Prompt for VS**.

From that prompt, go to the repo root and run the bash scripts in the [scripts](./scripts) folder:

| Script                                    | What it does                                                                                                              |
| ----------------------------------------- | ------------------------------------------------------------------------------------------------------------------------- |
| [build.sh](./scripts/build.sh) `<preset>` | Configures and builds the given preset (`dev` or `release`). For `dev`, it also symlinks `assets/` into the build output. |
| [dev.sh](./scripts/dev.sh)                | Builds the `dev` preset and runs `build/dev/bin/BombOS.exe`.                                                              |
| [watch.sh](./scripts/watch.sh)            | Watches `src/` and `include/` and re-runs `dev.sh` on every change. Pass `-n` to rebuild without running the app.         |
| [release.sh](./scripts/release.sh)        | Clean builds the `release` preset and installs it to `out/BombOS`.                                                        |

```bash
bash ./scripts/dev.sh        # build + run (debug)
bash ./scripts/watch.sh      # rebuild on save
bash ./scripts/release.sh    # build + package a release
```

You can also call CMake directly:

```bash
cmake --preset dev
cmake --build --preset dev
cmake --install build/release --prefix out/BombOS
```

#### Presets

| Preset    | Build type     | Output                                         |
| --------- | -------------- | ---------------------------------------------- |
| `dev`     | Debug          | `build/dev/bin`                                |
| `release` | RelWithDebInfo | `build/release/bin`, installed to `out/BombOS` |

### Running a release

After `release.sh`, the app is packaged like this:

```
out/BombOS/
├── BombOS.exe
└── assets/
    ├── level1.bos
    └── level1_p.bos
```

The app loads levels from `./assets/`, a path relative to the **current working directory**. To run it, either double-click `BombOS.exe` or `cd out/BombOS` first.

Do not run `build/release/bin/BombOS.exe` directly. Assets are only copied on install, so that folder has none.

### Assets

Level files live in [assets](./assets). In dev builds, `build.sh` links `build/dev/bin/assets` to this folder so your edits show up without rebuilding. Release installs copy the folder instead.

> **Note:** Git Bash's `ln -s` makes a copy instead of a real symlink unless native symlinks are enabled (set `MSYS=winsymlinks:nativestrict` and turn on Windows Developer Mode). If your asset edits don't show up in dev, delete `build/dev/bin/assets` and rebuild.

### Level formats

Both formats are plain text. Fields are separated by `;` and every line ends with `;;`.

#### BOS ([level1.bos](./assets/level1.bos))

```
BOS;0.5;;              format + version
TITLE;Nivel 1;;
TEXTURES;6;;           number of textures, then one "id;file" line each
a;verde_oscuro.png;;
SIZE;6;6;;             height;width
FLOOR;;                height rows of texture ids
MAP;;                  height rows of texture ids (0 = empty)
COLLISIONS;;           height rows of collision types (0, 1, D)
OBJECTS;;              height rows of object ids
```

#### PBOS ([level1_p.bos](./assets/level1_p.bos))

A compact "palette" format. Each palette entry defines a floor texture, a wall texture and a collision type, and the grid uses palette ids:

```
PBOS;0.0.1;;
TITLE;Nivel 1;;
TEXTURES;verde_oscuro.png;...;;
SIZE;6;6;;
PALETTE;;
# id;floor;wall;collision;;
wa;verde_oscuro;muro;1;;
GRID;;                 height rows of palette ids
OBJECTS;;
```
