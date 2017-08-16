#include <StdAfx.h>

#include "{{object.header}}"
#include <functional>

#include "{{class.name}}Factory.h"

namespace OpenDiva {
	{{class.name}}Factory::{{class.name}}Factory() {
		this->m_funcs = new std::unordered_map<std::string, New{{object.name}}Func>();
		this->m_names = new std::vector<std::string>();
	}

	{{class.name}}Factory::~{{class.name}}Factory() {
		this->m_funcs->clear();
		delete this->m_funcs;

		this->m_names->clear();
		delete this->m_names;
	}


	{{object.name}}* {{class.name}}Factory::new{{object.name}}(std::string name) {
		auto funcEntry = this->m_funcs->find(name);
		if (funcEntry != this->m_funcs->end()) return funcEntry->second();

		//if (this->m_funcs->at(name) != nullptr) return this->m_funcs->at(name)();
		return nullptr;
	}

	void {{class.name}}Factory::register{{object.name}}(std::string name, New{{object.name}}Func ptr) {
		this->m_funcs->insert({ name, ptr });
		this->m_names->push_back(name);
	}

	{{class.name}}Factory& {{class.name}}Factory::getFactory() {
		static {{class.name}}Factory theInstance;
		return theInstance;
	}
}