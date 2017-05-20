#ifndef _H_OPENDIVA_COMPONENT_FACTORY
#define _H_OPENDIVA_COMPONENT_FACTORY

#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore\Component\ComponentApplicationBus.h>

namespace LYGame {
	typedef std::function<void (void)> OpenDivaRegisterComponentFunc;

	class OpenDivaComponentFactory {
	public:
		static OpenDivaComponentFactory& getFactory();
	public:
		void RegisterComponents();
	public:
		void registerComponent(OpenDivaRegisterComponentFunc ptr);
	private:
		OpenDivaComponentFactory();
		~OpenDivaComponentFactory();
		OpenDivaComponentFactory(const OpenDivaComponentFactory&) = delete;
		OpenDivaComponentFactory& operator= (const OpenDivaComponentFactory&) = delete;
	private:
		std::vector<OpenDivaRegisterComponentFunc> * m_funcs;
	};

	#define OPENDIVA_REGISTER_COMPONENT(component) \
		public: \
			static void component##Register() {	AZ::ComponentApplicationBus::Broadcast(&AZ::ComponentApplicationBus::Events::RegisterComponentDescriptor, component##::CreateDescriptor()); } \
			static void Register##component##ToFactory() { \
				static_constructor<&##component##::Register##component##ToFactory>::c; \
				OpenDivaComponentFactory::getFactory().registerComponent(&##component##::##component##Register); \
			}
}

#endif