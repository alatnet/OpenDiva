#ifndef _H_TAILRESOURCE_
#define _H_TAILRESOURCE_
#pragma once

#include "../Tails/CometTail.h"
#include "../Tails/HoldTail.h"

namespace OpenDiva {
	static char const* sTailFileName = "tailConfig.xml";

	/*struct HoldTailStyle {
		const char * name;
		HoldTail *m_pHoldTails[4];
	};*/

	class TailResource {
	public:
		TailResource(const char *folder);
		~TailResource();
	public:
		CometTail * getCTail(ENoteType type) { return this->m_pCometTails[type]; }
		HoldTail * getHTail(ENoteType type) { /*return this->m_pHStyles[m_hTailStyle].m_pHoldTails[type];*/ return this->m_pHoldTails[type]; }
		//const char * getHTailStyleName(unsigned int index);
		//unsigned int getNumHTailStyles() { return this->m_numHTailStyles; }
		//void setHTailStyle(unsigned int index);

		void setCScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f));
		void setHScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f));

		void setScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f));
	private:
		void setCTailDefaults();
		//void setHTailDefaults();

		void loadCTail(XmlNodeRef comet);
		//void loadHTail(XmlNodeRef hold);
	private:
		//unsigned int m_hTailStyle, m_numHTailStyles;
		CometTail ** m_pCometTails;
		HoldTail** m_pHoldTails;
		//HoldTailStyle * m_pHStyles;
	};
}

#endif //_H_TAILRESOURCE_