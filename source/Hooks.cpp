#include "RE/B/BStimer.h"

#include "Hooks.h"

#include "GFxMoviePatcher.h"

namespace hooks
{
	void PatchGFxMovie(RE::GFxMovieRoot* a_movieRoot, float a_deltaT, std::uint32_t a_frameCatchUpCount, RE::IMenu* a_menu)
	{
		a_movieRoot->Advance(a_deltaT, a_frameCatchUpCount);
		const auto definition=a_movieRoot->GetMovieDef();
		if (!a_menu || !definition || !definition->GetFileURL()) return;

		IUI::GFxMoviePatcher moviePatcher{ a_menu, a_movieRoot };

		int loadedInstancesCount = moviePatcher.LoadInstancePatches();


		const auto movieRootFileUrl = moviePatcher.MovieName();

		if (loadedInstancesCount) 
		{
			logger::info("Patches loaded for {}: {}", movieRootFileUrl, loadedInstancesCount);
		}
		else
		{
			logger::debug("Patches loaded for {}: 0", movieRootFileUrl);
		}
		logger::debug("");

		//if (loadedInstancesCount)
		//{
		//	IUI::g_moviePatchersList.emplace(a_movieRoot, moviePatcher);
		//}
	}

	void InitExtensionsAndNotify(RE::GFxMovieRoot* a_movieRoot, const char* a_methodName, const char* a_argFmt, RE::IMenu* a_menu)
	{
		GFxMovieRoot::Invoke(a_movieRoot, a_methodName, a_argFmt);

		//if (IUI::g_moviePatchersList.contains(a_movieRoot))
		//{
			IUI::API::DispatchMessage(IUI::API::PostInitExtensionsMessage{ a_menu, a_movieRoot });
		//}
	}
}
