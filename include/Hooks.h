#pragma once
#include "utils/Trampoline.h"
#include "HookCode.h"
#include "HookPolicy.h"

namespace hooks {
class GFxMovieRoot {
public:
    // The native direct call is a variadic Invoke wrapper, not InvokeArgs.
    static inline REL::Relocation<const char*(*)(RE::GFxMovieRoot*, const char*, const char*, ...)> Invoke;
};
void PatchGFxMovie(RE::GFxMovieRoot*, float, std::uint32_t, RE::IMenu*);
void InitExtensionsAndNotify(RE::GFxMovieRoot*, const char*, const char*, RE::IMenu*);

inline bool Install()
{
    const auto& module=REL::Module::get();
    const auto text=module.segment(REL::Segment::textx);
    const auto data=module.segment(REL::Segment::rdata);
    const auto base=REL::Relocation<std::uintptr_t>{REL::RelocationID{80302,82325}}.address();
    const auto sites=policy::Offsets(REL::Module::IsSE());
    const auto length=sites.invoke+5;
    if (!policy::Contains(text.address(),text.size(),base,length)) return false;
    const auto code=std::span(reinterpret_cast<const std::uint8_t*>(base),length);
    if (!policy::Valid(code,sites)) {
        SKSE::log::error("InfinityUI hook instruction/argument validation failed; no patches installed");
        return false;
    }
    std::int32_t nameDisplacement{};
    std::memcpy(&nameDisplacement,code.data()+sites.invoke-7,sizeof(nameDisplacement));
    const auto nameAddress=base+sites.invoke-3+nameDisplacement;
    constexpr char method[]="_root.InitExtensions";
    if (!policy::Contains(data.address(),data.size(),nameAddress,sizeof(method)) ||
        std::memcmp(reinterpret_cast<const void*>(nameAddress),method,sizeof(method))) return false;
    std::int32_t callDisplacement{};
    std::memcpy(&callDisplacement,code.data()+sites.invoke+1,sizeof(callDisplacement));
    const auto original=base+sites.invoke+5+callDisplacement;
    if (!policy::Contains(text.address(),text.size(),original,16)) return false;

    const CallWithMenu advanceCode{reinterpret_cast<std::uintptr_t>(&PatchGFxMovie),base+sites.advance+6};
    const CallWithMenu invokeCode{reinterpret_cast<std::uintptr_t>(&InitExtensionsAndNotify),base+sites.invoke+5};
    Hook<6> advanceHook{base+sites.advance,advanceCode};
    Hook<5> invokeHook{base+sites.invoke,invokeCode};
    DefaultTrampoline trampoline{advanceHook.getSize()+invokeHook.getSize()};
    GFxMovieRoot::Invoke=original;
    trampoline.write_branch(advanceHook);
    trampoline.write_branch(invokeHook);
    return true;
}
}
