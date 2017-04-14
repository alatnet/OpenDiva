#ifndef _H_LYLABEL
#define _H_LYLABEL

#pragma once

#include "LYSwingComponent.h"

namespace LYSwing {
	class LYLabel : public LYSwingComponent {
	public:
		LYLabel(const LyShine::NameType& name, AZ::EntityId canvasId);
		LYLabel(const LyShine::NameType& name, UiCanvasInterface* canvas);
		LYLabel(const LyShine::NameType& name, LYSwingComponent * component);
	public:
		void SetColor(ColorF color);
		void SetText(const LyShine::StringType& text);
		void SetFont(const LyShine::StringType& font);
		void SetFontEffect(int effect);
		void SetFontSize(float size);
		void SetTextAlignment(IDraw2d::HAlign horizontalAlignment, IDraw2d::VAlign verticalAlignment);
		void SetOverflowMode(UiTextInterface::OverflowMode mode);
		void SetSelectionRange(int startIndex, int endIndex, ColorF color);
		void SetWrapText(UiTextInterface::WrapTextSetting setting);
		void SetTextWithFlags(const LyShine::StringType& text, UiTextInterface::SetTextFlags flags);

		ColorF GetColor();
		const LyShine::StringType GetText();
		const LyShine::StringType GetFont();
		int GetFontEffect();
		float GetFontSize();
		void GetTextAlignment(IDraw2d::HAlign& horizontalAlignment, IDraw2d::VAlign& verticalAlignment);
		UiTextInterface::OverflowMode GetOverflowMode();
		void GetSelectionRange(int& startIndex, int& endIndex);
		ColorF GetSelectionColor();
		UiTextInterface::WrapTextSetting GetWrapText();
		const LyShine::StringType GetTextWithFlags(UiTextInterface::GetTextFlags flags);
	private:
		void Initialize();
	};

	typedef LYLabel LYText;
}

#endif