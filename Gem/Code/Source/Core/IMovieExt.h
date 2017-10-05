#ifndef _H_IMOVIE_EXT
#define _H_IMOVIE_EXT

#pragma once

//so very hacky just to get a raw animation track creation...

#include <IMovieSystem.h>

/*#pragma push_macro("aznew")
#undef aznew
#define aznew new*/

#include <Cinematics\AnimSplineTrack.h>

//#pragma pop_macro("aznew")

namespace OpenDiva {
	/*class CMovieExtTrackFloat : public TAnimSplineTrack<float> {
	public:
		AZ_CLASS_ALLOCATOR(CMovieExtTrackFloat, AZ::SystemAllocator, 0);
		AZ_RTTI(CMovieExtTrackFloat, "{E7942F87-7165-4974-90E1-C71A7F6CD627}", IAnimTrack);

		CMovieExtTrackFloat() {}
	};*/

	class IMovieExt {
	public:
		static IAnimTrack * CreateTrack(const CAnimParamType & paramType);
		static IAnimTrack * CreateTrack(const EAnimParamType paramType);
		static IAnimTrack * CreateTrack(const EAnimCurveType curveType);
	private:
		static IAnimTrack* CreateTrackInternalVector(const CAnimParamType& paramType, const EAnimValue animValue);
		static IAnimTrack* CreateTrackInternalQuat(const CAnimParamType& paramType);
		static IAnimTrack* CreateTrackInternalVector4(const CAnimParamType& paramType);
	};

	namespace IMovieExts {
		//all compound spline track code taken from Maestro Gem: Code\Source\Cinematics\CompoundSplineTrack
		//because raw track creation was removed from imoviesystem...
		//so a trimmed, ish, version just for track creation is implemented here from the original.
		#define MAX_SUBTRACKS 4

		class IMovieExtCompoundSplineTrack : public IAnimTrack {
		public:
			AZ_CLASS_ALLOCATOR(IMovieExtCompoundSplineTrack, AZ::SystemAllocator, 0);
			AZ_RTTI(IMovieExtCompoundSplineTrack, "{1F4CB1D5-28C3-4112-A6FC-75A562F42917}", IAnimTrack);

			IMovieExtCompoundSplineTrack(int nDims, EAnimValue inValueType, CAnimParamType subTrackParamTypes[MAX_SUBTRACKS]);
			IMovieExtCompoundSplineTrack();

			//////////////////////////////////////////////////////////////////////////
			// for intrusive_ptr support 
			void add_ref() override;
			void release() override;
			//////////////////////////////////////////////////////////////////////////

			void SetNode(IAnimNode* node) override;
			// Return Animation Node that owns this Track.
			IAnimNode* GetNode() override { return m_node; }

			virtual int GetSubTrackCount() const { return m_nDimensions; };
			virtual IAnimTrack* GetSubTrack(int nIndex) const;
			virtual const char* GetSubTrackName(int nIndex) const;
			virtual void SetSubTrackName(int nIndex, const char* name);

			virtual EAnimCurveType GetCurveType() { return eAnimCurveType_BezierFloat; };
			virtual EAnimValue GetValueType() { return m_valueType; };

			virtual const CAnimParamType& GetParameterType() const { return m_nParamType; };
			virtual void SetParameterType(CAnimParamType type) { m_nParamType = type; }

			virtual int GetNumKeys() const;
			virtual void SetNumKeys(int numKeys) { assert(0); };
			virtual bool HasKeys() const;
			virtual void RemoveKey(int num);

			virtual void GetKeyInfo(int key, const char*& description, float& duration);
			virtual int CreateKey(float time) { assert(0); return 0; };
			virtual int CloneKey(int fromKey) { assert(0); return 0; };
			virtual int CopyKey(IAnimTrack* pFromTrack, int nFromKey) { assert(0); return 0; };
			virtual void GetKey(int index, IKey* key) const { assert(0); };
			virtual float GetKeyTime(int index) const;
			virtual int FindKey(float time) { assert(0); return 0; };
			virtual int GetKeyFlags(int index) { assert(0); return 0; };
			virtual void SetKey(int index, IKey* key) { assert(0); };
			virtual void SetKeyTime(int index, float time);
			virtual void SetKeyFlags(int index, int flags) { assert(0); };
			virtual void SortKeys() { assert(0); };

			virtual bool IsKeySelected(int key) const;
			virtual void SelectKey(int key, bool select);

			virtual int GetFlags() { return m_flags; };
			virtual bool IsMasked(const uint32 mask) const { return false; }
			virtual void SetFlags(int flags) {
				m_flags = flags;
			}

			//////////////////////////////////////////////////////////////////////////
			// Get track value at specified time.
			// Interpolates keys if needed.
			//////////////////////////////////////////////////////////////////////////
			virtual void GetValue(float time, float& value, bool applyMultiplier = false);
			virtual void GetValue(float time, Vec3& value, bool applyMultiplier = false);
			virtual void GetValue(float time, Vec4& value, bool applyMultiplier = false);
			virtual void GetValue(float time, Quat& value);
			virtual void GetValue(float time, bool& value) { assert(0); };

			//////////////////////////////////////////////////////////////////////////
			// Set track value at specified time.
			// Adds new keys if required.
			//////////////////////////////////////////////////////////////////////////
			virtual void SetValue(float time, const float& value, bool bDefault = false, bool applyMultiplier = false);
			virtual void SetValue(float time, const Vec3& value, bool bDefault = false, bool applyMultiplier = false);
			void SetValue(float time, const Vec4& value, bool bDefault = false, bool applyMultiplier = false);
			virtual void SetValue(float time, const Quat& value, bool bDefault = false);
			virtual void SetValue(float time, const bool& value, bool bDefault = false) { assert(0); };

			virtual void OffsetKeyPosition(const Vec3& value);

			virtual void SetTimeRange(const Range& timeRange);

			virtual int NextKeyByTime(int key) const;

			void SetSubTrackName(const int i, const string& name) { assert(i < MAX_SUBTRACKS); m_subTrackNames[i] = name; }

		#ifdef MOVIESYSTEM_SUPPORT_EDITING
			virtual ColorB GetCustomColor() const {
				return m_customColor;
			}
			virtual void SetCustomColor(ColorB color) {
				m_customColor = color;
				m_bCustomColorSet = true;
			}
			virtual bool HasCustomColor() const {
				return m_bCustomColorSet;
			}
			virtual void ClearCustomColor() {
				m_bCustomColorSet = false;
			}
		#endif

			virtual void GetKeyValueRange(float& fMin, float& fMax) const {
				if (GetSubTrackCount() > 0) {
					m_subTracks[0]->GetKeyValueRange(fMin, fMax);
				}
			};
			virtual void SetKeyValueRange(float fMin, float fMax) {
				for (int i = 0; i < m_nDimensions; ++i) {
					m_subTracks[i]->SetKeyValueRange(fMin, fMax);
				}
			};

			void SetMultiplier(float trackMultiplier) override {
				for (int i = 0; i < m_nDimensions; ++i) {
					m_subTracks[i]->SetMultiplier(trackMultiplier);
				}
			}
		protected:
			int m_refCount;
			EAnimValue m_valueType;
			int m_nDimensions;
			AZStd::vector<AZStd::intrusive_ptr<IAnimTrack>> m_subTracks;
			int m_flags;
			CAnimParamType m_nParamType;
			AZStd::vector<AZStd::string> m_subTrackNames;

		#ifdef MOVIESYSTEM_SUPPORT_EDITING
			ColorB m_customColor;
			bool m_bCustomColorSet;
		#endif

			float PreferShortestRotPath(float degree, float degree0) const;
			int GetSubTrackIndex(int& key) const;
			IAnimNode* m_node;

			//we dont use this but cant compile without it.
			bool Serialize(XmlNodeRef& xmlNode, bool bLoading, bool bLoadEmptyTracks = true) { return false; }
			bool SerializeSelection(XmlNodeRef& xmlNode, bool bLoading, bool bCopySelected = false, float fTimeOffset = 0) { return false; }
		};

		//////////////////////////////////////////////////////////////////////////
		inline void IMovieExtCompoundSplineTrack::add_ref() {
			++m_refCount;
		}

		//////////////////////////////////////////////////////////////////////////
		inline void IMovieExtCompoundSplineTrack::release() {
			if (--m_refCount <= 0) {
				delete this;
			}
		}
	}
}

#endif