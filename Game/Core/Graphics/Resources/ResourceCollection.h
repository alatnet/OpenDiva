#include "EffectResource.h"
#include "NoteResource.h"
#include "FontResource.h"
#include "TailResource.h"

#ifndef _H_RESOURCECOLLECTION_
#define _H_RESOURCECOLLECTION_

namespace LYGame {
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
	};
}

#endif //_H_RESOURCECOLLECTION_