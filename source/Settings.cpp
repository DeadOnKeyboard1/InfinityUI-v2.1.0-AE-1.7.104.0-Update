#include "Settings.h"

#include "utils/INISettingCollection.h"
#include "ValuePolicy.h"

namespace settings
{
	using namespace utils;

	void Init(const std::string& a_iniFileName)
	{
		INISettingCollection* iniSettingCollection = INISettingCollection::GetSingleton();

		{
			using namespace debug;
			iniSettingCollection->AddSettings(
				MakeSetting("uLogLevel:Debug", static_cast<std::uint32_t>(logLevel)));
		}

		if (!iniSettingCollection->ReadFromFile(a_iniFileName)) {
			logger::warn("Could not read {}, falling back to default options", a_iniFileName);
		}

		{
			using namespace debug;
			const auto value = iniSettingCollection->GetSetting<std::uint32_t>("uLogLevel:Debug");
			if (IUI::ValidLogLevel(value, static_cast<std::uint32_t>(logger::level::off))) {
				logLevel = static_cast<logger::level>(value);
			} else {
				logger::warn("Invalid uLogLevel:Debug; retaining the default log level");
			}
		}
	}
}
