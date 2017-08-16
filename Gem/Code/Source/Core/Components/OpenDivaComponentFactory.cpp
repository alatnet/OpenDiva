#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include <AzCore/std/functional.h>

#include "OpenDivaComponentFactory.h"


namespace OpenDiva {
	OpenDivaComponentFactory::OpenDivaComponentFactory() {
		//if (!AZ::AllocatorInstance<AZ::SystemAllocator>::IsReady()) AZ::AllocatorInstance<AZ::SystemAllocator>::Create();
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