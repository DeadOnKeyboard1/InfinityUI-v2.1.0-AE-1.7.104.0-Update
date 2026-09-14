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

This update is distributed under GPL-3.0-or-later as stated in `LICENSE`, with
the applicable CommonLibSSE-NG exceptions preserved in
`licenses/CommonLibSSE-NG-EXCEPTIONS.md`. The original Infinity UI source and
copyright notice remain available under their original MIT terms in
`licenses/InfinityUI-MIT.txt`. Third-party components retain their respective
licenses; see `THIRD_PARTY_NOTICES.md`.
