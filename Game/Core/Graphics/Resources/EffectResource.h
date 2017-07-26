#ifndef _H_EFFECTRESOURCE_
#define _H_EFFECTRESOURCE_
#pragma once

#include "../../Bus/DivaSequenceBus.h"

namespace LYGame {
	static const char * sEffectConfigFileName = "effectConfig.xml";

	//static const char * sEffectFileNames[eET_Count] {
	//	"cross.dds",
	//	"diffuse.dds",
	//	"ringflare.dds",
	//	"loop.dds",
	//	"nonote.dds",
	//	"note.dds",
	//	"ripple.dds"
	//};

	//static const char * sEffectsNames[eET_Count] = {
	//	"cross",
	//	"diffuse",
	//	"ringflare",
	//	"loop",
	//	"nonote",
	//	"note",
	//	"ripple"
	//};

	////depricated
	////#pragma deprecated(EffectAnim)
	//struct EffectAnim {
	//	float time;
	//	Vec3 data;
	//	//ripple: scale, rot, opacity

	//	EffectAnim() : time(0), data(0.0f, 0.0f, 0.0f) {}
	//};

	////depricated
	////#pragma deprecated(EffectResource)
	//class EffectResource {
	//public:
	//	EffectResource(const char * folder);
	//	~EffectResource();
	//public:
	//	void Draw(EEffectType type, Vec2 pos, float scale = 1.0f, float rot = 0.0f, float opacity = 1.0f);
	//public:
	//	void setImgScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->m_ImgScale = scale; }
	//	void setPosScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->m_PosScale = scale; }
	//	void setScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->setImgScale(scale); this->setPosScale(scale); }

	//	EffectAnim getAnimData(EEffectType type, unsigned int index) { if (/*m_NumAnims[type] > 0 && index < m_NumAnims[type]*/Util::CheckBounds<unsigned int>(index, 0, m_NumAnims[type]-1)) return m_pEffectAnims[type][index]; else return EffectAnim(); }
	//	unsigned int getNumAnims(EEffectType type) { return m_NumAnims[type]; }
	//private:
	//	ITexture * m_pEffectTextures[eET_Count];
	//	Vec2 m_ImgScale, m_PosScale;
	//	Vec2 m_EffectSizes[eET_Count];
	//private:
	//	EffectAnim * m_pEffectAnims[eET_Count];
	//	unsigned int m_NumAnims[eET_Count];
	//private:
	//	static IRenderer * iRenderer;
	//	static IDraw2d * iDraw2d;
	//};

	//version 2 effect resource system

	//hash table of effect images
	//use tag as key
	//use google's sparse hash system?
	//https://github.com/sparsehash/sparsehash

	class EffectImage { //hash key based on tag string
	public:
		EffectImage(AZStd::string imgPath, AZ::Vector2 size);
		~EffectImage() { this->m_pImg->Release(); }
	public:
		void Draw(AZ::Vector2 pos, float scale = 1.0f, float rot = 0.0f, float opacity = 1.0f);
	public:
		void setImgScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { this->m_ImgScale = scale; }
		void setPosScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { this->m_PosScale = scale; }
		void setScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { this->setImgScale(scale); this->setPosScale(scale); }
	private:
		ITexture * m_pImg;
		AZ::Vector2 m_size;
		AZ::Vector2 m_ImgScale, m_PosScale;
	private:
		static IRenderer * iRenderer;
		static IDraw2d * iDraw2d;
	};

	struct EffectAnim {
		float time;
		AZ::Vector2 pos;
		AZ::Vector3 sro; //scale, rot, opacity

		EffectAnim() : pos(0.0f,0.0f), time(0), sro(0.0f, 0.0f, 0.0f) {}
	};

	struct EffectEntry {
		AZStd::string name;
		EffectImage * img; //on loading data, grab effect image from hash table
		AZStd::vector<EffectAnim> anim;

		EffectEntry() : name(""), img(nullptr), anim() {}
		~EffectEntry() { img = nullptr; } //just to be sure that we are not deleting the image instead of derreferencing it.
	};

	struct EffectDef {
		AZStd::string name;
		AZStd::vector<EffectEntry> effects;
	};

	static const char * sEffectsNamesV2[eEL_Count] = {
		"cool",
		"fine",
		"safe",
		"sad",
		"worse",
		"swipeL",
		"swipeR",
		"swipeLEXTick",
		"swipeREXTick"
	};

	class EffectResource {
	public:
		EffectResource(const char * folder);
		~EffectResource(); //delete images from hash table;
	public:
		EffectDef * getEffect(EEffectList effect) { return this->m_pEffects[effect]; }
		EffectDef * getENoteEffect() { return this->m_pENoteEffect; }
	public:
		void setImgScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { for (AZStd::pair<AZStd::string, EffectImage *> effImg : m_images) effImg.second->setImgScale(scale); }
		void setPosScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { for (AZStd::pair<AZStd::string, EffectImage *> effImg : m_images) effImg.second->setPosScale(scale); }
		void setScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { for (AZStd::pair<AZStd::string, EffectImage *> effImg : m_images) effImg.second->setScale(scale); }
	private:
		AZStd::unordered_map<AZStd::string,EffectImage *> m_images; //load images once
		EffectDef * m_pEffects[eEL_Count];
		EffectDef * m_pENoteEffect;
	};
}

#endif //_H_EFFECTRESOURCE_