#include <AzCore\Math\Vector2.h>
#include <AzCore\Math\Vector3.h>

//most likely wont need it as gEnv->pFileIO provides a way of listing files.
//provided by http://softagalleria.net/dirent.php
//for directory listing in windows
//got to find those files!
/*#ifdef WIN32
#include "System\dirent.h"
#else
#include <dirent.h>
#endif*/

//our utility file.
//since this file gets included everywhere we can have our utility file be easily accessed.
#include "Core/Util.h"
#include "Core\CommonEnums.h"
#include "Core\CommonDefines.h"

//Steamworks stuff.
//Only used in public steam builds
#ifdef STEAMWORKS
#include "steam_api.h"
#endif