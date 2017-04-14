#include <StdAfx.h>

#include "IDivaJudge.h"
#include <functional>

#include "DivaJudgeFactory.h"

namespace LYGame {
	DivaJudgeFactory::DivaJudgeFactory() {
		this->m_funcs = new std::unordered_map<std::string, NewJudgeFunc>();
		this->m_names = new std::vector<std::string>();
	}

	DivaJudgeFactory::~DivaJudgeFactory() {
		this->m_funcs->clear();
		delete this->m_funcs;

		this->m_names->clear();
		delete this->m_names;
	}

	IDivaJudge* DivaJudgeFactory::newJudge(std::string name, void * userdata) {
		auto funcEntry = this->m_funcs->find(name);
		if (funcEntry != this->m_funcs->end()) return funcEntry->second(userdata);

		//if (this->m_funcs->at(name) != nullptr) return this->m_funcs->at(name)();
		return nullptr;
	}

	void DivaJudgeFactory::registerDivaJudge(std::string name, NewJudgeFunc ptr) {
		this->m_funcs->insert({ name, ptr });
		this->m_names->push_back(name);
	}

	DivaJudgeFactory& DivaJudgeFactory::getFactory() {
		static DivaJudgeFactory theInstance;
		return theInstance;
	}
}