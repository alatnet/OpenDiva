#ifndef _H_OPENDIVABUS_COMPONENT
#define _H_OPENDIVABUS_COMPONENT

#pragma once

#include "OpenDivaComponentFactory.h"
#include "../Bus/OpenDiva_Bus.h"

namespace LYGame {
	class OpenDivaBusComponent : public AZ::Component {
	OPENDIVA_REGISTER_COMPONENT(OpenDivaBusComponent);
	public:
		~OpenDivaBusComponent() override = default;
		AZ_COMPONENT(OpenDivaBusComponent, "{418E3DC9-9AEF-47F4-987B-08C6ADDE495C}");

		static void Reflect(AZ::ReflectContext* reflection);
	public:
	protected:
		// AZComponent
		void Init() override {};
		void Activate() override {};
		void Deactivate() override {};
	/*private:
		float m_someProperty;*/
	};

	class OpenDivaCompletionHandler : public OpenDivaBus::OpenDivaCompletionBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			OpenDivaCompletionHandler, //Name
			"{DFEA5D43-455C-4102-A819-C9EEC2EC6B18}", //TypeId
			AZ::SystemAllocator, //default allocator.
			// List of event names to handle and support for BehaviorContext.
			SetNotes,
			SetTotalNotes,
			SetRating
		);

		void SetNotes(int value) { Call(FN_SetNotes, value); }
		void SetTotalNotes(int value) { Call(FN_SetTotalNotes, value); }
		void SetRating(EDivaJudgeCompletion rating) { Call(FN_SetRating, rating); }
	};

	class OpenDivaHitScoreEventHandler : public OpenDivaBus::OpenDivaHitScoreBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			OpenDivaHitScoreEventHandler, //Name
			"{E93F94F2-CB2F-4133-83A4-6B016F58778A}", //TypeId
			AZ::SystemAllocator, //default allocator.
			// List of event names to handle and support for BehaviorContext.
			SetHitScore
		);

		void SetHitScore(EHitScore hitScore, bool wrong) { Call(FN_SetHitScore,hitScore,wrong); }
	};

	class OpenDivaSongHandler : public OpenDivaBus::OpenDivaSongBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			OpenDivaSongHandler, //Name
			"{5D82D8D2-AE0D-4CCD-8C5E-4604625D0809}", //TypeId
			AZ::SystemAllocator, //default allocator.
			// List of event names to handle and support for BehaviorContext.
			OnStart,
			OnEnd
		);

		void OnStart() { Call(FN_OnStart); }
		void OnEnd() { Call(FN_OnEnd); }
	};

	class OpenDivaScoreHandler : public OpenDivaBus::OpenDivaScoreBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			OpenDivaScoreHandler, //Name
			"{72A9D165-0041-4898-A9FE-F519B58BFE55}", //TypeId
			AZ::SystemAllocator, //default allocator.
			// List of event names to handle and support for BehaviorContext.
			SetScore
		);

		void SetScore(int value) { Call(FN_SetScore, value); }
	};

	class OpenDivaZoneEventsHandler : public OpenDivaBus::OpenDivaZoneEventsBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			OpenDivaZoneEventsHandler, //Name
			"{315DCA81-22CD-4893-9EDA-52F18EB6199E}", //TypeId
			AZ::SystemAllocator, //default allocator.
			// List of event names to handle and support for BehaviorContext.
			OnChanceEnter,
			OnChanceExit,
			OnTechEnter,
			OnTechExit
		);

		void OnChanceEnter() { Call(FN_OnChanceEnter); }
		void OnChanceExit() { Call(FN_OnChanceExit); }
		void OnTechEnter() { Call(FN_OnTechEnter); }
		void OnTechExit() { Call(FN_OnTechExit); }
	};

	/*class OpenDivaUiInputHandler : public OpenDivaBus::OpenDivaUiInputBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			OpenDivaUiInputHandler, //Name
			"{E950B06A-8635-4660-A0B3-22C90E0983F5}", //TypeId
			AZ::SystemAllocator, //default allocator.
			// List of event names to handle and support for BehaviorContext.
			OnUi_Up,
			OnUi_Down,
			OnUi_Left,
			OnUi_Right,
			OnUi_Select,
			OnUi_Cancel
		);

		void OnUi_Up(int mode, float value) { Call(FN_OnUi_Up, mode, value); }
		void OnUi_Down(int mode, float value) { Call(FN_OnUi_Down, mode, value); }
		void OnUi_Left(int mode, float value) { Call(FN_OnUi_Left, mode, value); }
		void OnUi_Right(int mode, float value) { Call(FN_OnUi_Right, mode, value); }
		void OnUi_Select(int mode, float value) { Call(FN_OnUi_Select, mode, value); }
		void OnUi_Cancel(int mode, float value) { Call(FN_OnUi_Cancel, mode, value); }
	};*/
}

#endif