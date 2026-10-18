#pragma once
#include "FullAPI.h"
#include "GFxLoggers.h"
#include "PatchFiles.h"

namespace IUI {
class GFxDisplayObject;
class GFxMoviePatcher :
    protected GFxMemberLogger<logger::level::trace>,
    protected GFxArrayLogger<logger::level::trace> {
public:
    GFxMoviePatcher(RE::IMenu*, RE::GFxMovieRoot*);
    int LoadInstancePatches();
    std::string_view MovieName() const { return movieRootFileBasename; }
private:
    bool AddInstance(const std::string&, GFxDisplayObject&, const std::string&);
    bool ReplaceInstance(const std::string&, GFxDisplayObject&, GFxDisplayObject&, const std::string&);
    void AbortReplaceInstance(const std::string&, RE::GFxValue&, const std::string&) const;
    RE::IMenu* menu{};
    RE::GFxMovieRoot* movieRoot{};
    std::string movieRootFileBasename;
    std::filesystem::path movieRootFilePath{"Data/Interface"};
    std::filesystem::path startPath;
};
}
