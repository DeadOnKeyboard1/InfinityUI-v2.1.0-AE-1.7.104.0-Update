#include "GFxMoviePatcher.h"
#include "GFxDisplayObject.h"

namespace IUI {
GFxMoviePatcher::GFxMoviePatcher(RE::IMenu* a_menu, RE::GFxMovieRoot* a_movieRoot) :
    menu(a_menu), movieRoot(a_movieRoot)
{
    const auto definition=movieRoot ? movieRoot->GetMovieDef() : nullptr;
    const auto url=definition ? definition->GetFileURL() : nullptr;
    if (!url) return;
    movieRootFileBasename=PatchFiles::MovieName(url);
    if (PatchFiles::Exported(url)) movieRootFilePath/="Exported";
    startPath=std::filesystem::path("Data/Interface/InfinityUI")/movieRootFileBasename;
    logger::trace("Detected {} movie load",movieRootFileBasename);
}

int GFxMoviePatcher::LoadInstancePatches()
{
    if (!menu || !movieRoot || movieRootFileBasename.empty()) return 0;
    std::error_code error;
    if (!std::filesystem::is_directory(startPath,error)) {
        if (error && error!=std::errc::no_such_file_or_directory)
            logger::warn("Optional patch folder unavailable (system error {})",error.value());
        return 0;
    }
    int count{};
    API::DispatchMessage(API::StartLoadInstancesMessage{menu,movieRoot});
    PatchFiles::Visit(startPath,[&](const std::filesystem::path& file) {
        const auto instancePath=PatchFiles::MemberPath(startPath,file);
        const auto parentPath=PatchFiles::ParentName(instancePath);
        if (instancePath.empty() || parentPath.empty()) return;
        const auto relative=PatchFiles::Utf8(file.lexically_relative(movieRootFilePath));
        const auto instanceName=PatchFiles::MemberName(instancePath);
        RE::GFxValue member;
        if (!movieRoot->GetVariable(&member,parentPath.c_str())) return;
        if (!member.IsDisplayObject()) {
            AbortReplaceInstance(PatchFiles::MemberName(parentPath),member,relative);
            return;
        }
        GFxDisplayObject parent(member);
        bool loaded{};
        if (movieRoot->GetVariable(&member,instancePath.c_str())) {
            if (!member.IsDisplayObject()) { AbortReplaceInstance(instanceName,member,relative); return; }
            GFxDisplayObject instance(member);
            loaded=ReplaceInstance(instanceName,instance,parent,relative);
        } else loaded=AddInstance(instanceName,parent,relative);
        if (loaded && count<std::numeric_limits<int>::max()) ++count;
    },[](std::error_code error) {
        logger::warn("Optional patch traversal skipped an unavailable/cyclic path (system error {})",error.value());
    });
    API::DispatchMessage(API::FinishLoadInstancesMessage{menu,movieRoot,count});
    return count;
}

bool GFxMoviePatcher::AddInstance(const std::string& name,GFxDisplayObject& parent,const std::string& path)
{
    const auto depth=parent.GetNextHighestDepth();
    if (!depth) return false;
    auto instance=parent.CreateEmptyMovieClip(name,*depth);
    if (!instance.IsDisplayObject() || !instance.LoadMovie(path)) {
        logger::warn("Movie clip creation/loading failed for {}",name);
        return false;
    }
    movieRoot->Advance(0);
    LogMembersOf(parent); LogMembersOf(instance);
    const auto [definition,sprite]=instance.GetDefs();
    API::DispatchMessage(API::PostPatchInstanceMessage{menu,movieRoot,instance,definition,sprite});
    return true;
}

bool GFxMoviePatcher::ReplaceInstance(const std::string& name,GFxDisplayObject& original,
    GFxDisplayObject& parent,const std::string& path)
{
    API::DispatchMessage(API::PreReplaceInstanceMessage{menu,movieRoot,original});
    LogMembersOf(parent); LogMembersOf(original);
    const auto depth=parent.GetNextHighestDepth();
    if (!depth || !original.SwapDepths(*depth)) return false;
    movieRoot->Advance(0);
    if (!original.RemoveMovieClip()) return false;
    movieRoot->Advance(0);
    return AddInstance(name,parent,path);
}

void GFxMoviePatcher::AbortReplaceInstance(const std::string& name,RE::GFxValue& value,const std::string& path) const
{
    logger::warn("{} exists in the movie, but it is not a DisplayObject. Aborting replacement for {}",name,path);
    API::DispatchMessage(API::AbortPatchInstanceMessage{menu,movieRoot,value});
}
}
