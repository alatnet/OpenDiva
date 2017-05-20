#include <StdAfx.h>
#include <functional>

#include "OpenDivaComponentFactory.h"


namespace LYGame {
	OpenDivaComponentFactory::OpenDivaComponentFactory() {
		this->m_funcs = new std::vector<OpenDivaRegisterComponentFunc>();
	}

	OpenDivaComponentFactory::~OpenDivaComponentFactory() {
		this->m_funcs->clear();
		delete this->m_funcs;
	}

	void OpenDivaComponentFactory::RegisterComponents() {
		for (unsigned long i = 0; i < this->m_funcs->size(); i++) this->m_funcs->at(i)();
		this->m_funcs->clear();
	}

	void OpenDivaComponentFactory::registerComponent(OpenDivaRegisterComponentFunc ptr) {
		this->m_funcs->push_back(ptr);
	}

	OpenDivaComponentFactory& OpenDivaComponentFactory::getFactory() {
		static OpenDivaComponentFactory theInstance;
		return theInstance;
	}
}