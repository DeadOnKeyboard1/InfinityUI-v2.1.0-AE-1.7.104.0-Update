#pragma once
#include <cstdint>
#include <xbyak/xbyak.h>

namespace hooks {
class CallWithMenu final : public Xbyak::CodeGenerator {
public:
    CallWithMenu(std::uintptr_t callback, std::uintptr_t continuation)
    {
        Xbyak::Label hookLabel, returnLabel;
        // The replaced call site is 16-byte aligned. Give our four-argument
        // callback its own shadow space; the engine's caller frame stays intact.
        sub(rsp, 0x20);
        mov(r9, rdi);
        call(ptr[rip + hookLabel]);
        add(rsp, 0x20);
        jmp(ptr[rip + returnLabel]);
        L(hookLabel); dq(callback);
        L(returnLabel); dq(continuation);
        ready();
    }
};
}
