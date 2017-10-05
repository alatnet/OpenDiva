#include <StdAfx.h>

#include <OpenDivaCommon.h>
#include "OpenDivaIntro.h"

#include <AzCore\std\string\conversions.h>
#include "../../Listing/SongList.h"

#include "../../Database/DatabaseManager.h"

namespace OpenDiva {
	OpenDivaIntro::OpenDivaIntro() {
		this->m_str = "";

		this->m_options = Draw2dHelper::GetDraw2d()->GetDefaultTextOptions();

		//this->m_options.color = AZ::Vector3(0.2, 1.0, 0.4); //terminal green color 1
		this->m_options.color = AZ::Vector3(0.29020, 0.92549, 0.14902); //terminal green color 2
	}

	OpenDivaIntro::~OpenDivaIntro() {
	}

	void OpenDivaIntro::SetState(EIntroState state) { //i wonder if anyone will get the reference...
		this->m_mutex.lock();
		this->m_str = "Open Diva OS ver. " OPENDIVA_VER_STR "\n";

		if (state >= eIS_Section2)
			this->m_str += "Get Ready To Start Sequence...\n";

		if (state >= eIS_Section3)
			this->m_str += "Uploading Voice Data...\n";

		if (state >= eIS_Section4) {
			this->m_str += "Uploading Song Data...";
			this->m_str += "\n- Groups: " + AZStd::to_string(SongList::GroupCount());
			this->m_str += "\n- Songs: " + AZStd::to_string(SongList::SongCount());
			this->m_str += "\n";
		}

		if (state >= eIS_Section5)
			this->m_str += "Control Parameter All Clear...\n";

		if (state >= eIS_Section6)
			this->m_str += "Start Sequence Complete...";

		this->m_mutex.unlock();
	}

	void OpenDivaIntro::Render() {
		this->m_mutex.lock();
		Draw2dHelper::GetDraw2d()->DrawText(this->m_str.c_str(), AZ::Vector2(10, 10), 16, 1.0f, &this->m_options);
		this->m_mutex.unlock();
	}
}