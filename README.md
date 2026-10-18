# Infinity UI 2.1.3 - Universal SE/AE

Infinity UI loads external SWF replacements into Skyrim menus and exposes
notifications for dependent UI mods. Original project: [alexsylex/InfinityUI](https://github.com/alexsylex/InfinityUI).
Compatibility update prepared by DeadOnKeyboard1.

## Installation

Install the binary ZIP with Vortex or MO2 and enable it. Do not keep another
`InfinityUI.dll` active. The ZIP provides the native plugin; install the UI mods
and assets that depend on Infinity UI separately. No additional original
Infinity UI DLL is required. There is no mandatory INI: the plugin uses defaults
when `Data/SKSE/Plugins/InfinityUI.ini` is absent.

Use the SKSE build and regular Address Library database matching your exact
Skyrim executable. Skyrim **1.7.104.0 / SKSE 2.3.1** is the primary target and is
confirmed working by the user for this exact DLL. **1.5.97.0 / SKSE 2.0.20** and
**1.6.1170.0 / SKSE 2.2.8** are statically verified and still require in-game
tests. GOG **1.6.1179.0 / SKSE 2.2.6** has build, library and interface checks;
its matching game EXE was unavailable for native verification. Other original
SE/AE branches remain enabled but are not individually user-confirmed. VR is
not included. Compatibility uses Address Library IDs with strict runtime-aware
hook guards, not an exact-version lock.

## Build

Use x64 MSVC with C++23 support (tested: MSVC 19.51, Windows SDK 10.0.26100),
CMake 3.31+, Ninja, Git and vcpkg. The manifest pins dependency resolution;
the tested build used fmt 12.2.0, spdlog 1.17.0 and Xbyak 7.42. Xbyak can be
supplied with `INFINITY_XBYAK_SOURCE_DIR` instead of the vcpkg package.
For the tested Xbyak headers, check out `b9f2bc7e40913cc539b199936caf5487c158831b`
from [herumi/xbyak](https://github.com/herumi/xbyak) and set `XBYAK_SOURCE` to it.

Clone [CommonLibSSE-NG](https://github.com/alandtse/CommonLibSSE-NG) separately
and check out **v12.2.0 / `67b201b6fe45241ac5c1e4630ecf847523775b66`**.
Configure from an x64 MSVC developer terminal, setting the following environment
variables to your own checkouts before running these commands:

```powershell
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release `
  "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  -DVCPKG_TARGET_TRIPLET=x64-windows-static-md `
  "-DINFINITY_COMMONLIB_SOURCE_DIR=$env:COMMONLIB_SOURCE" `
  "-DINFINITY_XBYAK_SOURCE_DIR=$env:XBYAK_SOURCE" `
  -DINFINITY_BUILD_TESTS=ON
cmake --build build/release --target InfinitySdkHeaderProbe
cmake --build build/release --target InfinityUI InfinityPolicyTests InfinityValueTests
ctest --test-dir build/release --output-on-failure
```

The project builds CommonLib from source with SE+AE enabled and VR disabled;
prebuilt all-runtime SDK bundles are not used. CommonLib fetches its pinned
MinHook v1.3.4 instruction decoder. Dependency checkouts are not vendored here.
The optional workspace shared-build integration remains available, but it is
not needed for the standalone route above.

Other dependency source refs are [fmt 12.2.0](https://github.com/fmtlib/fmt/tree/12.2.0),
[spdlog v1.17.0](https://github.com/gabime/spdlog/tree/v1.17.0),
[DirectXTK may2026](https://github.com/microsoft/DirectXTK/tree/may2026),
[DirectXMath jun2026](https://github.com/microsoft/DirectXMath/tree/jun2026), and
[MinHook v1.3.4](https://github.com/TsudaKageyu/minhook/tree/v1.3.4).
The pinned vcpkg baseline also supplies the applicable port patches.

The supplied DLL is the byte-identical user-tested artifact. Rebuilding creates
a new candidate that needs its own game test. Generated PDBs are local debug
artifacts: do not publish them without independently validating their paths.

## License

The combined update is **GPL-3.0-or-later**, with the exact CommonLib additional
permissions in `CommonLibSSE-NG-EXCEPTIONS.md`. Required original MIT/BSD notices
are retained in `NOTICE.md`; upstream authorship is unchanged. Distribute the
matching source archive alongside the binary, retaining the pinned dependency
source links and build instructions. Nothing has been uploaded automatically.
