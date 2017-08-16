#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include "NoteFileEntries.h"
#include <AzCore\std\functional.h>

#include "NoteFileEntryFactory.h"


namespace LYGame {
	NoteFileEntryFactory::NoteFileEntryFactory() {
		this->m_funcs = new std::unordered_map<std::string, NewNoteEntryFunc>();
		this->m_names = new std::vector<std::string>();
	}

	NoteFileEntryFactory::~NoteFileEntryFactory() {
		this->m_funcs->clear();
		delete this->m_funcs;

		this->m_names->clear();
		delete this->m_names;
	}


	NoteEntry* NoteFileEntryFactory::newNoteEntry(std::string name) {
		auto funcEntry = this->m_funcs->find(name);
		if (funcEntry != this->m_funcs->end()) return funcEntry->second();

		//if (this->m_funcs->at(name) != nullptr) return this->m_funcs->at(name)();
		return nullptr;
	}

	void NoteFileEntryFactory::registerNoteEntry(std::string name, NewNoteEntryFunc ptr) {
		this->m_funcs->insert({ name, ptr });
		this->m_names->push_back(name);
	}

	NoteFileEntryFactory& NoteFileEntryFactory::getFactory() {
		static NoteFileEntryFactory theInstance;
		return theInstance;
	}
}