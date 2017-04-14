/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#if !defined(AFX_STDAFX_H__B36C325D_F0EA_4C45_F3BC_1E5EAB3A5E43__INCLUDED_)
#define AFX_STDAFX_H__B36C325D_F0EA_4C45_F3BC_1E5EAB3A5E43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CryModuleDefs.h>
#define eCryModule eCryM_Game

#include <algorithm>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <limits>
#include <platform.h>
#include <smartptr.h>
#include <Cry_Math.h>
#include <CryName.h>
#include <CryThread.h>
#include <ISystem.h>
#include <I3DEngine.h>
#include <IConsole.h>
#include <IInput.h>
#include <ILog.h>
#include <ISerialize.h>
#include <ITimer.h>
#include <GameUtils.h>

#include <GameWarning.h>

#include <LyShine\IDraw2d.h>

#include <IMovieSystem.h>
#include <ITexture.h>

//provided by http://softagalleria.net/dirent.php
//for directory listing in windows
//got to find those files!
#ifdef WIN32
	#include "System\dirent.h"
#else
	#include <dirent.h>
#endif

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

#endif // !defined(AFX_STDAFX_H__B36C325D_F0EA_4C45_F3BC_1E5EAB3A5E43__INCLUDED_)
