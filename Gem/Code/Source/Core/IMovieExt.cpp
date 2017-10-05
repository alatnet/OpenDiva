#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "IMovieExt.h"

namespace OpenDiva {
	IAnimTrack * IMovieExt::CreateTrack(const CAnimParamType & paramType) {
		IAnimTrack * ret = nullptr;
		EAnimParamType animParam = paramType.GetType();
		switch (animParam) {
		case eAnimParamType_FOV:
		case eAnimParamType_Visibility:
		case eAnimParamType_Float:
		case eAnimParamType_ShakeAmplitudeA:
		case eAnimParamType_ShakeAmplitudeB:
		case eAnimParamType_ShakeFrequencyA:
		case eAnimParamType_ShakeFrequencyB:
		case eAnimParamType_ShakeNoise:
		case eAnimParamType_ShakeWorking:
		case eAnimParamType_ShakeAmpAMult:
		case eAnimParamType_ShakeAmpBMult:
		case eAnimParamType_ShakeFreqAMult:
		case eAnimParamType_ShakeFreqBMult:
		case eAnimParamType_FocusDistance:
		case eAnimParamType_FocusRange:
		case eAnimParamType_BlurAmount:
		case eAnimParamType_FixedTimeStep:
		case eAnimParamType_NearZ:
		case eAnimParamType_TimeWarp:
		case eAnimParamType_PositionX:
		case eAnimParamType_PositionY:
		case eAnimParamType_PositionZ:
		case eAnimParamType_ColorR:
		case eAnimParamType_ColorG:
		case eAnimParamType_ColorB:
		case eAnimParamType_ScaleX:
		case eAnimParamType_ScaleY:
		case eAnimParamType_ScaleZ:
		case eAnimParamType_LightRadius:
		case eAnimParamType_LightDiffuseMult:
		case eAnimParamType_LightHDRDynamic:
		case eAnimParamType_LightSpecularMult:
		case eAnimParamType_LightSpecPercentage:
		case eAnimParamType_MaterialOpacity:
		case eAnimParamType_MaterialSmoothness:
		case eAnimParamType_MaterialEmissiveIntensity:
		case eAnimParamType_SunLongitude:
		case eAnimParamType_SunLatitude:
		case eAnimParamType_MoonLongitude:
		case eAnimParamType_MoonLatitude:
		case eAnimParamType_RotationX:
		case eAnimParamType_RotationY:
		case eAnimParamType_RotationZ:
			ret = aznew C2DSplineTrack;
			break;
		case eAnimParamType_Position:
		case eAnimParamType_LookAt:
		case eAnimParamType_Scale:
		case eAnimParamType_DepthOfField:
			ret = IMovieExt::CreateTrackInternalVector(paramType, EAnimValue::eAnimValue_Vector);
			break;
		case eAnimParamType_MaterialDiffuse:
		case eAnimParamType_MaterialSpecular:
		case eAnimParamType_MaterialEmissive:
		case eAnimParamType_LightDiffuse:
			ret = IMovieExt::CreateTrackInternalVector(paramType, EAnimValue::eAnimValue_RGB);
			break;
		case eAnimParamType_Rotation:
			ret = IMovieExt::CreateTrackInternalQuat(paramType);
			break;
		case eAnimParamType_TransformNoise:
		case eAnimParamType_ShakeMultiplier:
			ret = IMovieExt::CreateTrackInternalVector4(paramType);
			break;
		case eAnimParamType_ByString:
		case eAnimParamType_Event:
		case eAnimParamType_Camera:
		case eAnimParamType_Animation:
		case eAnimParamType_Sound:
		case eAnimParamType_Sequence:
		case eAnimParamType_Console:
		case eAnimParamType_Music:
		case eAnimParamType_TrackEvent:
		case eAnimParamType_Capture:
		case eAnimParamType_Goto:
		case eAnimParamType_CommentText:
		case eAnimParamType_ScreenFader:
		case eAnimParamType_TimeRanges:
		case eAnimParamType_Physics:
		case eAnimParamType_ProceduralEyes:
		case eAnimParamType_Mannequin:
		case eAnimParamType_GSMCache:
		case eAnimParamType_ShutterSpeed:
		case eAnimParamType_Physicalize:
		case eAnimParamType_PhysicsDriven:
		case eAnimParamType_User:
		case eAnimParamType_Invalid:
			break;
		}

		if (ret) ret->add_ref();
		return ret;
	}

	IAnimTrack * IMovieExt::CreateTrack(const EAnimParamType paramType) {
		return CreateTrack(CAnimParamType(paramType));
	}

	IAnimTrack * IMovieExt::CreateTrack(const EAnimCurveType curveType) {
		IAnimTrack * ret = nullptr;

		switch (curveType) {
		default:
		case eAnimCurveType_TCBFloat:
		case eAnimCurveType_BezierFloat:
			ret = aznew C2DSplineTrack;
			break;
		case eAnimCurveType_TCBVector:
			//compound track
			ret = IMovieExt::CreateTrackInternalVector(CAnimParamType(EAnimParamType::eAnimParamType_Position), EAnimValue::eAnimValue_Vector);
			break;
		case eAnimCurveType_TCBQuat:
			//compound track
			ret = IMovieExt::CreateTrackInternalQuat(CAnimParamType(EAnimParamType::eAnimParamType_Rotation));
			break;
		}

		if (ret) ret->add_ref();
		return ret;
	}

	IAnimTrack* IMovieExt::CreateTrackInternalVector(const CAnimParamType& paramType, const EAnimValue animValue) {
		CAnimParamType subTrackParamTypes[MAX_SUBTRACKS];
		for (unsigned int i = 0; i < MAX_SUBTRACKS; ++i) {
			subTrackParamTypes[i] = eAnimParamType_Float;
		}

		if (paramType == eAnimParamType_Position) {
			subTrackParamTypes[0] = eAnimParamType_PositionX;
			subTrackParamTypes[1] = eAnimParamType_PositionY;
			subTrackParamTypes[2] = eAnimParamType_PositionZ;
		} else if (paramType == eAnimParamType_Scale) {
			subTrackParamTypes[0] = eAnimParamType_ScaleX;
			subTrackParamTypes[1] = eAnimParamType_ScaleY;
			subTrackParamTypes[2] = eAnimParamType_ScaleZ;
		} else if (paramType == eAnimParamType_Rotation) {
			subTrackParamTypes[0] = eAnimParamType_RotationX;
			subTrackParamTypes[1] = eAnimParamType_RotationY;
			subTrackParamTypes[2] = eAnimParamType_RotationZ;
			IAnimTrack* pTrack = aznew IMovieExts::IMovieExtCompoundSplineTrack(3, eAnimValue_Quat, subTrackParamTypes);
			return pTrack;
		} else if (paramType == eAnimParamType_DepthOfField) {
			subTrackParamTypes[0] = eAnimParamType_FocusDistance;
			subTrackParamTypes[1] = eAnimParamType_FocusRange;
			subTrackParamTypes[2] = eAnimParamType_BlurAmount;
			IAnimTrack* pTrack = aznew IMovieExts::IMovieExtCompoundSplineTrack(3, eAnimValue_Vector, subTrackParamTypes);
			pTrack->SetSubTrackName(0, "FocusDist");
			pTrack->SetSubTrackName(1, "FocusRange");
			pTrack->SetSubTrackName(2, "BlurAmount");
			return pTrack;
		} else if (animValue == eAnimValue_RGB || paramType == eAnimParamType_LightDiffuse ||
			paramType == eAnimParamType_MaterialDiffuse || paramType == eAnimParamType_MaterialSpecular
			|| paramType == eAnimParamType_MaterialEmissive) {
			subTrackParamTypes[0] = eAnimParamType_ColorR;
			subTrackParamTypes[1] = eAnimParamType_ColorG;
			subTrackParamTypes[2] = eAnimParamType_ColorB;
			IAnimTrack* pTrack = aznew IMovieExts::IMovieExtCompoundSplineTrack(3, eAnimValue_RGB, subTrackParamTypes);
			pTrack->SetSubTrackName(0, "Red");
			pTrack->SetSubTrackName(1, "Green");
			pTrack->SetSubTrackName(2, "Blue");
			return pTrack;
		}

		return aznew IMovieExts::IMovieExtCompoundSplineTrack(3, eAnimValue_Vector, subTrackParamTypes);
	}

	IAnimTrack* IMovieExt::CreateTrackInternalQuat(const CAnimParamType& paramType) {
		CAnimParamType subTrackParamTypes[MAX_SUBTRACKS];
		if (paramType == eAnimParamType_Rotation) {
			subTrackParamTypes[0] = eAnimParamType_RotationX;
			subTrackParamTypes[1] = eAnimParamType_RotationY;
			subTrackParamTypes[2] = eAnimParamType_RotationZ;
		} else {
			// Unknown param type
			assert(0);
		}

		return aznew IMovieExts::IMovieExtCompoundSplineTrack(3, eAnimValue_Quat, subTrackParamTypes);
	}

	IAnimTrack* IMovieExt::CreateTrackInternalVector4(const CAnimParamType& paramType) {
		IAnimTrack* pTrack;

		CAnimParamType subTrackParamTypes[MAX_SUBTRACKS];

		// set up track subtypes
		if (paramType == eAnimParamType_TransformNoise
			|| paramType == eAnimParamType_ShakeMultiplier) {
			subTrackParamTypes[0] = eAnimParamType_ShakeAmpAMult;
			subTrackParamTypes[1] = eAnimParamType_ShakeAmpBMult;
			subTrackParamTypes[2] = eAnimParamType_ShakeFreqAMult;
			subTrackParamTypes[3] = eAnimParamType_ShakeFreqBMult;
		} else {
			// default to a Vector4 of floats
			for (unsigned int i = 0; i < MAX_SUBTRACKS; ++i) {
				subTrackParamTypes[i] = eAnimParamType_Float;
			}
		}

		// create track
		pTrack = aznew IMovieExts::IMovieExtCompoundSplineTrack(4, eAnimValue_Vector4, subTrackParamTypes);

		// label subtypes
		if (paramType == eAnimParamType_TransformNoise) {
			pTrack->SetSubTrackName(0, "Pos Noise Amp");
			pTrack->SetSubTrackName(1, "Pos Noise Freq");
			pTrack->SetSubTrackName(2, "Rot Noise Amp");
			pTrack->SetSubTrackName(3, "Rot Noise Freq");
		} else if (paramType == eAnimParamType_ShakeMultiplier) {
			pTrack->SetSubTrackName(0, "Amplitude A");
			pTrack->SetSubTrackName(1, "Amplitude B");
			pTrack->SetSubTrackName(2, "Frequency A");
			pTrack->SetSubTrackName(3, "Frequency B");
		}

		return pTrack;
	}

	namespace IMovieExts {
		IMovieExtCompoundSplineTrack::IMovieExtCompoundSplineTrack(int nDims, EAnimValue inValueType, CAnimParamType subTrackParamTypes[MAX_SUBTRACKS])
			: m_refCount(0) {
			assert(nDims > 0 && nDims <= MAX_SUBTRACKS);
			m_node = nullptr;
			m_nDimensions = nDims;
			m_valueType = inValueType;

			m_nParamType = eAnimNodeType_Invalid;
			m_flags = 0;

			m_subTracks.resize(MAX_SUBTRACKS);
			for (int i = 0; i < m_nDimensions; i++) {
				m_subTracks[i].reset(aznew C2DSplineTrack());
				m_subTracks[i]->SetParameterType(subTrackParamTypes[i]);

				if (inValueType == eAnimValue_RGB) {
					m_subTracks[i]->SetKeyValueRange(0.0f, 255.f);
				}
			}

			m_subTrackNames.resize(MAX_SUBTRACKS);
			m_subTrackNames[0] = "X";
			m_subTrackNames[1] = "Y";
			m_subTrackNames[2] = "Z";
			m_subTrackNames[3] = "W";

		#ifdef MOVIESYSTEM_SUPPORT_EDITING
			m_bCustomColorSet = false;
		#endif
		}

		//////////////////////////////////////////////////////////////////////////
		// Need default constructor for AZ Serialization
		IMovieExtCompoundSplineTrack::IMovieExtCompoundSplineTrack()
			: m_refCount(0)
			, m_nDimensions(0)
			, m_valueType(eAnimValue_Float)
		#ifdef MOVIESYSTEM_SUPPORT_EDITING
			, m_bCustomColorSet(false)
		#endif
		{
		}

		void IMovieExtCompoundSplineTrack::SetNode(IAnimNode* node) {
			m_node = node;
			for (int i = 0; i < m_nDimensions; i++) {
				m_subTracks[i]->SetNode(node);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::SetTimeRange(const Range& timeRange) {
			for (int i = 0; i < m_nDimensions; i++) {
				m_subTracks[i]->SetTimeRange(timeRange);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::GetValue(float time, float& value, bool applyMultiplier) {
			for (int i = 0; i < 1 && i < m_nDimensions; i++) {
				m_subTracks[i]->GetValue(time, value, applyMultiplier);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::GetValue(float time, Vec3& value, bool applyMultiplier) {
			for (int i = 0; i < m_nDimensions; i++) {
				float v = value[i];
				m_subTracks[i]->GetValue(time, v, applyMultiplier);
				value[i] = v;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::GetValue(float time, Vec4& value, bool applyMultiplier) {
			for (int i = 0; i < m_nDimensions; i++) {
				float v = value[i];
				m_subTracks[i]->GetValue(time, v, applyMultiplier);
				value[i] = v;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::GetValue(float time, Quat& value) {
			if (m_nDimensions == 3) {
				// Assume Euler Angles XYZ
				float angles[3] = { 0, 0, 0 };
				for (int i = 0; i < m_nDimensions; i++) {
					m_subTracks[i]->GetValue(time, angles[i]);
				}
				value = Quat::CreateRotationXYZ(Ang3(DEG2RAD(angles[0]), DEG2RAD(angles[1]), DEG2RAD(angles[2])));
			} else {
				assert(0);
				value.SetIdentity();
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::SetValue(float time, const float& value, bool bDefault, bool applyMultiplier) {
			for (int i = 0; i < m_nDimensions; i++) {
				m_subTracks[i]->SetValue(time, value, bDefault, applyMultiplier);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::SetValue(float time, const Vec3& value, bool bDefault, bool applyMultiplier) {
			for (int i = 0; i < m_nDimensions; i++) {
				m_subTracks[i]->SetValue(time, value[i], bDefault, applyMultiplier);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::SetValue(float time, const Vec4& value, bool bDefault, bool applyMultiplier) {
			for (int i = 0; i < m_nDimensions; i++) {
				m_subTracks[i]->SetValue(time, value[i], bDefault, applyMultiplier);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::SetValue(float time, const Quat& value, bool bDefault) {
			if (m_nDimensions == 3) {
				// Assume Euler Angles XYZ
				Ang3 angles = Ang3::GetAnglesXYZ(value);
				for (int i = 0; i < 3; i++) {
					float degree = RAD2DEG(angles[i]);
					if (false == bDefault) {
						// Try to prefer the shortest path of rotation.
						float degree0 = 0.0f;
						m_subTracks[i]->GetValue(time, degree0);
						degree = PreferShortestRotPath(degree, degree0);
					}
					m_subTracks[i]->SetValue(time, degree, bDefault);
				}
			} else {
				assert(0);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::OffsetKeyPosition(const Vec3& offset) {
			if (m_nDimensions == 3) {
				for (int i = 0; i < 3; i++) {
					IAnimTrack* pSubTrack = m_subTracks[i].get();
					// Iterate over all keys.
					for (int k = 0, num = pSubTrack->GetNumKeys(); k < num; k++) {
						// Offset each key.
						float time = pSubTrack->GetKeyTime(k);
						float value = 0;
						pSubTrack->GetValue(time, value);
						value = value + offset[i];
						pSubTrack->SetValue(time, value);
					}
				}
			} else {
				assert(0);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		IAnimTrack* IMovieExtCompoundSplineTrack::GetSubTrack(int nIndex) const {
			assert(nIndex >= 0 && nIndex < m_nDimensions);
			return m_subTracks[nIndex].get();
		}

		//////////////////////////////////////////////////////////////////////////
		const char* IMovieExtCompoundSplineTrack::GetSubTrackName(int nIndex) const {
			assert(nIndex >= 0 && nIndex < m_nDimensions);
			return m_subTrackNames[nIndex].c_str();
		}

		//////////////////////////////////////////////////////////////////////////
		void IMovieExtCompoundSplineTrack::SetSubTrackName(int nIndex, const char* name) {
			assert(nIndex >= 0 && nIndex < m_nDimensions);
			assert(name);
			m_subTrackNames[nIndex] = name;
		}

		//////////////////////////////////////////////////////////////////////////
		int IMovieExtCompoundSplineTrack::GetNumKeys() const {
			int nKeys = 0;
			for (int i = 0; i < m_nDimensions; i++) {
				nKeys += m_subTracks[i]->GetNumKeys();
			}
			return nKeys;
		}

		//////////////////////////////////////////////////////////////////////////
		bool IMovieExtCompoundSplineTrack::HasKeys() const {
			for (int i = 0; i < m_nDimensions; i++) {
				if (m_subTracks[i]->GetNumKeys()) {
					return true;
				}
			}
			return false;
		}

		float IMovieExtCompoundSplineTrack::PreferShortestRotPath(float degree, float degree0) const {
			// Assumes the degree is in (-PI, PI).
			assert(-181.0f < degree && degree < 181.0f);
			float degree00 = degree0;
			degree0 = fmod_tpl(degree0, 360.0f);
			float n = (degree00 - degree0) / 360.0f;
			float degreeAlt;
			if (degree >= 0) {
				degreeAlt = degree - 360.0f;
			} else {
				degreeAlt = degree + 360.0f;
			}
			if (fabs(degreeAlt - degree0) < fabs(degree - degree0)) {
				return degreeAlt + n * 360.0f;
			} else {
				return degree + n * 360.0f;
			}
		}

		int IMovieExtCompoundSplineTrack::GetSubTrackIndex(int& key) const {
			assert(key >= 0 && key < GetNumKeys());
			int count = 0;
			for (int i = 0; i < m_nDimensions; i++) {
				if (key < count + m_subTracks[i]->GetNumKeys()) {
					key = key - count;
					return i;
				}
				count += m_subTracks[i]->GetNumKeys();
			}
			return -1;
		}

		void IMovieExtCompoundSplineTrack::RemoveKey(int num) {
			assert(num >= 0 && num < GetNumKeys());
			int i = GetSubTrackIndex(num);
			assert(i >= 0);
			if (i < 0) {
				return;
			}
			m_subTracks[i]->RemoveKey(num);
		}

		void IMovieExtCompoundSplineTrack::GetKeyInfo(int key, const char*& description, float& duration) {
			static char str[64];
			duration = 0;
			description = str;
			const char* subDesc = NULL;
			float time = GetKeyTime(key);
			int m = 0;
			/// Using the time obtained, combine descriptions from keys of the same time
			/// in sub-tracks if any into one compound description.
			str[0] = 0;
			// A head case
			for (m = 0; m < m_subTracks[0]->GetNumKeys(); ++m) {
				if (m_subTracks[0]->GetKeyTime(m) == time) {
					float dummy;
					m_subTracks[0]->GetKeyInfo(m, subDesc, dummy);
					cry_strcat(str, subDesc);
					break;
				}
			}
			if (m == m_subTracks[0]->GetNumKeys()) {
				cry_strcat(str, m_subTrackNames[0].c_str());
			}
			// Tail cases
			for (int i = 1; i < GetSubTrackCount(); ++i) {
				cry_strcat(str, ",");
				for (m = 0; m < m_subTracks[i]->GetNumKeys(); ++m) {
					if (m_subTracks[i]->GetKeyTime(m) == time) {
						float dummy;
						m_subTracks[i]->GetKeyInfo(m, subDesc, dummy);
						cry_strcat(str, subDesc);
						break;
					}
				}
				if (m == m_subTracks[i]->GetNumKeys()) {
					cry_strcat(str, m_subTrackNames[i].c_str());
				}
			}
		}

		float IMovieExtCompoundSplineTrack::GetKeyTime(int index) const {
			assert(index >= 0 && index < GetNumKeys());
			int i = GetSubTrackIndex(index);
			assert(i >= 0);
			if (i < 0) {
				return 0;
			}
			return m_subTracks[i]->GetKeyTime(index);
		}

		void IMovieExtCompoundSplineTrack::SetKeyTime(int index, float time) {
			assert(index >= 0 && index < GetNumKeys());
			int i = GetSubTrackIndex(index);
			assert(i >= 0);
			if (i < 0) {
				return;
			}
			m_subTracks[i]->SetKeyTime(index, time);
		}

		bool IMovieExtCompoundSplineTrack::IsKeySelected(int key) const {
			assert(key >= 0 && key < GetNumKeys());
			int i = GetSubTrackIndex(key);
			assert(i >= 0);
			if (i < 0) {
				return false;
			}
			return m_subTracks[i]->IsKeySelected(key);
		}

		void IMovieExtCompoundSplineTrack::SelectKey(int key, bool select) {
			assert(key >= 0 && key < GetNumKeys());
			int i = GetSubTrackIndex(key);
			assert(i >= 0);
			if (i < 0) {
				return;
			}
			float keyTime = m_subTracks[i]->GetKeyTime(key);
			// In the case of compound tracks, animators want to
			// select all keys of the same time in the sub-tracks together.
			const float timeEpsilon = 0.001f;
			for (int k = 0; k < m_nDimensions; ++k) {
				for (int m = 0; m < m_subTracks[k]->GetNumKeys(); ++m) {
					if (fabs(m_subTracks[k]->GetKeyTime(m) - keyTime) < timeEpsilon) {
						m_subTracks[k]->SelectKey(m, select);
						break;
					}
				}
			}
		}

		int IMovieExtCompoundSplineTrack::NextKeyByTime(int key) const {
			assert(key >= 0 && key < GetNumKeys());
			float time = GetKeyTime(key);
			int count = 0, result = -1;
			float timeNext = FLT_MAX;
			for (int i = 0; i < GetSubTrackCount(); ++i) {
				for (int k = 0; k < m_subTracks[i]->GetNumKeys(); ++k) {
					float t = m_subTracks[i]->GetKeyTime(k);
					if (t > time) {
						if (t < timeNext) {
							timeNext = t;
							result = count + k;
						}
						break;
					}
				}
				count += m_subTracks[i]->GetNumKeys();
			}
			return result;
		}
	}
}