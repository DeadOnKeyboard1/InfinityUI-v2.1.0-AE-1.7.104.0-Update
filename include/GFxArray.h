#pragma once

#include "RE/G/GFxMovieRoot.h"
#include "RE/G/GFxValue.h"
#include <algorithm>
#include <limits>

namespace IUI
{
	class GFxArray : public RE::GFxValue
	{
	public:

		static GFxArray GetFrom(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToArray)
		{
			RE::GFxValue array;

			if (!a_movieView) return array;
			const std::string path(a_pathToArray);
			a_movieView->GetVariable(&array, path.c_str());

			assert(array.IsArray());

			return array;
		}

		GFxArray(RE::GFxMovieView* a_movieView)
		{
			if (!a_movieView) return;
			a_movieView->CreateArray(this);

			assert(GetMovieView() == a_movieView);
			assert(IsArray());
		}

		GFxArray(const RE::GFxValue& a_value)
		{
			*static_cast<RE::GFxValue*>(this) = a_value;

			assert(IsArray());
		}

		RE::GFxMovieView* GetMovieView()
		{
			return _objectInterface ? _objectInterface->GetMovieRoot() : nullptr;
		}

		RE::GFxValue GetElement(std::uint32_t a_index) const
		{
			RE::GFxValue value;
			if (!IsArray() || !_objectInterface || a_index>=GetArraySize()) return value;
			RE::GFxValue::GetElement(a_index, &value);
			return value;
		}

		bool SetElement(std::uint32_t a_index, const RE::GFxValue& a_value)
		{
			return IsArray() && _objectInterface && RE::GFxValue::SetElement(a_index, a_value);
		}

		std::int32_t FindElement(const RE::GFxValue& a_value)
		{
			if (!IsArray() || !_objectInterface) return -1;
			const auto size=std::min(GetArraySize(), static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())+1U);
			for (std::uint32_t i = 0; i < size; i++)
			{
				RE::GFxValue element;
				if (RE::GFxValue::GetElement(i, &element) && element == a_value)
				{
					return static_cast<std::int32_t>(i);
				}
			}

			return -1;
		}
	};
}
