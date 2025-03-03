# MetaHookSv

This is a porting of [MetaHook](https://github.com/nagist/metahook) for SvEngine (GoldSrc engine modified by Sven Co-op Team),

Mainly to keep you a good game experience in Sven Co-op.

Most of plugins are still compatible with vanilla GoldSrc engine. please check plugin docs for compatibility.

[中文README](READMECN.md)

## Download

[GitHub Release](https://github.com/hzqst/MetaHookSv/releases)

## Risk of VAC ?

There is no VAC ban reported yet.

The binaries or executables of Sven Co-op are not signed with digital signatures thus no integrity check from VAC would be applied for them.

## One Click Installation

1. Download from [GitHub Release](https://github.com/hzqst/MetaHookSv/releases), then unzip it.

2. Run `install-to-SvenCoop.bat`

3. Launch game from shortcut `MetaHook for SvenCoop` or `\SteamLibrary\steamapps\common\Sven Co-op\svencoop.exe`

* Other games follow the same instruction.

* You should have your Steam running otherwise the [SteamAppsLocation](SteamAppsLocation/README.md) will probably not going to find GameInstallDir.

## Manual Installation

1. Download from [GitHub Release](https://github.com/hzqst/MetaHookSv/releases), then unzip it.

2. All required executable and resource files are in `Build` folder, copy [whatever you want](Build/README.md) to `\SteamLibrary\steamapps\common\Sven Co-op\`.

3. Go to `\SteamLibrary\steamapps\common\Sven Co-op\svencoop\metahook\configs\`, rename `plugin_svencoop.lst` (or `plugin_goldsrc.lst`) to `plugin.lst` (depending on the engine you are going to run)

4. Launch game from Steam Game Library or `\SteamLibrary\steamapps\common\Sven Co-op\svencoop.exe`

* Launch game with launch parameter `-game (gamename)` if you are going to play other games than Sven Co-op, e.g. `svencoop.exe -game valve` or `svencoop.exe -game cstrike`. Renaming `svencoop.exe` to something like `cstrike.exe` also works.

* The new `svencoop.exe` is renamed from `metahook.exe`. You could run game from "metahook.exe -game svencoop" however it will cause game crash when changing video settings.

* Plugins can be disabled or enabled in `\SteamLibrary\steamapps\common\Sven Co-op\svencoop\metahook\configs\plugins.lst`

* The `SDL2.dll` fixes a bug that the IME input handler from original SDL library provided by Valve was causing buffer overflow and game crash when using non-english IME.

## Build Requirements

1. [Visual Studio 2017 / 2019 / 2022, with vc141 / vc142 / vc143 toolset](https://visualstudio.microsoft.com/)

2. [CMake](https://cmake.org/download/)

3. [Git for Windows](https://gitforwindows.org/)

## Build Instruction

Let's assume that you have all requirements installed correctly.

1. `git clone https://github.com/hzqst/MetaHookSv` to somewhere that doesn't contain space in the directory path.

2. Run `build-initdeps.bat`, wait until all required submodules / dependencies are pulled. (this may takes couple of minutes, depending on your network connection and download speed)

3. Run `build-MetaHook.bat`, wait until `svencoop.exe` generated at `Build` directory.

4. Run `build-(SpecifiedPluginName).bat`, wait until `(SpecifiedPluginName).dll` generated. Current available plugins : CaptionMod, Renderer, StudioEvents, SteamScreenshots, SCModelDownloader, CommunicationDemo, DontFlushSoundCache.

8. If generated without problem, plugins should be at `Build\svencoop\metahook\plugins\` directory.

## Debugging

1. `git clone https://github.com/hzqst/MetaHookSv` to somewhere that doesn't contain space in the directory path.

2. Run `build-initdeps.bat`, wait until all required submodules / dependencies are pulled.  (Ignore if you have already done this before) (this may takes couple of minutes, depending on your network connection and download speed)

3. Run `debug-SvenCoop.bat` (or `debug-(WhateverGameYouWant).bat`, depends on which you are going to debug with)

4. Open `MetaHook.sln` with Visual Studio IDE, set specified project as launch project, compile the project, then press F5 to start debugging.

* Other games follow the same instruction.

* You should restart Visual Studio IDE to apply changes to debugging profile if Visual Studio IDE was running.

## MetaHookSv (V3) new features compare to nagist's old metahook (V2)

1. New and better capstone API to disassemble and analyze engine code, new API to reverse search the function begin.

2. Blocking duplicate plugins (which may introduce infinite-recursive calling) from loading.

3. A transaction will be there at stage `LoadEngine` and `LoadClient`, to prevent `InlineHook` issued by multiple plugins from immediately taking effect. Allowing multiple plugins to `SearchPattern` and `InlineHook` same function without conflict.

## Load Order

1. MetaHook launcher always loads plugins listed in `\(GameDirectory)\metahook\configs\plugins.lst` in ascending order. Plugin name started with ";" will be ignored.

2. (PluginName)_AVX2.dll will be loaded if exists only when AVX2 instruction set supported.

3. (PluginName)_AVX.dll will be loaded if exists only when AVX instruction set supported and (2) fails.

4. (PluginName)_SSE2.dll will be loaded if exists only when SSE2 instruction set supported and (3) fails

5. (PluginName)_SSE.dll will be loaded if exists only when SSE instruction set supported and (4) fails.

6. (PluginName).dll will be loaded if (2) (3) (4) (5) fails.

## Plugins

### CaptionMod

A subtitle plugin designed for displaying subtitles and translate in-game HUD text.

[DOCUMENTATION](CaptionMod.md) [中文文档](CaptionModCN.md)

![](/img/1.png)

### BulletPhysics

A plugin that transform player model into ragdoll when player is dead or being caught by barnacle.

[DOCUMENTATION](BulletPhysics.md) [中文文档](BulletPhysicsCN.md)

![](/img/6.png)

### MetaRenderer

A graphic enhancement plugin that modifiy the original render engine.

You can even play with 200k epolys models and still keep a high framerate.

[DOCUMENTATION](Renderer.md) [中文文档](RendererCN.md)

![](/img/3.png)

### StudioEvents

This plugin can block studio-event sound spamming with controllable cvars.

[DOCUMENTATION](StudioEvents.md) [中文文档](StudioEventsCN.md)

![](/img/8.png)

### SteamScreenshots (Sven Co-op only)

This plugin intercepts `snapshot` command and replace it with `ISteamScreenshots` interface which will upload the snapshot to Steam Screenshot Manager.

### SCModelDownloader (Sven Co-op only)

This plugin downloads missing player models from https://wootguy.github.io/scmodels/ automatically and reload them once mdl files are ready.

Cvar : `scmodel_autodownload 0 / 1` Automatically download missing model from scmodel database.

Cvar : `scmodel_downloadlatest 0 / 1` Download latest version of this model if there are multiple ones with different version.

Cvar : `scmodel_usemirror 0 / 1` Use mirror (cdn.jsdelivr.net) if github is not available.

### CommunicationDemo (Sven Co-op only)

This plugin exposes an interface to communicate with Sven Co-op server.

### DontFlushSoundCache (Sven Co-op only) (Experimental)

* NOT READY FOR NON-DEVs

This plugin prevents client from flushing soundcache at `retry` (engine issues `retry` command everytime when HTTP download progress is finished), make it possible to preserve soundcache txt downloaded from fastdl resource server (sv_downloadurl).

The fastdl procedure only works when game server uploads current map's soundcache txt to the fastdl resource server. (I am using AliyunOSS)

The reason why I made this plugin is because transfering soundcache txt via UDP netchannel is really not a good idea as server bandwidth and file IO resource is expensive.

### ABCEnchance (third-party) (Sven Co-op only)

ABCEnchance is a metahook plugin that provides experience improvement for Sven co-op.

1. CSGO style health and ammo HUD
2. Annular weapon menu
3. Dynamic damage indicator 
4. Dynamic crosshair
5. Minimap which is rendered at real time
6. Teammate health, armor and name display with floattext.
7. Some useless blood efx

https://github.com/DrAbcrealone/ABCEnchance

### HUDColor (third-party) (Sven Co-op only)

Changing HUD colors in game.

https://github.com/DrAbcrealone/HUDColor

### MetaAudio (third-party) (GoldSrc only)

This is a plugin for GoldSrc that adds OpenAL support to its sound system. This fork fixes some bugs and uses Alure instead of OpenAL directly for easier source management.

Since SvEngine uses FMOD as it's sound system, you really shouldn't use this plugin in Sven Co-op.

https://github.com/LAGonauta/MetaAudio

* MetaAudio blocks goldsrc engine's sound system and replaces with it's own sound engine. You should always put `MetaAudio.dll` on top of any other plugins that rely on goldsrc engine's sound system (i.e CaptionMod) in the `plugins.lst` to prevent those plugins from being blocked by MetaAudio.

### Trinity Renderer (third-party) (GoldSrc only)

This is a port of the original Trinity Renderer for metahook so it can work in Counter Strike 1.6

https://github.com/ollerjoaco/MH_TrinityRender