#include <cstdio>
#include <string>

extern "C" __declspec(dllexport) int InfinityPrivacyProbe(const char* value)
{
    const std::string text = value ? value : "probe";
    return std::printf("%s", text.c_str());
}
