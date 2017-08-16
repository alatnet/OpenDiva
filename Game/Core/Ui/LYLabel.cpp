#include <StdAfx.h>
#include "LYSwingIncludes.h"
#include "LYLabel.h"

namespace LYSwing {

	LYLabel::LYLabel(const LyShine::NameType& name, AZ::EntityId canvasId) : LYSwingComponent(name, canvasId) {
		Initialize();
	}

	LYLabel::LYLabel(const LyShine::NameType& name, UiCanvasInterface* canvas) : LYSwingComponent(name, canvas) {
		Initialize();
	}

	LYLabel::LYLabel(const LyShine::NameType& name, LYSwingComponent * component) : LYSwingComponent(name, component) {
		Initialize();
	}

	void LYLabel::Initialize() {
		auto textFactory = gEnv->pLyShine->GetComponentFactory("Text");
		AZ::Component* textComponent = textFactory->CreateComponent();
		this->m_entity.first->AddComponent(textComponent);
	}

	void LYLabel::SetColor(ColorF color) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetColor, color);
	}

	void LYLabel::SetText(const LyShine::StringType& text) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetText, text);
	}

	void LYLabel::SetFont(const LyShine::StringType& font) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetFont, font);
	}

	void LYLabel::SetFontEffect(int effect) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetFontEffect, effect);
	}

	void LYLabel::SetFontSize(float size) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetFontSize, size);
	}

	void LYLabel::SetTextAlignment(IDraw2d::HAlign horizontalAlignment, IDraw2d::VAlign verticalAlignment) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetTextAlignment, horizontalAlignment, verticalAlignment);
	}

	void LYLabel::SetOverflowMode(UiTextInterface::OverflowMode mode) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetOverflowMode, mode);
	}

	void LYLabel::SetSelectionRange(int startIndex, int endIndex, ColorF color) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetSelectionRange, startIndex, endIndex, color);
	}

	void LYLabel::SetWrapText(UiTextInterface::WrapTextSetting setting) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetWrapText, setting);
	}

	void LYLabel::SetTextWithFlags(const LyShine::StringType& text, UiTextInterface::SetTextFlags flags) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, SetTextWithFlags, text, flags);
	}

	ColorF LYLabel::GetColor() {
		ColorF ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetColor);
		return ret;
	}

	const LyShine::StringType LYLabel::GetText() {
		LyShine::StringType ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetText);
		return ret;
	}

	const LyShine::StringType LYLabel::GetFont() {
		LyShine::StringType ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetFont);
		return ret;
	}

	int LYLabel::GetFontEffect() {
		int ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetFontEffect);
		return ret;
	}

	float LYLabel::GetFontSize() {
		float ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetFontSize);
		return ret;
	}

	void LYLabel::GetTextAlignment(IDraw2d::HAlign& horizontalAlignment, IDraw2d::VAlign& verticalAlignment) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, GetTextAlignment, horizontalAlignment, verticalAlignment);
	}

	UiTextInterface::OverflowMode LYLabel::GetOverflowMode() {
		UiTextInterface::OverflowMode ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetOverflowMode);
		return ret;
	}

	void LYLabel::GetSelectionRange(int& startIndex, int& endIndex) {
		EBUS_EVENT_ID(this->m_entity.second, UiTextBus, GetSelectionRange, startIndex, endIndex);
	}

	ColorF LYLabel::GetSelectionColor() {
		ColorF ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetSelectionColor);
		return ret;
	}

	UiTextInterface::WrapTextSetting LYLabel::GetWrapText() {
		UiTextInterface::WrapTextSetting ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetWrapText);
		return ret;
	}

	const LyShine::StringType LYLabel::GetTextWithFlags(UiTextInterface::GetTextFlags flags) {
		LyShine::StringType ret;
		EBUS_EVENT_ID_RESULT(ret, this->m_entity.second, UiTextBus, GetTextWithFlags, flags);
		return ret;
	}

}