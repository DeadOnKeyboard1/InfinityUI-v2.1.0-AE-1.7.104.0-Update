#include "Hooks.h"
#include "GFxMoviePatcher.h"
#include "GFxDisplayObject.h"
#include "GFxArray.h"
#include "utils/INISettingCollection.h"

static_assert(sizeof(RE::GFxValue)==0x18);
static_assert(sizeof(RE::GFxMovieRoot::Flag)==4);
static_assert(RE::GFxResourceSource::kNormal==0 && RE::GFxResourceSource::kImported==1);
static_assert(offsetof(RE::GFxResourceSource,data)==8);
static_assert(sizeof(RE::INISettingCollection)==sizeof(utils::INISettingCollection));
static_assert(sizeof(IUI::API::Message)==0x10);
static_assert(sizeof(IUI::API::StartLoadInstancesMessage)==0x10);
static_assert(sizeof(IUI::API::PreReplaceInstanceMessage)==0x18);
static_assert(sizeof(IUI::API::PostPatchInstanceMessage)==0x28);
static_assert(sizeof(IUI::API::AbortPatchInstanceMessage)==0x18);
static_assert(sizeof(IUI::API::FinishLoadInstancesMessage)==0x18);
static_assert(sizeof(IUI::API::PostInitExtensionsMessage)==0x10);
static_assert(std::is_same_v<decltype(&RE::GFxValue::ObjectInterface::VisitElements),
    void(RE::GFxValue::ObjectInterface::*)(void*, RE::GFxValue::ObjectInterface::ArrVisitor*, std::uint32_t, std::int32_t) const>);
static_assert(std::is_same_v<decltype(&hooks::InitExtensionsAndNotify),
    void(*)(RE::GFxMovieRoot*, const char*, const char*, RE::IMenu*)>);
static_assert(std::is_same_v<decltype(static_cast<void(RE::GFxValue::*)(RE::GFxValue::ArrayVisitor*,std::uint32_t,std::int32_t) const>(&RE::GFxValue::VisitElements)),
    void(RE::GFxValue::*)(RE::GFxValue::ArrayVisitor*,std::uint32_t,std::int32_t) const>);
