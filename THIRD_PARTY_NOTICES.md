# Third-Party Notices

This file records the dependencies relevant to the Infinity UI source and the
tested Skyrim 1.7.104.0 binary distribution.

## CommonLibSSE-NG provenance

The source tree's vcpkg overlay pins:

- Repository: https://github.com/alandtse/CommonLibSSE-NG
- Source commit: `6962ef2cd81afcdd0a35d0d23ac3d30191814092`
- Overlay date: `14/08/25`
- License declared by that historical port: MIT

The already-tested release DLL was not built from that pinned overlay. Its CMake
cache points to an external vcpkg installation, and its PDB records a
CommonLibSSE-NG build-tree source directory beginning with `b2f24ebf25`. The
available artifacts do not contain the remaining commit digits, so the exact
full revision cannot be recovered without rebuilding.

Because that tested binary uses the newer CommonLibSSE-NG licensing generation,
the release includes the current CommonLibSSE-NG GPL-3.0-or-later text and its
Modding and Linking Exceptions, plus the historical CommonLib MIT notice. This
preserves the tested DLL while providing the relevant notices conservatively.

## Components

| Component | License | Notice file |
| --- | --- | --- |
| Infinity UI update | GPL-3.0-or-later | `LICENSE` |
| Original Infinity UI source | MIT | `licenses/InfinityUI-MIT.txt` |
| CommonLibSSE-NG (tested binary) | GPL-3.0-or-later with exceptions | `licenses/CommonLibSSE-NG-GPL-3.0-or-later.txt`, `licenses/CommonLibSSE-NG-EXCEPTIONS.md` |
| CommonLibSSE-NG (historical pinned source) | MIT | `licenses/CommonLibSSE-NG-MIT.txt` |
| OpenVR | BSD-3-Clause | `licenses/OpenVR-BSD-3-Clause.txt` |
| DirectXMath | MIT | `licenses/DirectXMath.txt` |
| DirectX Tool Kit | MIT | `licenses/DirectXTK.txt` |
| fmt | MIT | `licenses/fmt.txt` |
| rapidcsv | BSD-3-Clause | `licenses/rapidcsv.txt` |
| spdlog | MIT | `licenses/spdlog.txt` |
| Xbyak | BSD-3-Clause | `licenses/Xbyak-BSD-3-Clause.txt` |

All names and copyrights remain the property of their respective owners.
