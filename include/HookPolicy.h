#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace hooks::policy {
struct Sites { std::size_t advance, invoke; };
constexpr Sites Offsets(bool se) { return se ? Sites{0x39B, 0x3C2} : Sites{0x399, 0x3C0}; }
constexpr bool Contains(std::uintptr_t base, std::size_t size, std::uintptr_t address, std::size_t length)
{
    return address>=base && address-base<=size && length<=size-(address-base);
}
inline bool Matches(std::span<const std::uint8_t> data, std::size_t offset, std::span<const std::uint8_t> expected)
{
    return offset<=data.size() && expected.size()<=data.size()-offset &&
        std::equal(expected.begin(),expected.end(),data.begin()+offset);
}
inline bool Valid(std::span<const std::uint8_t> code, Sites sites)
{
    constexpr std::array<std::uint8_t,6> advance{0xff,0x90,0x28,0x01,0,0};
    constexpr std::array<std::uint8_t,3> receiver{0x48,0x8b,0xcb}, zeroFloat{0x0f,0x57,0xc9}, seFloat{0x0f,0x28,0xce}, zeroFormat{0x45,0x33,0xc0};
    constexpr std::array<std::uint8_t,3> loadName{0x48,0x8d,0x15};
    return sites.advance>=6 && sites.invoke>=13 && sites.invoke<=code.size() && code.size()-sites.invoke>=5 &&
        Matches(code,sites.advance,advance) && Matches(code,sites.advance-3,receiver) &&
        Matches(code,sites.advance-6,sites.advance==Offsets(true).advance ? seFloat : zeroFloat) && code[sites.invoke]==0xe8 &&
        Matches(code,sites.invoke-13,zeroFormat) && Matches(code,sites.invoke-10,loadName) &&
        Matches(code,sites.invoke-3,receiver);
}
}
