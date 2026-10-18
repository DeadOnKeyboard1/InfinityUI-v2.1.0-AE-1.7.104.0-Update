#pragma once

#include "GFxObject.h"
#include "ValuePolicy.h"

#include "RE/G/GFxMovieDataDef.h"
#include "RE/G/GFxSprite.h"
#include "RE/G/GFxSpriteDef.h"

namespace IUI
{
	class GFxDisplayObject : public GFxObject
	{
	public:

		GFxDisplayObject(const RE::GFxValue& a_value)
		: GFxObject{ a_value }
		{
			assert(a_value.IsDisplayObject());
		}

		GFxDisplayObject CreateEmptyMovieClip(const std::string_view& a_name, std::int32_t a_depth)
		{
			RE::GFxValue mc;
			const std::string name(a_name);
			RE::GFxValue::CreateEmptyMovieClip(&mc, name.c_str(), a_depth);
			return mc;
		}

		std::optional<std::int32_t> GetNextHighestDepth()
		{
			const auto result=Invoke("getNextHighestDepth");
			return result.IsNumber() ? MovieClipDepth(result.GetNumber()) : std::nullopt;
		}

		bool SwapDepths(std::int32_t a_depth)
		{
			return Invoke("swapDepths", static_cast<RE::GFxValue*>(nullptr), a_depth);
		}

		bool LoadMovie(const std::string_view& a_swfRelativePath)
		{
			const std::string path(a_swfRelativePath);
			return Invoke("loadMovie", static_cast<RE::GFxValue*>(nullptr), path.c_str());
		}

		bool RemoveMovieClip()
		{
			return Invoke("removeMovieClip", static_cast<RE::GFxValue*>(nullptr));
		}

		std::pair<RE::GFxMovieDefImpl*, RE::GFxSpriteDef*> GetDefs() const
		{
			if (!IsDisplayObject() || !_objectInterface || !_value.obj) return {nullptr,nullptr};
			auto sprite = static_cast<RE::GFxSprite*>(static_cast<RE::GFxCharacterHandle*>(_value.obj)->character);
			if (!sprite) return {nullptr,nullptr};
			auto movieDef = sprite->GetResourceMovieDef();
			if (!movieDef || !movieDef->bindData || !movieDef->bindData->dataDef || !movieDef->bindData->dataDef->loadTaskData)
				return {nullptr,nullptr};

			RE::GFxMovieDataDef::LoadTaskData::ResourceHash& movieResourceSources = movieDef->bindData->dataDef->loadTaskData->resources;

			auto resourceSourceIt = movieResourceSources.Find(sprite->resourceID);

			if (resourceSourceIt != movieResourceSources.end() && resourceSourceIt->second.type == RE::GFxResourceSource::kNormal)
			{
				RE::GFxResource* resource = resourceSourceIt->second.data.resource;

				if (resource && resource->GetResourceType() == RE::GFxResource::ResourceType::kSpriteDef)
				{
					auto spriteDef = static_cast<RE::GFxSpriteDef*>(resource);

					return { movieDef, spriteDef };
				}
			}

			return { nullptr, nullptr };
		}

		RE::GPointF LocalToGlobal()
		{
			const auto view = GetMovieView();
			if (!IsDisplayObject() || !view) return {};
			GFxObject pt(view);
			if (!pt.IsObject() || !pt.SetMember("x", 0.0) || !pt.SetMember("y", 0.0) ||
				!Invoke("localToGlobal", static_cast<RE::GFxValue*>(nullptr), pt)) return {};
			const auto x = pt.GetMember("x");
			const auto y = pt.GetMember("y");
			if (!x.IsNumber() || !y.IsNumber()) return {};

			return { static_cast<float>(x.GetNumber()), static_cast<float>(y.GetNumber()) };
		}
	};
}
