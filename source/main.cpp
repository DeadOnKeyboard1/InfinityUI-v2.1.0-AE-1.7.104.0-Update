#include "Hooks.h"
#include "Settings.h"

#include "utils/Logger.h"

SKSEPluginInfo(
    .Version = REL::Version{2, 1, 3, 0},
    .Name = "InfinityUI"sv,
    .Author = "alexsylex"sv,
    .StructCompatibility = SKSE::StructCompatibility::Dependent,
    .RuntimeCompatibility = {SKSE::VersionIndependence::AddressLibrary, true},
    .MinimumSKSEVersion = REL::Version{0, 0, 0, 0}
)

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	if (!a_skse || a_skse->IsEditor()) return false;
	const SKSE::PluginDeclaration* plugin = SKSE::PluginDeclaration::GetSingleton();

	if (!logger::init(plugin->GetName())) 
	{
		return false;
	}

	logger::info("Loading {} {}...", plugin->GetName(), plugin->GetVersion());

	SKSE::Init(a_skse);

	settings::Init(std::string(plugin->GetName()) + ".ini");

	logger::set_level(settings::debug::logLevel, settings::debug::logLevel);

	if (!hooks::Install()) return false;

	logger::set_level(logger::level::info, logger::level::info);
	logger::info("Succesfully loaded!");

	logger::set_level(settings::debug::logLevel, settings::debug::logLevel);

	return true;
}
