#include "utils/INISettingCollection.h"

#include "utils/Logger.h"

namespace utils
{
	INISettingCollection::INISettingCollection() noexcept
	{
		REL::Relocation<std::uintptr_t> __vTable(*reinterpret_cast<std::uintptr_t*>(this));

		// Use the Skyrim's INISettingCollection virtual functions
		std::uintptr_t* skyrimsVTable = REL::Relocation<std::uintptr_t*>{ vTableId }.get();

		// Replace all except the destructor (index 0)
		for (int i = 1; i < 10; i++) {
			__vTable.write_vfunc(i, skyrimsVTable[i]);
		}
	}

	bool INISettingCollection::ReadFromFile(std::string_view a_fileName)
	{
		std::error_code error;
		auto iniPath=std::filesystem::current_path(error);
		if (error) return false;
		iniPath/="Data/SKSE/Plugins";
		iniPath/=a_fileName;
		std::string path;
		try { path=iniPath.string(); }
		catch (const std::filesystem::filesystem_error&) { return false; }
		if (path.size()>=sizeof(subKey)) return false;
		if (strcpy_s(subKey,path.c_str())!=0) return false;

		if (_this()->OpenHandle(false)) {
			_this()->ReadAllSettings();
			_this()->CloseHandle();

			return true;
		}

		return false;
	}
}
