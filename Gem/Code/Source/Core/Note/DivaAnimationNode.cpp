#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "DivaAnimationNode.h"

#include <AlternativeAudio\DSP\VolumeDSPBus.h>

//how fast the fade in/out of the song should be in seconds
#define DIVAFADETIME 1.0f

//how long to delay the fade at the end of the song in seconds
#define DIVAFADEDELAY 2.0f

//when a transition from one section to another should happen in seconds
#define DIVAZONETRANSITION 2.0f

namespace OpenDiva {
	//-----------------------------------------------------------------------
	// DivaAnimationNode
	//-----------------------------------------------------------------------
	DivaAnimationNode::DivaAnimationNode(ResourceCollection * rc) :
		m_pRC(rc),
		m_pSequence(nullptr),
		m_pOwner(nullptr),
		m_pParentNode(nullptr),
		m_pEvents(nullptr),
		m_pFader(nullptr)
	{
		this->m_hitNote = this->m_renderRangeStart = this->m_renderRangeEnd = 0;

		//this->m_seq = gEnv->pMovieSystem->CreateSequence("DivaSequence", false, 0U, eSequenceType_SequenceComponent);
		//this->m_seq->AddRef();

		//this->m_seq->SetFlags(IAnimSequence::eSeqFlags_NoAbort /*| IAnimSequence::eSeqFlags_CutScene*/ | IAnimSequence::eSeqFlags_OutOfRangeConstant | IAnimSequence::eSeqFlags_NoMPSyncingNeeded | IAnimSequence::eSeqFlags_NoSpeed);

		//use an anim node to call DivaSequences Animate and Render functions.
		//this->m_DivaSeqRedirect = new DivaSequenceRedirect(this);
		//this->m_DivaSeqRedirect->AddRef();
		//this->m_seq->AddNode(this->m_DivaSeqRedirect);

		//setup the zone events track
		//this->m_pEvents = this->m_seq->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		//this->m_pEvents->CreateDefaultTracks();

		//setup the lyrics events track
		//this->m_pLyricsEvents = this->m_seq->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		//this->m_pLyricsEvents->CreateDefaultTracks();

		/*IAnimSequence* seq = gEnv->pMovieSystem->CreateSequence("",false,0U,eSequenceType_SequenceComponent);
		IAnimNode * events = seq->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		IAnimNode * ent = seq->CreateNode(EAnimNodeType::eAnimNodeType_AzEntity);*/

		this->m_Music.m_idVocal = this->m_Music.m_idMelody = this->m_Music.m_idSong = -1;
		this->m_Music.m_wrongVol = false;
		this->m_Music.m_type = true;
		this->m_Music.m_audioVocal = this->m_Music.m_audioMelody = this->m_Music.m_audioSong = nullptr;
		this->m_Music.vocalDSP = nullptr;

		m_prevMusicTime = 0.0f;
		m_musicDeltaTotal = 0.0f;

		this->m_pEvents = new DivaEventSystem();

		DivaSequenceJudgeBus::Handler::BusConnect();
		DivaSequenceEffectsBus::Handler::BusConnect();
		DivaHudHitScoreBus::Handler::BusConnect();
		DivaAudioEventsBus::Handler::BusConnect();
	}

	DivaAnimationNode::~DivaAnimationNode() {
		DivaSequenceJudgeBus::Handler::BusDisconnect();
		DivaSequenceEffectsBus::Handler::BusDisconnect();
		DivaHudHitScoreBus::Handler::BusDisconnect();
		DivaAudioEventsBus::Handler::BusDisconnect();

		delete this->m_pEvents;

		for (DivaNoteBaseNode * n : this->m_NoteNodes) n->Release();
		this->m_NoteNodes.clear();

		//clear all particles
		this->m_Particles.clear();

		if (this->m_Music.m_audioVocal) delete this->m_Music.m_audioVocal;
		if (this->m_Music.m_audioMelody) delete this->m_Music.m_audioMelody;
		if (this->m_Music.m_audioSong) delete this->m_Music.m_audioSong;
		if (this->m_Music.vocalDSP) delete this->m_Music.vocalDSP;
	}

	//timeline
	/*
	//start of timeline
	0     DIVAFADETIME
	|-----|----->
	^     ^
	|     fade in end
	|     song start event
	song audio start (event)
	fade in start

	//end of timeline
	     endTime
	     |     endTime+DIVAFADETIME
	     v     v     endTime+(DIVAFADETIME*2)+DIVAFADEDELAY
	<----|-----|-----|
	     ^     ^     ^
	     |     |     fade out end
	     |     |     song audio end event
	     |     fade out start
	     |     song end event
	     Tech/Change exit event
	*/

	bool DivaAnimationNode::InitDemo(SongInfo::Global songinfo) {
		this->m_pEvents->AddEvent(
			0,
			DivaAnimationNode::SongEvents,
			(void *)"AudioStart"
		);

		this->m_pEvents->AddEvent(
			DIVAFADETIME,
			DivaAnimationNode::SongEvents,
			(void *)"Start"
		);

		float endTime = songinfo.length;

		//set beginning fade
		IAnimTrack * faderTrack = this->m_pFader->GetTrackByIndex(0);
		IScreenFaderKey fadeKeyIn;
		fadeKeyIn.time = 0;
		fadeKeyIn.m_fadeTime = DIVAFADETIME;
		fadeKeyIn.m_fadeColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		fadeKeyIn.m_fadeType = IScreenFaderKey::eFT_FadeIn;
		faderTrack->SetKey(0, &fadeKeyIn);

		//set ending fade
		IScreenFaderKey fadeKeyOut;
		fadeKeyOut.time = endTime + DIVAFADETIME + DIVAFADEDELAY;
		fadeKeyOut.m_fadeTime = DIVAFADETIME;
		fadeKeyOut.m_fadeColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		fadeKeyOut.m_fadeType = IScreenFaderKey::eFT_FadeOut;
		faderTrack->SetKey(1, &fadeKeyOut);

		Range timeRange = Range(0, endTime + (DIVAFADETIME*2.0f) + DIVAFADEDELAY);
		this->m_pSequence->SetTimeRange(timeRange); //+offset
		this->SetTimeRange(timeRange); //+offset

		this->m_pEvents->AddEvent(
			endTime + DIVAFADETIME,
			DivaAnimationNode::SongEvents,
			(void *)"End"
		);

		this->m_pEvents->AddEvent(
			endTime + (DIVAFADETIME*2.0f) + DIVAFADEDELAY,
			DivaAnimationNode::SongEvents,
			(void *)"AudioEnd"
		);

		return true;
	}

	bool DivaAnimationNode::InitNotes(NoteFile *noteFile, SongInfo::Global songinfo) {
		unsigned int numNotes = noteFile->GetNumNotes();

		NoteEntryBPM * currBPM = nullptr;

		if (noteFile->GetNote(0)->getClassType() == eNHT_BPM) {
			currBPM = (NoteEntryBPM*)(noteFile->GetNote(0));

			//float endTime = 0;
			SectionType sectionEvent = eST_Norm;
			unsigned int techZoneNotes = 0;
			AZStd::vector<unsigned int> techZoneNotesVector;

			this->m_pEvents->Clear();

			this->m_pEvents->AddEvent(
				0,
				DivaAnimationNode::SongEvents,
				(void *)"AudioStart"
			);

			this->m_pEvents->AddEvent(
				DIVAFADETIME,
				DivaAnimationNode::SongEvents,
				(void *)"Start"
			);

			//set beginning fade
			IAnimTrack * faderTrack = this->m_pFader->GetTrackByIndex(0);
			IScreenFaderKey fadeKeyIn;
			fadeKeyIn.time = 0;
			fadeKeyIn.m_fadeTime = DIVAFADETIME;
			fadeKeyIn.m_fadeColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			fadeKeyIn.m_fadeType = IScreenFaderKey::eFT_FadeIn;
			faderTrack->SetKey(0, &fadeKeyIn);

			for (int i = 1; i < numNotes; i++) {
				NoteEntry *currNote = noteFile->GetNote(i);

				if (currNote->getClassType() == eNHT_BPM) {
					currBPM = (NoteEntryBPM*)currNote;
				} else {
					DivaNoteBaseNode* node = DivaNoteNodeFactory::getFactory().newNoteNode(std::string(currNote->getTagName().c_str()), this->m_pRC, currNote); //create a new diva note
					if (node != nullptr) { // if the note was created ok
						node->AddRef(); //add a reference (smart pointer thing)
						node->Init(*currBPM); //initialize it with the current bpm
						this->m_NoteNodes.push_back(node); //add this note to the list of notes to render
						//if (endTime < node->GetTimeRange().end) endTime = node->GetTimeRange().end; //check end times

						techZoneNotes++; //increase the number of tech zone notes

						//set zone events
						if (currNote->sType != sectionEvent) { //if the section is different
							float time = node->GetTimeRange().start + DIVAFADETIME; //+offset

							//set event
							if (currNote->sType == eST_Norm && sectionEvent == eST_Tech) { //if we are exiting tech zone and entering normal
								techZoneNotesVector.push_back(techZoneNotes);
								techZoneNotes = 0;

								this->m_pEvents->AddEvent(
									time,
									DivaAnimationNode::ZonesEvents,
									(void *)"TechExit"
								);
							} else if (currNote->sType == eST_Tech && sectionEvent == eST_Norm) { //if we are entering tech zone
								techZoneNotes = 0;
								this->m_pEvents->AddEvent(
									time - DIVAZONETRANSITION,
									DivaAnimationNode::ZonesEvents,
									(void *)"TechEnter"
								);
							} else if (currNote->sType == eST_Chance && sectionEvent == eST_Norm) { //if we are entering chance time zone
								techZoneNotes = 0;
								this->m_pEvents->AddEvent(
									time - DIVAZONETRANSITION,
									DivaAnimationNode::ZonesEvents,
									(void *)"ChanceEnter"
								);
							} else if (currNote->sType == eST_Norm && sectionEvent == eST_Chance) { //if we are exiting chance time and entering normal
								techZoneNotes = 0;

								this->m_pEvents->AddEvent(
									time,
									DivaAnimationNode::ZonesEvents,
									(void *)"ChanceExit"
								);
							} else if (currNote->sType == eST_Tech && sectionEvent == eST_Chance) { //if we are swaping zones (chance to tech)
								techZoneNotes = 0;

								this->m_pEvents->AddEvent(
									time,
									DivaAnimationNode::ZonesEvents,
									(void *)"ChanceExit"
								);

								this->m_pEvents->AddEvent(
									time - DIVAZONETRANSITION,
									DivaAnimationNode::ZonesEvents,
									(void *)"TechEnter"
								);
							} else if (currNote->sType == eST_Chance && sectionEvent == eST_Tech) { //if we are swaping zones (tech to chance)
								techZoneNotesVector.push_back(techZoneNotes);
								techZoneNotes = 0;

								this->m_pEvents->AddEvent(
									time,
									DivaAnimationNode::ZonesEvents,
									(void *)"TechExit"
								);

								this->m_pEvents->AddEvent(
									time - DIVAZONETRANSITION,
									DivaAnimationNode::ZonesEvents,
									(void *)"ChanceEnter"
								);
							}

							sectionEvent = currNote->sType;
						}
					}
				}
			}

			float endTime = songinfo.length;

			//make sure we exit out of a zone
			if (sectionEvent != eST_Norm) {
				if (sectionEvent == eST_Tech) {
					techZoneNotesVector.push_back(techZoneNotes);

					this->m_pEvents->AddEvent(
						endTime,
						DivaAnimationNode::ZonesEvents,
						(void *)"TechExit"
					);
				} else {
					this->m_pEvents->AddEvent(
						endTime,
						DivaAnimationNode::ZonesEvents,
						(void *)"ChanceExit"
					);
				}
			}

			//set ending fade
			IScreenFaderKey fadeKeyOut;
			fadeKeyOut.time = endTime + DIVAFADETIME + DIVAFADEDELAY;
			fadeKeyOut.m_fadeTime = DIVAFADETIME;
			fadeKeyOut.m_fadeColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			fadeKeyOut.m_fadeType = IScreenFaderKey::eFT_FadeOut;
			faderTrack->SetKey(1, &fadeKeyOut);

			//notify busses.
			EBUS_EVENT(DivaJudgeBus, SetTotalNotes, this->m_NoteNodes.size());
			EBUS_EVENT(DivaHudCompletionBus, SetCompletion, 0, this->m_NoteNodes.size());
			EBUS_EVENT(DivaJudgeBus, SetTechZoneNotes, techZoneNotesVector);

			//find effect animation time and whichever biggest ending time is, tack it onto the end time of note nodes.
			Range timeRange = Range(0, endTime + (DIVAFADETIME*2.0f) + DIVAFADEDELAY);
			this->m_pSequence->SetTimeRange(timeRange); //+offset
			this->SetTimeRange(timeRange); //+offset

			this->m_pEvents->AddEvent(
				endTime + DIVAFADETIME,
				DivaAnimationNode::SongEvents,
				(void *)"End"
			);

			this->m_pEvents->AddEvent(
				endTime + (DIVAFADETIME*2.0f) + DIVAFADEDELAY,
				DivaAnimationNode::SongEvents,
				(void *)"AudioEnd"
			);
		} else {
			//error out
			//first note should ALWAYS be bpm note
			return false;
		}

		return true;
	}

	bool DivaAnimationNode::InitLyrics(LyricsFile * lyrics) {
		this->m_lyrics = lyrics;

		for (unsigned int i = 0; i < lyrics->GetNumLines(); i++) {
			LyricsFile::LineEntry *entry = lyrics->GetLine(i);
			this->m_pEvents->AddEvent(
				entry->time,
				DivaAnimationNode::LyricEvents,
				entry
			);
		}

		for (unsigned int i = 0; i < lyrics->GetNumSubtexts(); i++) {
			LyricsFile::SubtextEntry *entry = lyrics->GetSubtext(i);
			this->m_pEvents->AddEvent(
				entry->start,
				DivaAnimationNode::SubtextEvents,
				entry
			);
		}

		return true;
	}

	bool DivaAnimationNode::InitAudio(SongInfo::Global songinfo) {
		//set the library to use
		AZ::Crc32 lib = AZ::Crc32("libsndfile");
		if (!songinfo.lib.empty()) lib = AZ::Crc32(songinfo.lib.c_str());

		//song and melody
		if (!songinfo.vocal.empty() && !songinfo.melody.empty()) {
			EBUS_EVENT_RESULT(
				this->m_Music.m_audioMelody,
				AlternativeAudio::AlternativeAudioSourceBus,
				NewAudioSource,
				lib,
				songinfo.melody.c_str(),
				nullptr
			);
			EBUS_EVENT_RESULT(
				this->m_Music.m_audioVocal,
				AlternativeAudio::AlternativeAudioSourceBus,
				NewAudioSource,
				lib,
				songinfo.vocal.c_str(),
				nullptr
			);

			if (!this->m_Music.m_audioMelody && !this->m_Music.m_audioVocal) {
				if (this->m_Music.m_audioMelody) delete this->m_Music.m_audioMelody;
				if (this->m_Music.m_audioVocal) delete this->m_Music.m_audioVocal;
				CLOG("Error loading melody and vocals.");
				return false;
			}

			EBUS_EVENT_RESULT(
				this->m_Music.vocalDSP,
				AlternativeAudio::AlternativeAudioDSPBus,
				NewDSPEffect,
				AZ_CRC("AAVolumeControl", 0x722dd2a9),
				nullptr
			);

			if (!this->m_Music.vocalDSP) {
				if (this->m_Music.m_audioMelody) delete this->m_Music.m_audioMelody;
				if (this->m_Music.m_audioVocal) delete this->m_Music.m_audioVocal;
				CLOG("Error creating vocal dsp.");
				return false;
			}

			this->m_Music.m_audioVocal->AddEffectFreeSlot(this->m_Music.vocalDSP);

			EBUS_EVENT_ID(this->m_Music.vocalDSP, AlternativeAudio::DSP::VolumeDSPBus, SetVol, 1.0f);

			this->m_Music.m_type = true;

			return true;
		} else if (!songinfo.song.empty()) { //song only
			EBUS_EVENT_RESULT(
				this->m_Music.m_audioSong,
				AlternativeAudio::AlternativeAudioSourceBus,
				NewAudioSource,
				lib,
				songinfo.song.c_str(),
				nullptr
			);

			if (!this->m_Music.m_audioSong) {
				CLOG("Error loading song.");
				return false;
			}

			this->m_Music.m_type = false;

			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------
	// Buses
	//-----------------------------------------------------------------------
	//particle stuff
	void DivaAnimationNode::PushbackRating(AZ::Vector2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong) {
		this->m_Particles.push_back(new RatingParticle(this->m_pRC, pos, nType, hitscore, combo, wrong, RATINGPARTICLE_LIFE));
	}

	void DivaAnimationNode::PushbackEffect(AZ::Vector2 pos, EEffectList eff) {
		this->m_Particles.push_back(new EffectParticle(this->m_pRC, pos, eff));
	}

	void DivaAnimationNode::PushbackHoldMulti(AZ::Vector2 pos, unsigned int score, EHitScore hitscore, bool wrong) {
		this->m_Particles.push_back(new HoldMultiParticle(this->m_pRC, pos, score, hitscore, wrong));
	}

	void DivaAnimationNode::PushbackSubtext(LyricsFile::SubtextEntry* entry) {
		IDraw2d::TextOptions options = gEnv->pLyShine->GetDraw2d()->GetDefaultTextOptions();

		IFFont* sfont = this->m_lyrics->GetFont(entry->font);
		sfont = (sfont) ? sfont : options.font;
		int sfonte = (sfont) ? entry->effect : options.effectIndex;

		this->m_Particles.push_back(
			new SubtextParticle(
				entry->text,
				entry->pos,
				entry->size,
				entry->rot,
				sfont,
				sfonte,
				this->m_lyrics->GetColor(entry->color),
				entry->end - entry->start
			)
		);
	}

	void DivaAnimationNode::UpdateNoteHit() {
		this->m_hitNote++; //we are done scoring this note, on to the next one.
		if (this->m_hitNote >= this->m_NoteNodes.size()) this->m_hitNote = this->m_NoteNodes.size() - 1;
	}
	
	// DivaHudHitScoreGroup
	void DivaAnimationNode::SetHitScore(EHitScore hs, bool wrong) {
		this->m_Music.m_wrongVol = wrong;
		if (wrong) { //if the hit was wrong
			if (this->m_Music.vocalDSP)
				EBUS_EVENT_ID(this->m_Music.vocalDSP, AlternativeAudio::DSP::VolumeDSPBus, SetVol, 0.5f);
		} else { //else if the hit was correct
			if (this->m_Music.vocalDSP)
				EBUS_EVENT_ID(this->m_Music.vocalDSP, AlternativeAudio::DSP::VolumeDSPBus, SetVol, 1.0f);
		}
	}
	// ~DivaHudHitScoreGroup

	//-----------------------------------------------------------------------
	//Input
	//-----------------------------------------------------------------------
	void DivaAnimationNode::OnCross(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnCross(mode, value);
	}
	void DivaAnimationNode::OnCircle(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnCircle(mode, value);
	}
	void DivaAnimationNode::OnSquare(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnSquare(mode, value);
	}
	void DivaAnimationNode::OnTriangle(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnTriangle(mode, value);
	}
	void DivaAnimationNode::OnLeft(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnLeft(mode, value);
	}
	void DivaAnimationNode::OnRight(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnRight(mode, value);
	}
	void DivaAnimationNode::OnUp(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnUp(mode, value);
	}
	void DivaAnimationNode::OnDown(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnDown(mode, value);
	}
	void DivaAnimationNode::OnStar(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnStar(mode, value);
	}
	void DivaAnimationNode::OnSwipeL(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnSwipeL(mode, value);
	}
	void DivaAnimationNode::OnSwipeR(LyInputEventType mode, float value) {
		m_NoteNodes[m_hitNote]->OnSwipeR(mode, value);
	}

	//-----------------------------------------------------------------------
	// CAnimNode has these.
	//-----------------------------------------------------------------------
	void DivaAnimationNode::OnStart() {
		this->OnAudioStart();
	}
	void DivaAnimationNode::OnReset() {
		this->OnAudioEnd();

		this->m_musicDeltaTotal = 0.0f;
		this->m_prevMusicTime = 0.0f;

		this->m_pEvents->Reset();
		for (DivaNoteBaseNode* node : this->m_NoteNodes) node->Reset();
		this->m_Particles.clear();

		this->m_renderRangeEnd = this->m_renderRangeStart = this->m_hitNote = 0;

		EBUS_EVENT(DivaEventsBus, OnSongReset);
	}
	void DivaAnimationNode::OnResetHard() { OnReset(); }
	void DivaAnimationNode::OnPause() {
		this->OnAudioPause();
		EBUS_EVENT(DivaEventsBus, OnSongPause);
	}
	void DivaAnimationNode::OnResume() {
		EBUS_EVENT(DivaEventsBus, OnSongResume);
		this->OnAudioResume();
	}
	void DivaAnimationNode::OnStop() {
		this->OnAudioEnd();
		this->m_pEvents->Reset();

		this->m_musicDeltaTotal = 0.0f;
		this->m_prevMusicTime = 0.0f;

		for (DivaNoteBaseNode* node : this->m_NoteNodes) node->Reset();
		this->m_Particles.clear();

		this->m_renderRangeEnd = this->m_renderRangeStart = this->m_hitNote = 0;
	}
	void DivaAnimationNode::OnLoop() {
	}

	//-----------------------------------------------------------------------
	// Inherited via IAnimNode
	//-----------------------------------------------------------------------
	void DivaAnimationNode::Animate(SAnimContext & ec) {
		this->m_pEvents->Update(ec.time); //update the events system.

		SAnimContext animContext = ec;

		//music and video syncing
		if (this->m_Music.m_type) {
			AlternativeAudio::AudioSourceTime melodyTime, vocalTime;
			if (this->m_Music.m_idMelody != -1 && this->m_Music.m_audioMelody) //get the time for melody
				EBUS_EVENT_RESULT(melodyTime, AlternativeAudio::AlternativeAudioDeviceBus, GetTime, this->m_Music.m_idMelody);
			if (this->m_Music.m_idVocal != -1 && this->m_Music.m_audioVocal) //get the time for vocal
				EBUS_EVENT_RESULT(vocalTime, AlternativeAudio::AlternativeAudioDeviceBus, GetTime, this->m_Music.m_idVocal);
			animContext.time = (float)((melodyTime.totalSec + vocalTime.totalSec) / 2.0f); //average the times and set the current time for the context.
		} else {
			AlternativeAudio::AudioSourceTime musicTime;
			if (this->m_Music.m_idSong != -1 && this->m_Music.m_audioSong) //get the time for song
				EBUS_EVENT_RESULT(musicTime, AlternativeAudio::AlternativeAudioDeviceBus, GetTime, this->m_Music.m_idSong);
			animContext.time = (float)musicTime.totalSec; //set the current time for the context.
		}

		//music and video smoothing. (for when there is high audio latency)
		if (m_prevMusicTime != animContext.time) { //when the time reported from the music changes
			m_prevMusicTime = animContext.time; //update the prev music time
			m_musicDeltaTotal = 0.0f; //reset delta total
		} else { //otherwise
			//update delta total
			if (m_musicDeltaTotal == 0.0f) m_musicDeltaTotal = ec.dt; //set initial delta time
			else m_musicDeltaTotal = ((m_musicDeltaTotal + ec.dt) / 2.0f); //average out the delta times
			animContext.time += m_musicDeltaTotal; //add delta total to reported music time
		}

		//animContext.time += offset;

		//update nodes
		if (this->m_NoteNodes.size() > 0) {
			//animate first then shift ranges
			for (unsigned int i = this->m_renderRangeStart; i <= this->m_renderRangeEnd; i++) this->m_NoteNodes[i]->Animate(animContext);

			//shift the start range to a note node that needs to render
			while (this->m_NoteNodes[this->m_renderRangeStart]->NeedToRender(animContext.time) == false) {
				this->m_renderRangeStart++;

				if (this->m_renderRangeStart >= this->m_renderRangeEnd) {
					this->m_renderRangeStart = this->m_renderRangeEnd;
					break;
				}
			}

			//shift the end range to a note node that doesnt need to render
			while (this->m_NoteNodes[this->m_renderRangeEnd]->NeedToRender(animContext.time) == true) {
				this->m_renderRangeEnd++;

				if (this->m_renderRangeEnd > this->m_NoteNodes.size() - 1) {
					this->m_renderRangeEnd = this->m_NoteNodes.size() - 1;
					break;
				}
			}
		}

		//update particles
		if (this->m_Particles.size() > 0) {
			for (int i = 0; i < this->m_Particles.size(); i++) {
				ParticleBase * p = this->m_Particles.at(i);
				p->Tick(ec.dt);
				if (!p->Alive()) {
					this->m_Particles.erase(AZStd::remove(m_Particles.begin(), m_Particles.end(), p), m_Particles.end());
					i--;
					delete p;
				}
			}
		}
	}
	void DivaAnimationNode::Render() {
		OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
		//render particles
		if (this->m_Particles.size() > 0) for (ParticleBase* particle : this->m_Particles) particle->Render();

		//render nodes
		if (this->m_NoteNodes.size() > 0)
			for (int i = this->m_renderRangeEnd; i != this->m_renderRangeStart - 1; i--)
				this->m_NoteNodes[i]->Render(); //reverse render so that current note to hit is on top.
		OD_Draw2d::getDraw2D().EndDraw2d();
	}

	//-----------------------------------------------------------------------
	// Inherited via IAnimNode
	//-----------------------------------------------------------------------
	IAnimSequence * DivaAnimationNode::GetSequence() { return this->m_pSequence; }
	void DivaAnimationNode::SetSequence(IAnimSequence * sequence) {
		if (this->m_pSequence || !sequence) {
			if (this->m_pFader) this->m_pFader->Release();
			this->m_pFader = nullptr;
		}

		this->m_pSequence = sequence;

		if (this->m_pSequence) {
			this->m_pFader = this->m_pSequence->CreateNode(EAnimNodeType::eAnimNodeType_ScreenFader);
			this->m_pFader->CreateDefaultTracks();
			this->m_pFader->AddRef();
			IAnimTrack * faderTrack = this->m_pFader->GetTrackByIndex(0);
			faderTrack->SetNumKeys(2);
		}
	}
	void DivaAnimationNode::SetNodeOwner(IAnimNodeOwner * pOwner) {
		this->m_pOwner = pOwner;

		if (pOwner) {
			pOwner->OnNodeAnimated(this);
		}
	}
	IAnimNodeOwner * DivaAnimationNode::GetNodeOwner() { return this->m_pOwner; }
	void DivaAnimationNode::SetParent(IAnimNode * pParent) { this->m_pParentNode = pParent; }
	IAnimNode * DivaAnimationNode::GetParent() const { return this->m_pParentNode; }
	IAnimNode * DivaAnimationNode::HasDirectorAsParent() const {
		IAnimNode* pParent = GetParent();
		while (pParent) {
			if (pParent->GetType() == eAnimNodeType_Director) {
				return pParent;
			}
			// There are some invalid data.
			if (pParent->GetParent() == pParent) {
				pParent->SetParent(nullptr);
				return nullptr;
			}
			pParent = pParent->GetParent();
		}
		return nullptr;
	}

	void DivaAnimationNode::GetMemoryUsage(ICrySizer * pSizer) const {
		for (DivaNoteBaseNode* n : m_NoteNodes) pSizer->AddObject(n);
		for (ParticleBase* p : m_Particles) pSizer->AddObject(p);
	}

	//-----------------------------------------------------------------------------
	// event funcs
	//-----------------------------------------------------------------------------
	void DivaAnimationNode::ZonesEvents(void* userdata) {
		AZStd::string sparam((const char*)userdata);

		if (sparam.compare("ChanceEnter") == 0) EBUS_EVENT(DivaHudChanceEventsBus, OnChanceEnter);
		else if (sparam.compare("ChanceExit") == 0) EBUS_EVENT(DivaHudChanceEventsBus, OnChanceExit);
		else if (sparam.compare("TechEnter") == 0) EBUS_EVENT(DivaHudTechEventsBus, OnTechEnter);
		else if (sparam.compare("TechExit") == 0) EBUS_EVENT(DivaHudTechEventsBus, OnTechExit);
	}

	void DivaAnimationNode::LyricEvents(void* userdata) {
		LyricsFile::LineEntry* entry = static_cast<LyricsFile::LineEntry*>(userdata);
		if (entry) EBUS_EVENT(DivaHudLyricsBus, SetLyrics, entry->text);
	}

	void DivaAnimationNode::SubtextEvents(void* userdata) {
		LyricsFile::SubtextEntry* entry = static_cast<LyricsFile::SubtextEntry*>(userdata);
		if (entry) EBUS_EVENT(DivaSequenceEffectsBus, PushbackSubtext, entry);
	}

	void DivaAnimationNode::SongEvents(void* userdata) {
		AZStd::string sparam((const char*)userdata);
		
		if (sparam.compare("Start") == 0) {
			EBUS_EVENT(DivaEventsBus, OnSongStart);
		} else if (sparam.compare("End") == 0) {
			EBUS_EVENT(DivaEventsBus, OnSongEnd);
		} else if (sparam.compare("AudioStart") == 0) {
			EBUS_EVENT(DivaEventsBus, OnSongAudioStart);
		} else if (sparam.compare("AudioEnd") == 0) {
			EBUS_EVENT(DivaAudioEventsBus, OnAudioEnd);
			EBUS_EVENT(DivaEventsBus, OnSongAudioEnd);
		}
	}

	//-----------------------------------------------------------------------------
	// DivaAudioEventsBus
	//-----------------------------------------------------------------------------
	void DivaAnimationNode::OnAudioStart() {
		if (this->m_Music.m_type) {
			EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, Queue, true);
			if (this->m_Music.m_idMelody == -1 && this->m_Music.m_audioMelody)
				EBUS_EVENT_RESULT(this->m_Music.m_idMelody, AlternativeAudio::AlternativeAudioDeviceBus, PlaySource, this->m_Music.m_audioMelody);
			if (this->m_Music.m_idVocal == -1 && this->m_Music.m_audioVocal)
				EBUS_EVENT_RESULT(this->m_Music.m_idVocal, AlternativeAudio::AlternativeAudioDeviceBus, PlaySource, this->m_Music.m_audioVocal);
			EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, Queue, false);
		} else {
			if (this->m_Music.m_idSong == -1 && this->m_Music.m_audioSong)
				EBUS_EVENT_RESULT(this->m_Music.m_idSong, AlternativeAudio::AlternativeAudioDeviceBus, PlaySource, this->m_Music.m_audioSong);
		}
	}
	void DivaAnimationNode::OnAudioEnd() {
		if (this->m_Music.m_type) {
			EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, Queue, true);
			if (this->m_Music.m_idMelody != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, StopSource, this->m_Music.m_idMelody);
			if (this->m_Music.m_idVocal != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, StopSource, this->m_Music.m_idVocal);
			EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, Queue, false);

			this->m_Music.m_idMelody = this->m_Music.m_idVocal = -1;
		} else {
			if (this->m_Music.m_idSong != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, StopSource, this->m_Music.m_idSong);
			this->m_Music.m_idSong = -1;
		}
	}
	void DivaAnimationNode::OnAudioPause() {
		if (this->m_Music.m_type) {
			EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, Queue, true);
			if (this->m_Music.m_idMelody != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, PauseSource, this->m_Music.m_idMelody);
			if (this->m_Music.m_idVocal != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, PauseSource, this->m_Music.m_idVocal);
			EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, Queue, false);
		} else {
			if (this->m_Music.m_idSong != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, PauseSource, this->m_Music.m_idSong);
		}
	}
	void DivaAnimationNode::OnAudioResume() {
		if (this->m_Music.m_type) {
			EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, Queue, true);
			if (this->m_Music.m_idMelody != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, ResumeSource, this->m_Music.m_idMelody);
			if (this->m_Music.m_idVocal != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, ResumeSource, this->m_Music.m_idVocal);
			EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, Queue, false);
		} else {
			if (this->m_Music.m_idSong != -1)
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, ResumeSource, this->m_Music.m_idSong);
		}
	}
	//-----------------------------------------------------------------------------
}