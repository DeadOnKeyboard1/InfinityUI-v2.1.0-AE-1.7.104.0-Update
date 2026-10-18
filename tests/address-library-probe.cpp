#include "SKSE/Impl/PCH.h"
#include "REL/IDDB.h"
#include "REL/Offset2ID.h"
#include <iostream>

int wmain(int argc,wchar_t** argv)
{
    if (argc<7) return 2;
    REL::Version::value_type version[4]{};
    for (unsigned i=0;i<4;++i) {
        wchar_t* end{};
        const auto value=std::wcstoul(argv[i+2],&end,10);
        if (!end || end==argv[i+2] || *end || value>UINT16_MAX) return 2;
        version[i]=static_cast<REL::Version::value_type>(value);
    }
    if (!REL::IDDB::inject(argv[1],REL::Version{version[0],version[1],version[2],version[3]})) return 3;
    std::map<std::uint64_t,std::uint64_t> wanted;
    for (int i=6;i<argc;++i) {
        wchar_t* end{};
        const auto id=std::wcstoull(argv[i],&end,10);
        if (!end || end==argv[i] || *end) return 2;
        wanted.emplace(id,0);
    }
    const REL::Offset2ID mappings;
    for (const auto& mapping:mappings) {
        if (auto it=wanted.find(mapping.id);it!=wanted.end()) {
            if (it->second && it->second!=mapping.offset) return 4;
            it->second=mapping.offset;
        }
    }
    std::cout<<"{\"mapped_entries\":"<<mappings.size()<<",\"offsets\":{";
    bool first=true;
    for (const auto& [id,offset]:wanted) {
        if (!first) std::cout<<',';
        first=false;
        if (offset && REL::IDDB::get().id2offset(id)!=offset) return 4;
        std::cout<<'"'<<id<<"\":"<<offset;
    }
    std::cout<<"}}\n";
    REL::IDDB::reset();
}
