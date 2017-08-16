#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include <AzCore\std\functional.h>
#include "DivaNoteBaseNode.h"
#include "DivaNoteNodeFactory.h"

namespace LYGame {
	DivaNoteNodeFactory::DivaNoteNodeFactory() {
		this->m_funcs = new std::unordered_map<std::string, NewDivaNoteNodeFunc>();
		this->m_names = new std::vector<std::string>();
	}

	DivaNoteNodeFactory::~DivaNoteNodeFactory() {
		this->m_funcs->clear();
		delete this->m_funcs;

		this->m_names->clear();
		delete this->m_names;
	}

	DivaNoteBaseNode* DivaNoteNodeFactory::newNoteNode(std::string name, ResourceCollection *rc, NoteEntry* entry) {
		auto funcEntry = this->m_funcs->find(name);
		if (funcEntry != this->m_funcs->end()) return funcEntry->second(rc, entry);

		//if (this->m_funcs->at(name) != nullptr) return this->m_funcs->at(name)(rc,divaJudge,entry);
		return nullptr;
	}

	void DivaNoteNodeFactory::registerDivaNoteNode(std::string name, NewDivaNoteNodeFunc ptr) {
		this->m_funcs->insert({ name, ptr });
		this->m_names->push_back(name);
	}

	DivaNoteNodeFactory& DivaNoteNodeFactory::getFactory() {
		static DivaNoteNodeFactory theInstance;
		return theInstance;
	}
}