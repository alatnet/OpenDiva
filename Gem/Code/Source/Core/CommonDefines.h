#ifndef _H_COMMONDEFINES_
#define _H_COMMONDEFINES_
#pragma once

#include <AzCore/std/string/string.h>
#include <string>

#define OPENDIVA_VER_HIGH 0
#define OPENDIVA_VER_LOW 1
#define OPENDIVA_VER_REV 0
#define OPENDIVA_VER_STR "0.1.0"

#define OPENDIVA_SALT "OpenDivaSalt"

#define CLOG(...) CryLog(__VA_ARGS__)

namespace OpenDiva {
	namespace FolderStruct {
		namespace Folders {
			//base game folder
			static const std::string sOpenDivaFolder = "OpenDiva";

			//scripts folder
			static const std::string sScriptsFolder = "Scripts";

			//songs folder
			static const std::string sSongFolder = "Songs";

			//resources folder
			static const std::string sResourcesFolder = "Resources";
			static const std::string sHudFolder = "Hud";

			//style folders
			static const std::string sStylesFolder = "Styles";
			static const std::string sEffectsFolder = "Effects";
			static const std::string sNoteFolder = "Note";
			static const std::string sTailFolder = "Tails";
			static const std::string sFontsFolder = "Fonts";
		}
		namespace Paths {
			static const std::string sSongPath = "/" + Folders::sSongFolder;
			static const std::string sResourcesPath = "/" + Folders::sResourcesFolder;
			static const std::string sHudPath = sResourcesPath + "/" + Folders::sHudFolder;
			static const std::string sStylesPath = sResourcesPath + "/" + Folders::sStylesFolder;
			static const std::string sScriptsPath = "/" + Folders::sScriptsFolder;
		}
		/*namespace Paths {
			static const string sSongPath = "/" + Folders::sOpenDivaFolder + "/" + Folders::sSongFolder;
			static const string sResourcesPath = "/" + Folders::sOpenDivaFolder + "/" + Folders::sResourcesFolder;
			static const string sHudPath = sResourcesPath + "/" + Folders::sHudFolder;
			static const string sStylesPath = sResourcesPath + "/" + Folders::sStylesFolder;
			static const string sScriptsPath = "/" + Folders::sOpenDivaFolder + "/" + Folders::sScriptsFolder;
		}*/
	}

	#ifndef STATIC_CONSTRUCTOR
		#define STATIC_CONSTRUCTOR
		//so that we can have a simple way of static constructors
		//http://stackoverflow.com/questions/1197106/static-constructors-in-c-i-need-to-initialize-private-static-objects
		template<void(*ctor)()>
		struct static_constructor
		{
			struct constructor { constructor() { ctor(); } };
			static constructor c;
		};

		template<void(*ctor)()>
		typename static_constructor<ctor>::constructor static_constructor<ctor>::c;
	#endif
}

//because /WX is fucking with us...
//quick and easy way to disable and enable a warning
#ifdef WX_DISABLE
	#ifdef _MSC_VER
		#define WX_DISABLE_(x) __pragma(warning(disable : x));
		#define WX_ENABLE_(x) __pragma(warning(default : x));
	#else
		#define WX_DISABLE_(x)
		#define WX_ENABLE_(x)
	#endif
#else
	#define WX_DISABLE_(x)
	#define WX_ENABLE_(x)
#endif

#endif