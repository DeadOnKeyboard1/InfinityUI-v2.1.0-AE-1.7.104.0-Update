#pragma once

#include "RE/G/GFxMovieRoot.h"
#include "RE/G/GFxValue.h"

namespace IUI
{
	class GFxObject : public RE::GFxValue
	{
	protected:

		template <typename... Args>
		GFxObject(RE::GFxMovieView* a_movieView, const char* a_className, Args&&... args)
		{
			if (!a_movieView) return;
			std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

			a_movieView->CreateObject(this, a_className, sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args));

			assert(GetMovieView() == a_movieView);
			assert(IsObject());
		}

	public:

		static GFxObject GetFrom(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToObject)
		{
			RE::GFxValue object;
			if (!a_movieView) return object;
			const std::string path(a_pathToObject);
			a_movieView->GetVariable(&object, path.c_str());

			assert(object.IsObject());

			return object;
		}

		template <typename... Args>
		GFxObject(RE::GFxMovieView* a_movieView, Args&&... args)
		{
			if (!a_movieView) return;
			std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

			a_movieView->CreateObject(this, nullptr, sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args));

			assert(GetMovieView() == a_movieView);
			assert(IsObject());
		}

		GFxObject(const RE::GFxValue& a_value)
		{
			*static_cast<RE::GFxValue*>(this) = a_value;

			assert(IsObject());
		}

		RE::GFxMovieView* GetMovieView()
		{
			return _objectInterface ? _objectInterface->GetMovieRoot() : nullptr;
		}

		RE::GFxValue GetMember(const std::string_view& a_memberName) const
		{
			RE::GFxValue value;
			if (!IsObject() || !_objectInterface) return value;
			const std::string name(a_memberName);
			RE::GFxValue::GetMember(name.c_str(), &value);
			return value;
		}
	 
		bool SetMember(const std::string_view& a_memberName, const RE::GFxValue& a_value)
		{
			if (!IsObject() || !_objectInterface) return false;
			const std::string name(a_memberName);
			return RE::GFxValue::SetMember(name.c_str(), a_value);
		}

		template <typename... Args>
		RE::GFxValue Invoke(const std::string_view& a_functionName, Args&&... args)
		{
			RE::GFxValue result;
			Invoke(a_functionName, &result, args...);
			return result;
		}

		template <typename... Args>
		bool Invoke(const std::string_view& a_functionName, RE::GFxValue* a_result, Args&&... args)
		{
			if (!IsObject() || !_objectInterface) return false;
			const std::string name(a_functionName);
			std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

			if (RE::GFxValue::Invoke(name.c_str(), a_result,
									 sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args)))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}
