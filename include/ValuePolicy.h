#pragma once
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>

namespace IUI {
constexpr bool ValidLogLevel(std::uint32_t value, std::uint32_t off)
{
    return value <= off;
}

inline std::optional<std::int32_t> MovieClipDepth(double value)
{
    if (!std::isfinite(value) || value<0 || value>std::numeric_limits<std::int32_t>::max() || std::trunc(value)!=value)
        return std::nullopt;
    return static_cast<std::int32_t>(value);
}
}
