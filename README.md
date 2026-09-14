# Infinity UI v2.1.0 - Skyrim 1.7.104.0 Update

Source for the Skyrim 1.7.104.0 compatibility update of Infinity UI.

## Requirements

- Skyrim Special Edition / Anniversary Edition runtime 1.7.104.0
- SKSE 2.3.1
- Address Library for SKSE Plugins with the 1.7.104.0 database

This repository contains the native plugin source. The release archive contains
`InfinityUI.dll`, its matching PDB, the configuration file, and the required
license notices.

## Credits

Infinity UI was created by alexsylex. The original project is available at
https://github.com/alexsylex/InfinityUI and the original mod page is at
https://www.nexusmods.com/skyrimspecialedition/mods/74483.

The Skyrim 1.7.104.0 compatibility build was prepared by DeadOnKeyboard1.

## Building

The project uses CMake, Visual Studio 2022, vcpkg, and the custom
`commonlibsse-ng` overlay port in `cmake/ports`.

Set `VCPKG_ROOT` to a working vcpkg checkout, then configure with one of the
presets in `CMakePresets.json`. The dependency manifest is pinned by
`vcpkg.json`; the overlay pins CommonLibSSE-NG source commit
`6962ef2cd81afcdd0a35d0d23ac3d30191814092`.

The tested release binary was produced earlier with an external vcpkg package
whose PDB identifies a different CommonLibSSE-NG source revision by the prefix
`b2f24ebf25`. The complete commit hash is not embedded in the available build
artifacts. See `THIRD_PARTY_NOTICES.md` for the exact provenance record and
license handling.

## License

The Infinity UI project source retains the original MIT license in `LICENSE`.
Third-party components retain their own licenses. The tested binary is
distributed with the CommonLibSSE-NG GPL-3.0-or-later license and exceptions
because it was statically linked with a newer CommonLibSSE-NG build. See the
`licenses` directory and `THIRD_PARTY_NOTICES.md`.
