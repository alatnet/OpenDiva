#include "EffectResource.h"
#include "NoteResource.h"
#include "FontResource.h"
#include "TailResource.h"

#ifndef _H_RESOURCECOLLECTION_
#define _H_RESOURCECOLLECTION_

namespace OpenDiva {
	struct ResourceCollection {
		EffectResource * p_EffectResource;
		NoteResource * p_NoteResource;
		FontResource * p_FontResource;
		TailResource * p_TailResource;

		ResourceCollection() :
			p_EffectResource(nullptr),
			p_NoteResource(nullptr),
			p_FontResource(nullptr),
			p_TailResource(nullptr)
		{};

		~ResourceCollection() {
			if (p_EffectResource != nullptr) delete p_EffectResource;
			if (p_NoteResource != nullptr) delete p_NoteResource;
			if (p_FontResource != nullptr) delete p_FontResource;
			if (p_TailResource != nullptr) delete p_TailResource;
		};

		void setScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) {
			if (p_EffectResource != nullptr) p_EffectResource->setScale(scale);
			if (p_NoteResource != nullptr) p_NoteResource->setScale(scale);
			if (p_FontResource != nullptr) p_FontResource->setScale(scale);
			if (p_TailResource != nullptr) p_TailResource->setScale(scale);
		}
	};
}

#endif //_H_RESOURCECOLLECTION_