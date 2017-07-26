#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "EffectResource.h"

namespace LYGame {
	//IDraw2d *EffectResource::iDraw2d = NULL;
	//IRenderer *EffectResource::iRenderer = NULL;

	//EffectResource::EffectResource(const char * folder) : m_ImgScale({ 1.0f, 1.0f }), m_PosScale({ 1.0f, 1.0f }) {
	//	CryLog("[EffectResource] Loading Effect Resources - Path: %s", folder);

	//	if (EffectResource::iRenderer == NULL) EffectResource::iRenderer = gEnv->pSystem->GetIRenderer();
	//	if (EffectResource::iDraw2d == NULL) EffectResource::iDraw2d = Draw2dHelper::GetDraw2d();

	//	//target sizes for effect images.
	//	this->m_EffectSizes[eET_Cross] = { 100, 100 };
	//	this->m_EffectSizes[eET_Diffuse] = { 88, 88 };
	//	this->m_EffectSizes[eET_RingFlare] = { 200, 200 };
	//	this->m_EffectSizes[eET_Loop] = { 70, 70 };
	//	this->m_EffectSizes[eET_NoNote] = { 66, 66 };
	//	this->m_EffectSizes[eET_Note] = { 75, 75 };
	//	this->m_EffectSizes[eET_Ripple] = { 75, 75 };

	//	{
	//		string effectConfig = folder;
	//		effectConfig.append("/");
	//		effectConfig.append(sEffectConfigFileName);
	//		XmlNodeRef config = gEnv->pSystem->LoadXmlFromFile(effectConfig);

	//		if (config) {
	//			#pragma omp parallel for
	//			for (int i = 0; i < eET_Count;i++) {
	//				XmlNodeRef node = config->findChild(sEffectsNames[i]);
	//				node->getAttr("size", this->m_EffectSizes[i]);

	//				unsigned int nodeCount = node->getChildCount();
	//				if (nodeCount > 0) {
	//					this->m_NumAnims[i] = nodeCount;
	//					this->m_pEffectAnims[i] = new EffectAnim[nodeCount];
	//					#pragma omp parallel for
	//					for (int i2 = 0; i2 < nodeCount; i2++) {
	//						XmlNodeRef anim = node->getChild(i2);
	//						anim->getAttr("time", this->m_pEffectAnims[i][i2].time);
	//						anim->getAttr("scale", this->m_pEffectAnims[i][i2].data.x);
	//						anim->getAttr("rot", this->m_pEffectAnims[i][i2].data.y);
	//						anim->getAttr("opacity", this->m_pEffectAnims[i][i2].data.z);
	//					}
	//				} else {
	//					this->m_NumAnims[i] = 0;
	//					this->m_pEffectAnims[i] = nullptr;
	//				}
	//			}
	//		}
	//	}

	//	#pragma omp parallel for
	//	for (int i = 0; i < eET_Count; i++) {
	//		//effect image itself
	//		string effectFilename = folder;
	//		effectFilename.append("/");
	//		effectFilename.append(sEffectFileNames[i]);

	//		this->m_pEffectTextures[i] = EffectResource::iRenderer->EF_LoadTexture(effectFilename, FT_DONT_STREAM);

	//		assert(m_pEffectTextures[i] != NULL);
	//	}

	//	CryLog("[EffectResource] Finished Loading Effect Resources");
	//}

	//EffectResource::~EffectResource() {
	//	for (int i = 0; i < eET_Count; i++) {
	//		this->m_pEffectTextures[i]->Release();
	//		if (this->m_NumAnims[i] > 0) delete this->m_pEffectAnims[i];
	//	}
	//}

	//void EffectResource::Draw(EEffectType type, Vec2 pos, float scale, float rot, float opacity) {
	//	if (scale <= 0.0f) return;
	//	pos.x = pos.x * m_PosScale.x;
	//	pos.y = pos.y * m_PosScale.y;

	//	EffectResource::iDraw2d->DrawImageAligned(
	//		this->m_pEffectTextures[type]->GetTextureID(),
	//		pos,
	//		Vec2(this->m_EffectSizes[type].x*m_ImgScale.x*scale, this->m_EffectSizes[type].y*m_ImgScale.y*scale),
	//		IDraw2d::HAlign::Center, IDraw2d::VAlign::Center,
	//		opacity,
	//		rot
	//	);
	//}

	//--------------------------------------------------------------------------------------------------------
	//Effect Resource V2
	//--------------------------------------------------------------------------------------------------------

	IDraw2d *EffectImage::iDraw2d = NULL;
	IRenderer *EffectImage::iRenderer = NULL;

	//Effect Image
	EffectImage::EffectImage(AZStd::string imgPath, AZ::Vector2 size) : m_size(size), m_ImgScale(1.0f, 1.0f), m_PosScale(1.0f, 1.0f) {
		if (EffectImage::iRenderer == NULL) EffectImage::iRenderer = gEnv->pSystem->GetIRenderer();
		if (EffectImage::iDraw2d == NULL) EffectImage::iDraw2d = Draw2dHelper::GetDraw2d();

		this->m_pImg = EffectImage::iRenderer->EF_LoadTexture(imgPath.c_str(), FT_DONT_STREAM);
		assert(this->m_pImg != NULL);
	}

	void EffectImage::Draw(AZ::Vector2 pos, float scale, float rot, float opacity) {
		EffectImage::iDraw2d->DrawImageAligned(
			this->m_pImg->GetTextureID(),
			pos, //AZ::Vector2(pos.x, pos.y),
			this->m_size*this->m_ImgScale*scale, //AZ::Vector2(this->m_size.x*m_ImgScale.x*scale, this->m_size.y*m_ImgScale.y*scale),
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center,
			opacity,
			rot
		);
	}

	//Effect Resource
	EffectResource::EffectResource(const char * folder) {
		CryLog("[EffectResourceV2] Loading Effect Resources - Path: %s", folder);

		this->m_pENoteEffect = nullptr;
		for (int i = 0; i < eEL_Count; i++) this->m_pEffects[i] = nullptr;

		AZStd::string effectConfig = folder;
		effectConfig.append("/");
		effectConfig.append(sEffectConfigFileName);
		XmlNodeRef config = gEnv->pSystem->LoadXmlFromFile(effectConfig.c_str());

		if (config) {
			//load the images
			XmlNodeRef images = config->findChild("images");
			for (int i = 0; i < images->getChildCount(); i++) {
				XmlNodeRef img = images->getChild(i);

				AZStd::string effectFilename = folder;
				effectFilename.append("/");
				effectFilename.append(img->getAttr("img"));
				
				Vec2 size;
				img->getAttr("size", size);

				this->m_images.insert({ img->getTag(), new EffectImage(effectFilename, AZ::Vector2(size.x,size.y)) });
			}

			//load the effect configuration
			XmlNodeRef effects = config->findChild("effects");
			#pragma omp parallel for
			for (int i = 0; i < eEL_Count; i++) { //for each effect
				m_pEffects[i] = new EffectDef(); //create a new effect definition
				m_pEffects[i]->name = sEffectsNamesV2[i]; //assign the effect name (not really needed but eh.)

				XmlNodeRef noteEffect = effects->findChild(sEffectsNamesV2[i]); //get the effect configuration

				if (noteEffect->getChildCount() != 0) { //if we have effects
					for (int eI = 0; eI < noteEffect->getChildCount(); eI++) { //for each effect
						XmlNodeRef effectAnimList = noteEffect->getChild(eI); //get the animation list

						EffectEntry effectEntry;
						effectEntry.img = this->m_images[effectAnimList->getAttr("img")]; //assign the EffectImage

						if (effectAnimList->getChildCount() != 0) { //if we have animation
							for (int aI = 0; aI < effectAnimList->getChildCount(); aI++) { //for each animation entry
								XmlNodeRef effectAnim = effectAnimList->getChild(aI); //get the animation entry
								EffectAnim anim;

								Vec2 pos;
								Vec3 sro;

								//get the data
								effectAnim->getAttr("time", anim.time);
								effectAnim->getAttr("x", pos.x);
								effectAnim->getAttr("y", pos.y);
								effectAnim->getAttr("scale", sro.x);
								effectAnim->getAttr("rot", sro.y);
								effectAnim->getAttr("opacity", sro.z);

								anim.pos.Set(pos.x, pos.y);
								anim.sro.Set(sro.x, sro.y, sro.z);

								effectEntry.anim.push_back(anim); //push back the animation data
							}
						}
						m_pEffects[i]->effects.push_back(effectEntry); //push back the effect entry
					}
				}
			}

			//load the effect configuration for enote
			XmlNodeRef enote = effects->findChild("enote");
			this->m_pENoteEffect = new EffectDef();

			for (int i = 0; i < enote->getChildCount(); i++) { //for each effect entry
				XmlNodeRef effectEntry = enote->getChild(i); //get the effect entry
				string effectName = effectEntry->getAttr("name"); //get the name of the effect

				EffectEntry entry;

				//check to see if we are a regular effect
				if (effectName.compare("enote") != 0) entry.img = this->m_images[effectEntry->getAttr("img")];

				//XmlNodeRef effectAnimList = effectEntry->getChild(i); //get the animation list
				if (effectEntry->getChildCount() != 0) { //if we have animation
					for (int aI = 0; aI < effectEntry->getChildCount(); aI++) { //for each animation entry
						XmlNodeRef effectAnim = effectEntry->getChild(aI); //get the animation entry
						EffectAnim anim;

						Vec2 pos;
						Vec3 sro;

						//get the data
						effectAnim->getAttr("time", anim.time);
						effectAnim->getAttr("x", pos.x);
						effectAnim->getAttr("y", pos.y);
						effectAnim->getAttr("scale", sro.x);
						effectAnim->getAttr("rot", sro.y);
						effectAnim->getAttr("opacity", sro.z);

						anim.pos.Set(pos.x, pos.y);
						anim.sro.Set(sro.x, sro.y, sro.z);

						entry.anim.push_back(anim); //push back the animation data
					}
				}

				this->m_pENoteEffect->effects.push_back(entry);
			}
		}

		CryLog("[EffectResourceV2] Finished Loading Effect Resources");
	}

	EffectResource::~EffectResource() {
		for (int i = 0; i < eEL_Count; i++) delete m_pEffects[i];
		this->m_images.clear(); //calls deconstructor.
	}
}