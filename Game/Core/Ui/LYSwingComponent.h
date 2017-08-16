#ifndef _H_LYSWING_COMPONENT
#define _H_LYSWING_COMPONENT

#pragma once

#include "LYSwingIncludes.h"

namespace LYSwing {
	class LYSwingComponent {
	public:
		LYSwingComponent(const LyShine::NameType& name, AZ::EntityId canvasId) {
			EBUS_EVENT_ID_RESULT(this->m_entity.first, canvasId, UiCanvasBus, CreateChildElement, name);
			this->m_entity.second = this->m_entity.first->GetId();
			this->Initialize();
		}

		LYSwingComponent(const LyShine::NameType& name, UiCanvasInterface* canvas) {
			this->m_entity.first = canvas->CreateChildElement(name);
			this->m_entity.second = this->m_entity.first->GetId();
			this->Initialize();
		}

		LYSwingComponent(const LyShine::NameType& name, LYSwingComponent * component) {
			EBUS_EVENT_ID_RESULT(this->m_entity.first, component->GetId(), UiElementBus, CreateChildElement, name);
			this->m_entity.second = this->m_entity.first->GetId();
			this->Initialize();
		}

		~LYSwingComponent() {
			/*m_entity.first->Deactivate();
			delete m_entity.first;*/
		}
	public:
		void Init() { this->m_entity.first->Init(); }
		void Activate() { this->m_entity.first->Activate(); }
		void Deactivate() { this->m_entity.first->Deactivate(); }

		AZ::EntityId GetId() { return this->m_entity.second; }
		bool IsValid(){ return this->m_entity.second.IsValid(); }

		//Transform2d Set
		void SetAnchors(UiTransform2dInterface::Anchors anchors, bool adjustOffsets, bool allowPush) {
			EBUS_EVENT_ID(this->m_entity.second, UiTransform2dBus, SetAnchors, anchors, adjustOffsets, allowPush);
		}
		void SetOffsets(UiTransform2dInterface::Offsets offsets) {
			EBUS_EVENT_ID(this->m_entity.second, UiTransform2dBus, SetOffsets, offsets);
		}
		void SetPivotAndAdjustOffsets(Vec2 pivot) {
			EBUS_EVENT_ID(this->m_entity.second, UiTransform2dBus, SetPivotAndAdjustOffsets, pivot);
		}

		//Transform2d Get
		UiTransform2dInterface::Anchors GetAnchors() {
			UiTransform2dInterface::Anchors ret;
			EBUS_EVENT_ID_RESULT(ret,this->m_entity.second, UiTransform2dBus, GetAnchors);
			return ret;
		}
		UiTransform2dInterface::Offsets GetOffsets() {
			UiTransform2dInterface::Offsets ret;
			EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTransform2dBus, GetOffsets);
			return ret;
		}

		//Transform Set
		void SetPivot(Vec2 pivot) {
			EBUS_EVENT_ID(this->m_entity.second, UiTransformBus, SetPivot, pivot);
		}
		void SetRecomputeTransformFlag() {
			EBUS_EVENT_ID(this->m_entity.second, UiTransformBus, SetRecomputeTransformFlag);
		}
		void SetScale(Vec3 scale) {
			EBUS_EVENT_ID(this->m_entity.second, UiTransformBus, SetScale, scale);
		}
		void SetScaleToDevice(bool scaleToDevice) {
			EBUS_EVENT_ID(this->m_entity.second, UiTransformBus, SetScaleToDevice, scaleToDevice);
		}
		void SetZRotation(float rotation) {
			EBUS_EVENT_ID(this->m_entity.second, UiTransformBus, SetZRotation, rotation);
		}

		//Transform Get
		Vec2 GetPivot() {
			Vec2 ret;
			EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTransformBus, GetPivot);
			return ret;
		}
		Vec3 GetScale() {
			Vec3 ret;
			EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTransformBus, GetScale);
			return ret;
		}
		bool GetScaleToDevice() {
			bool ret;
			EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTransformBus, GetScaleToDevice);
			return ret;
		}
		float GetZRotation() {
			float ret;
			EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTransformBus, GetZRotation);
			return ret;
		}
	public:
		std::pair<AZ::Entity *, AZ::EntityId> m_entity;
	private:
		void Initialize() {
			/*auto elementFactory = gEnv->pLyShine->GetComponentFactory("Element");
			AZ::Component* elementComponent = elementFactory->CreateComponent();
			this->m_entity.first->AddComponent(elementComponent);*/

			auto transformFactory = gEnv->pLyShine->GetComponentFactory("Transform2d");
			AZ::Component* transformComponent = transformFactory->CreateComponent();
			this->m_entity.first->AddComponent(transformComponent);
		}
	};

	typedef LYSwingComponent LYPanel;
}

#endif