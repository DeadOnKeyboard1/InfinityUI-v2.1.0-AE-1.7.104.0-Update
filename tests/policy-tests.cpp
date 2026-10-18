#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <intrin.h>
#include "HookPolicy.h"
#include "HookCode.h"
#include "PatchFiles.h"
#include "ValuePolicy.h"

namespace {
unsigned checks{};
void Check(bool value,const char* message) {
    ++checks;
    if (!value) { std::cerr<<message<<'\n'; std::exit(1); }
}
void* bridge{};
void* receivedRoot{};
void* receivedMenu{};
float receivedDelta{};
std::uint32_t receivedCount{};
bool aligned{};
extern "C" __declspec(noinline) std::uintptr_t Callback(void* root,float delta,std::uint32_t count,void* menu) {
    receivedRoot=root; receivedDelta=delta; receivedCount=count; receivedMenu=menu;
    aligned=(reinterpret_cast<std::uintptr_t>(_AddressOfReturnAddress())&15)==8;
    return 0x12345678;
}
}

int main() {
    using namespace IUI;
    for (bool se:{false,true}) {
        const auto sites=hooks::policy::Offsets(se);
        std::vector<std::uint8_t> code(sites.invoke+5);
        const std::array<std::uint8_t,6> advance{0xff,0x90,0x28,1,0,0};
        const std::array<std::uint8_t,3> receiver{0x48,0x8b,0xcb},zero{0x0f,se ? std::uint8_t{0x28} : std::uint8_t{0x57},se ? std::uint8_t{0xce} : std::uint8_t{0xc9}},format{0x45,0x33,0xc0},name{0x48,0x8d,0x15};
        std::copy(advance.begin(),advance.end(),code.begin()+sites.advance);
        std::copy(receiver.begin(),receiver.end(),code.begin()+sites.advance-3);
        std::copy(zero.begin(),zero.end(),code.begin()+sites.advance-6);
        std::copy(format.begin(),format.end(),code.begin()+sites.invoke-13);
        std::copy(name.begin(),name.end(),code.begin()+sites.invoke-10);
        std::copy(receiver.begin(),receiver.end(),code.begin()+sites.invoke-3);
        code[sites.invoke]=0xe8;
        Check(hooks::policy::Valid(code,sites),"Valid native call policy rejected");
        code[sites.advance-5]=se ? 0x57 : 0x28;
        code[sites.advance-4]=se ? 0xc9 : 0xce;
        Check(!hooks::policy::Valid(code,sites),"Wrong runtime's float argument instruction accepted");
        std::copy(zero.begin(),zero.end(),code.begin()+sites.advance-6);
        for (auto position:{sites.advance,sites.advance+2,sites.advance-3,sites.advance-6,sites.invoke,sites.invoke-13,sites.invoke-10,sites.invoke-3}) {
            code[position]^=0x80;
            Check(!hooks::policy::Valid(code,sites),"Changed instruction/argument setup accepted");
            code[position]^=0x80;
        }
        Check(!hooks::policy::Valid(std::span(code.data(),sites.invoke),sites),"Truncated instruction span accepted");
        for (std::size_t tail=1;tail<5;++tail)
            Check(!hooks::policy::Valid(std::span(code.data(),sites.invoke+tail),sites),"Partial CALL displacement accepted");
    }
    Check(hooks::policy::Contains(10,20,10,20),"Valid image range rejected");
    Check(!hooks::policy::Contains(10,20,9,1) && !hooks::policy::Contains(10,20,25,6),"Out-of-range image memory accepted");
    Check(!hooks::policy::Contains(10,20,UINTPTR_MAX,1),"Overflowing image address accepted");
    Check(PatchFiles::MovieName("Interface/Exported/HUDMenu.swf")=="HUDMenu","Movie URL basename extraction failed");
    Check(PatchFiles::MovieName("Interface\\HUDMenu.swf")=="HUDMenu","Windows movie URL separator failed");
    Check(PatchFiles::MovieName("Interface/HUDMenu.custom.swf")=="HUDMenu.custom","Dotted movie name truncated");
    Check(PatchFiles::MovieName("../").empty(),"Empty movie filename accepted");
    Check(PatchFiles::Exported("INTERFACE\\EXPORTED\\HUDMenu.swf"),"Exported movie path normalization failed");
    Check(!PatchFiles::Exported("Interface/HUDMenu.swf"),"Normal movie falsely exported");
    Check(PatchFiles::MemberPath("root","root/a/b.SWF")=="a.b","Uppercase SWF member path failed");
    Check(PatchFiles::MemberPath("root","other/a.swf").empty(),"Parent traversal accepted as AS member path");
    Check(PatchFiles::MemberName("a.b")=="b" && PatchFiles::ParentName("a.b")=="a","Nested member extraction failed");
    Check(PatchFiles::ParentName("a")=="_root","Top-level movie parent changed");
    Check(MovieClipDepth(0)==0 && MovieClipDepth(42)==42,"Valid movie depth rejected");
    for (double number:{-1.,.5,2147483648.,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()})
        Check(!MovieClipDepth(number),"Invalid numeric movie depth accepted");

    Check(ValidLogLevel(0,6) && ValidLogLevel(6,6),"Valid log severity rejected");
    Check(!ValidLogLevel(7,6) && !ValidLogLevel(UINT32_MAX,6),"Invalid log severity accepted");
    const auto parent=std::filesystem::absolute("build/ng12/test-fixtures");
    const auto fixture=parent/("infinity-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if (!std::filesystem::create_directories(fixture)) { std::cerr<<"Fixture already exists\n"; return 1; }
    const auto expectedParent=std::filesystem::weakly_canonical(parent);
    std::filesystem::create_directories(fixture/"a");
    std::ofstream(fixture/"a/b.swf")<<"fixture";
    std::ofstream(fixture/"a/c.SWF")<<"fixture";
    std::ofstream(fixture/"a.swf")<<"fixture";
    std::ofstream(fixture/"z.swf")<<"fixture";
    std::ofstream(fixture/"ignore.txt")<<"fixture";
    std::vector<std::filesystem::path> files;
    unsigned errors{};
    PatchFiles::Visit(fixture,[&](const auto& file) { files.push_back(file); },[&](auto) { ++errors; });
    Check(files.size()==4 && errors==0,"Patch traversal includes wrong files or fails on ordinary folders");
    Check(files==std::vector<std::filesystem::path>{fixture/"a.swf",fixture/"z.swf",fixture/"a/b.swf",fixture/"a/c.SWF"},
          "Patch traversal is not deterministic and parent-before-child");
    PatchFiles::Visit(fixture/"missing",[](const auto&) {},[&](auto) { ++errors; });
    Check(errors==1,"Missing optional path must report an error without throwing");
    std::error_code symlinkError;
    std::filesystem::create_directory_symlink(fixture,fixture/"a/loop",symlinkError);
    if (!symlinkError) {
        errors=0; files.clear();
        PatchFiles::Visit(fixture,[&](const auto& file) { files.push_back(file); },[&](auto) { ++errors; });
        Check(files.size()==4 && errors==1,"Directory link cycle was not bounded");
    } else std::cout<<"Directory symlink fixture skipped: privilege unavailable\n";
    Check(std::filesystem::weakly_canonical(fixture).parent_path()==expectedParent,"Fixture cleanup escaped its dedicated directory");
    std::filesystem::remove_all(fixture);

    Xbyak::CodeGenerator caller;
    Xbyak::Label resume;
    const std::uintptr_t root=0x11223344,menu=0x55667788,sentinel=0x10203040;
    caller.push(caller.rdi);
    caller.sub(caller.rsp,0x40);
    caller.mov(caller.qword[caller.rsp+0x20],static_cast<std::uint32_t>(sentinel));
    caller.mov(caller.rdi,menu); caller.mov(caller.rcx,root);
    caller.mov(caller.eax,0x3fa00000); caller.movd(caller.xmm1,caller.eax);
    caller.mov(caller.r8d,2); caller.mov(caller.r9,0xdeadbeef);
    caller.mov(caller.rax,reinterpret_cast<std::uintptr_t>(&bridge)); caller.jmp(caller.ptr[caller.rax]);
    caller.L(resume);
    caller.mov(caller.rdx,caller.qword[caller.rsp+0x20]);
    caller.cmp(caller.rdx,static_cast<std::uint32_t>(sentinel));
    Xbyak::Label intact;
    caller.je(intact); caller.xor_(caller.eax,caller.eax); caller.L(intact);
    caller.add(caller.rsp,0x40); caller.pop(caller.rdi); caller.ret(); caller.ready();
    hooks::CallWithMenu stub{reinterpret_cast<std::uintptr_t>(&Callback),reinterpret_cast<std::uintptr_t>(resume.getAddress())};
    bridge=const_cast<std::uint8_t*>(stub.getCode());
    const auto result=caller.getCode<std::uintptr_t(*)()>()();
    Check(result==0x12345678,"Callback return or engine caller's fifth stack slot corrupted");
    Check(aligned && receivedRoot==reinterpret_cast<void*>(root) && receivedMenu==reinterpret_cast<void*>(menu) &&
        receivedDelta==1.25f && receivedCount==2,"Hook bridge register, float, menu or stack alignment mismatch");
    std::cout<<"InfinityUI policy/native bridge tests passed: "<<checks<<" checks\n";
}
