# Piano-FX Pro

A high-performance Black MIDI player built on DirectX 11, forked from Brian Pantano's [Piano From Above](https://github.com/brian-pantano/PianoFromAbove) and extensively rewritten for extreme-scale MIDI playback.

Developed by **happy_mimimix**, with major architectural contributions from **Khangaroo** (note vertex shader, DirectX renderer, MIDI parser redesign).

## Features

### Black MIDI Performance

Piano-FX Pro is purpose-built for Black MIDI — files with millions to billions of notes. The rendering pipeline uses custom HLSL shaders with GPU-side note expansion: each note is a 12-byte struct uploaded to a structured buffer, expanded into quads by the vertex shader, with per-track colors resolved entirely on the GPU. No CPU-side geometry generation, no per-note draw calls.

The MIDI parser uses a pool allocator with 32-byte cache-line-aligned events, a tournament tree for O(log T) track merging, and LZMA decompression for compressed MIDI files. A `LimitedColor` build configuration caps track colors at 256 for even faster rendering on extreme files.

### Pitch Bend Visualization with RPN 0 Support

Piano-FX Pro doesn't just visualize pitch bends — it correctly parses **RPN 0 (Pitch Bend Sensitivity)** events to determine the actual semitone range of each channel's pitch wheel. If a MIDI file sets channel 1's pitch bend range to ±24 semitones via RPN 0, Piano-FX Pro will render those notes sliding across 24 semitones instead of the default ±2.

As of today, only two MIDI visualizers in the world correctly handle Pitch Bend Sensitivity: Piano-FX Pro and MIDI Trail. Of those two, only Piano-FX Pro can handle Black MIDI.

### Dynamic Color Events

In other MIDI players, note colors are decided by the player, not the music's creator. Piano-FX Pro changes this. Creators can embed color events directly in their MIDI files to specify the exact color and transparency of every track and channel.

This is not a static configuration file bundled alongside the MIDI. Color events are processed in real-time during playback, allowing **mid-song color changes** — tracks can shift hues, fade in and out, or become completely invisible at precisely timed moments. This is something no config file can do.

The color event format uses MIDI meta events (Generic Text A) with an 8 or 12 byte payload encoding track, channel, RGBA color, and a hidden flag. Channel `0x7F` targets all channels on a track simultaneously.

### Cheat Engine Integration

Nearly every visual and playback parameter in Piano-FX Pro is exposed as a stable memory address accessible to Cheat Engine. This isn't a debug feature — it's a creative tool designed for MIDI music creators.

What can be controlled:

- Zoom, X/Y offset, and camera position
- Playback position, note speed, playback speed, and volume
- Key range (start note / end note)
- Tick-based vs. time-based rendering
- Equal-width note mode
- Pitch bend visualization toggle
- Velocity-to-alpha mapping
- Marker display
- Frame rate lock (60 FPS)
- Debug info and statistics visibility
- Overlapping note hiding (OR mode)
- Phigros-style scoring system
- Velocity threshold

Many MIDI players offer some of these as user settings. But Piano-FX Pro is the only one that lets the **music's creator** — not the end user — control all of these parameters through Cheat Engine scripts synchronized to the music. Creators can script zoom changes, camera moves, speed ramps, and visual toggles timed to specific moments in their compositions.

Beyond parameter control, Cheat Engine can also be used to apply GDI post-processing effects rendered on top of the DirectX output — another Piano-FX Pro exclusive.

### Video Export

Built-in frame dump pipeline via named pipes to FFmpeg. Renders frames at full quality to a raw video pipe, which FFmpeg encodes to H.264 in real time. No screen capture, no quality loss.

## Building

Piano-FX Pro uses the **Clang-CL** toolset (LLVM) and targets **x64 only**.

### Prerequisites

- Visual Studio 2022 with the **C++ Clang-CL** toolset installed
- Windows SDK

### Standard Build

1. Open `Piano-FX-Pro.sln` in Visual Studio
2. Select your desired configuration and **x64** platform
3. Build

### Automatic Build (Git ≤ 2.45.0)

On Windows with Git 2.45.0 or earlier, cloning with the `--recursive` flag will automatically trigger the build process:

```
git clone --recursive https://github.com/happymimimix/Piano-FX-Pro.git
```

This uses Git hooks via submodules to automate the build. Git 2.45.1 and later patched the mechanism this relies on (CVE-2024-32002), so automatic builds require version 2.45.0 or earlier.

### Build Configurations

**For development**, use **Debug**. It compiles in ~30 seconds, includes full compiler optimizations (performance is identical to Release), and generates PDB files. It excludes FFmpeg, Colorizer, and PFXGDI to keep compile times short. To test a different language or enable a special feature during development, define it in `DebugLanguageOverride.h` — do not switch configurations.

**For debugging**, use **NoOptimization**. This is the traditional debug configuration: all optimizations disabled, `_DEBUG` defined, full safety checks, and all variables visible in the watch window. Also generates PDB files.

**For release**, use the language-specific configurations below. These all include FFmpeg, Colorizer, and PFXGDI. Release does not generate PDB files. A full Release build takes ~5 minutes; building all languages takes ~30 minutes.

| Configuration | Description |
|---|---|
| Debug | Fast compile (~30s), optimized, PDB, no FFmpeg/Colorizer/PFXGDI |
| NoOptimization | Traditional debug, no optimizations, full watch support, PDB |
| Release | English, full optimization, FFmpeg/Colorizer/PFXGDI included |
| LimitedColor | 256 track color cap for extreme Black MIDI performance |
| SoftwareRender | **Forces** WARP software rendering, bypassing the hardware GPU entirely. All configurations already fall back to software rendering automatically if the hardware GPU fails — this configuration exists specifically for cases where the GPU is malfunctioning but the normal fallback still tries to use it. Only use this if your hardware GPU produces broken output and the regular Release build won't switch away from it. |
| Chinese | 中文界面 |
| Japanese | 日本語インターフェース |
| Hiragana | ひらがなインターフェース |
| Korean | 한국어 인터페이스 |
| Russian | Русский интерфейс |
| Arinoglish | Language of the fictional nation of Arino. Derived from Chinese but written entirely in Hanyu Pinyin, solving the problem of complex Chinese characters. |
| DoubleArinoglish | A revised Arinoglish standard (2023) from Arino's National Literature Committee. Uses 33 unique Cyrillic letters to represent all 24 consonants and 33 vowels, replacing multi-character Latin combinations. Each Chinese character now takes only 3 written characters, drastically shortening text length — most visible in the Preferences dialog width. |
| DPRK | Pyongyang standard Korean. Made purely for fun — the author has no affiliation with the DPRK government. |

`DebugLanguageOverride.h` is only compiled in Debug and NoOptimization configurations. It will not affect release builds.

## Credits

- **Brian Pantano** — Original Piano From Above
- **Khangaroo** — Note vertex shader, DirectX renderer architecture, MIDI parser redesign
- **happy_mimimix** — Piano-FX Pro development, Black MIDI optimization, color events, CE integration, pitch bend sensitivity
- **Claude Opus (Anthropic)** — DirectX 11 migration, JumpTo optimization, tournament tree for O(log T) track merging, SWAR keyboard color blending, README documentation
