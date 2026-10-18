#pragma once
#include <algorithm>
#include <filesystem>
#include <deque>
#include <string>
#include <string_view>
#include <vector>

namespace IUI::PatchFiles {
inline std::string Utf8(const std::filesystem::path& path)
{
    const auto text=path.generic_u8string();
    return {text.begin(),text.end()};
}
inline std::string MovieName(std::string_view url)
{
    const auto separator=url.find_last_of("/\\");
    auto name=url.substr(separator==url.npos ? 0 : separator+1);
    return std::string(name.substr(0,name.rfind('.')));
}
inline bool Exported(std::string_view url)
{
    std::string normalized(url);
    for (auto& c:normalized) {
        if (c=='\\') c='/';
        if (c>='A' && c<='Z') c+= 'a'-'A';
    }
    return normalized.find("interface/exported/")!=normalized.npos;
}
inline bool IsSwf(const std::filesystem::path& path)
{
    auto extension=Utf8(path.extension());
    for (auto& c:extension) if (c>='A' && c<='Z') c+= 'a'-'A';
    return extension==".swf";
}
inline std::string MemberPath(const std::filesystem::path& root,const std::filesystem::path& file)
{
    const auto relative=file.lexically_relative(root);
    if (relative.empty() || !IsSwf(relative)) return {};
    for (const auto& part:relative) if (part=="..") return {};
    auto path=relative; path.replace_extension();
    auto name=Utf8(path);
    std::replace(name.begin(),name.end(),'/','.');
    return name;
}
inline std::string MemberName(std::string_view path)
{
    const auto dot=path.rfind('.');
    return std::string(path.substr(dot==path.npos ? 0 : dot+1));
}
inline std::string ParentName(std::string_view path)
{
    const auto dot=path.rfind('.');
    return dot==path.npos ? "_root" : std::string(path.substr(0,dot));
}
template<class File,class Error>
void Visit(const std::filesystem::path& root,File&& file,Error&& report)
{
    struct Entry { std::filesystem::path path; std::vector<std::filesystem::path> ancestors; };
    std::deque<Entry> pending; pending.push_back({root,{}});
    while (!pending.empty()) {
        auto entry=std::move(pending.front()); pending.pop_front();
        std::error_code error;
        const auto status=std::filesystem::status(entry.path,error);
        if (error) { report(error); continue; }
        if (std::filesystem::is_directory(status)) {
            const auto canonical=std::filesystem::weakly_canonical(entry.path,error);
            if (error) { report(error); continue; }
            if (std::find(entry.ancestors.begin(),entry.ancestors.end(),canonical)!=entry.ancestors.end()) {
                report(std::make_error_code(std::errc::too_many_symbolic_link_levels)); continue;
            }
            entry.ancestors.push_back(canonical);
            std::filesystem::directory_iterator it(entry.path,std::filesystem::directory_options::skip_permission_denied,error),end;
            if (error) { report(error); continue; }
            std::vector<std::filesystem::path> children;
            while (it!=end) {
                children.push_back(it->path());
                it.increment(error);
                if (error) { report(error); break; }
            }
            // A parent clip replacement must precede patches below that clip.
            std::sort(children.begin(),children.end());
            for (auto& child:children) pending.push_back({std::move(child),entry.ancestors});
        } else if (std::filesystem::is_regular_file(status) && IsSwf(entry.path)) file(entry.path);
    }
}
}
