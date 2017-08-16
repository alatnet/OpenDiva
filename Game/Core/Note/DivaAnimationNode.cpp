#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "DivaAnimationNode.h"

namespace LYGame {
	//-----------------------------------------------------------------------
	// DivaAnimationNode
	//-----------------------------------------------------------------------
	DivaAnimationNode::DivaAnimationNode(ResourceCollection * rc) :
		m_pRC(rc),
		m_pSequence(nullptr),
		m_pOwner(nullptr),
		m_pParentNode(nullptr),
		m_pEvents(nullptr)
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
		//this->m_pZoneEvents = this->m_seq->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		//this->m_pZoneEvents->CreateDefaultTracks();

		//setup the lyrics events track
		//this->m_pLyricsEvents = this->m_seq->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		//this->m_pLyricsEvents->CreateDefaultTracks();

		/*IAnimSequence* seq = gEnv->pMovieSystem->CreateSequence("",false,0U,eSequenceType_SequenceComponent);
		IAnimNode * events = seq->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		IAnimNode * ent = seq->CreateNode(EAnimNodeType::eAnimNodeType_AzEntity);*/

		DivaSequenceJudgeBus::Handler::BusConnect();
		DivaSequenceEffectsBus::Handler::BusConnect();
	}

	DivaAnimationNode::~DivaAnimationNode() {
		DivaSequenceJudgeBus::Handler::BusDisconnect();
		DivaSequenceEffectsBus::Handler::BusDisconnect();

		//delete the notes
		/*while (this->m_NoteNodes.size() > 0) {
			DivaNoteBaseNode* node = this->m_NoteNodes.back();
			this->m_NoteNodes.pop_back();
			node->Release();
		}*/

		for (DivaNoteBaseNode * n : this->m_NoteNodes) n->Release();
		this->m_NoteNodes.clear();

		//clear all particles
		this->m_Particles.clear();
	}

	bool DivaAnimationNode::Init(NoteFile *noteFile) {
		unsigned int numNotes = noteFile->GetNumNotes();

		NoteEntryBPM * currBPM = nullptr;

		if (noteFile->GetNote(0)->getClassType() == eNHT_BPM) {
			currBPM = (NoteEntryBPM*)(noteFile->GetNote(0));

			float endTime = 0;
			SectionType sectionEvent = eST_Norm;
			unsigned int techZoneNotes = 0;
			AZStd::vector<unsigned int> techZoneNotesVector;

			IAnimTrack * eventTrack = this->m_pEvents->GetTrackByIndex(0);

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
						if (endTime < node->GetTimeRange().end) endTime = node->GetTimeRange().end; //check end times

						techZoneNotes++; //increase the number of tech zone notes

						//set zone events
						if (currNote->sType != sectionEvent) { //if the section is different
							//setup the sequence event
							IEventKey key;
							key.event = "Zone";

							//key.time = currNote->getTime(); //get the note time for the event
							key.time = node->GetTimeRange().start;

							//set event
							if (currNote->sType == eST_Norm && sectionEvent == eST_Tech) { //if we are exiting tech zone and entering normal
								key.eventValue = "TechExit";
								techZoneNotesVector.push_back(techZoneNotes);
								techZoneNotes = 0;
							} else if (currNote->sType == eST_Tech && sectionEvent == eST_Norm) { //if we are entering tech zone
								key.eventValue = "TechEnter";
								key.time -= 2;
								techZoneNotes = 0;
							} else if (currNote->sType == eST_Chance && sectionEvent == eST_Norm) { //if we are entering chance time zone
								key.eventValue = "ChanceEnter";
								key.time -= 2;
								techZoneNotes = 0;
							} else if (currNote->sType == eST_Norm && sectionEvent == eST_Chance) { //if we are exiting chance time and entering normal
								key.eventValue = "ChanceExit";
								techZoneNotes = 0;
							} else if (currNote->sType == eST_Tech && sectionEvent == eST_Chance) { //if we are swaping zones (chance to tech)
								key.eventValue = "ChanceExit";
								techZoneNotes = 0;

								IEventKey key2;
								key2.eventValue = "TechEnter";
								key2.time = key.time - 2;
								int key2index = eventTrack->CreateKey(key2.time);
								eventTrack->SetKey(key2index, &key2);
							} else if (currNote->sType == eST_Chance && sectionEvent == eST_Tech) { //if we are swaping zones (tech to chance)
								key.eventValue = "TechExit";
								techZoneNotesVector.push_back(techZoneNotes);
								techZoneNotes = 0;

								IEventKey key2;
								key2.eventValue = "ChanceEnter";
								key2.time = key.time - 2;
								int key2index = eventTrack->CreateKey(key2.time);
								eventTrack->SetKey(key2index, &key2);
							}

							int keyindex = eventTrack->CreateKey(key.time);
							eventTrack->SetKey(keyindex, &key);

							sectionEvent = currNote->sType;
						}
					}
				}
			}

			//make sure we exit out of a zone
			if (sectionEvent != eST_Norm) {
				IEventKey key;
				key.event = "Zone";
				key.time = endTime;

				if (sectionEvent == eST_Tech) {
					key.eventValue = "TechExit";
					techZoneNotesVector.push_back(techZoneNotes);
				} else {
					key.eventValue = "ChanceExit";
				}

				int keyindex = eventTrack->CreateKey(key.time);
				eventTrack->SetKey(keyindex, &key);
			}

			EBUS_EVENT(DivaJudgeBus, SetTotalNotes, this->m_NoteNodes.size());
			//EBUS_EVENT(OpenDivaBus::OpenDivaCompletionBus, SetTotalNotes, this->m_NoteNodes.size());
			EBUS_EVENT(DivaHudBus, SetCompletion, 0, this->m_NoteNodes.size());
			EBUS_EVENT(DivaJudgeBus, SetTechZoneNotes, techZoneNotesVector);

			//find effect animation time and whichever biggest ending time is, tack it onto the end time of note nodes.
			//this->m_seq->SetTimeRange(Range(0, endTime + 10));
			this->m_pSequence->SetTimeRange(Range(0, endTime + 10));
			this->SetTimeRange(Range(0, endTime + 10));
		} else {
			//error out
			//first note should ALWAYS be bpm note
			return false;
		}

		return true;
	}

	bool DivaAnimationNode::InitLyrics(LyricsFile * lyrics) {
		IAnimTrack * eventTrack = m_pEvents->GetTrackByIndex(0);

		//this->m_seq->AddTrackEvent("lyric");
		//this->m_seq->AddTrackEvent("translation");
		//this->m_seq->AddTrackEventListener(nullptr);

		//IEventKey key;
		//key.event = "Lyrics";
		//key.eventValue = "1"; //lyric number
							   //key.eventValue = "lyric";
							   //key.eventValue = "translation";
							   //key.value = 1; //which lyric/translation
		//key.time = 0;

		//int keyindex = eventTrack->CreateKey(key.time);
		//eventTrack->SetKey(keyindex, &key);

		for (unsigned int i = 0; i < lyrics->GetNumLines(); i++) {
			LyricsFile::LineEntry entry = lyrics->GetLine(i);
			IEventKey key;
			key.event = "Lyrics";
			key.eventValue = entry.text.c_str();
			key.time = entry.time;

			int keyindex = eventTrack->CreateKey(key.time);
			eventTrack->SetKey(keyindex, &key);

			key.event = "Romaji";
			key.eventValue = entry.romaji.c_str();
			keyindex = eventTrack->CreateKey(key.time);
			eventTrack->SetKey(keyindex, &key);
		}
		return true;
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

	void DivaAnimationNode::UpdateNoteHit() {
		this->m_hitNote++; //we are done scoring this note, on to the next one.
		if (this->m_hitNote >= this->m_NoteNodes.size()) this->m_hitNote = this->m_NoteNodes.size() - 1;
	}

	//-----------------------------------------------------------------------
	//Input
	//-----------------------------------------------------------------------
	void DivaAnimationNode::OnCross(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnCross(mode, value); }
	void DivaAnimationNode::OnCircle(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnCircle(mode, value); }
	void DivaAnimationNode::OnSquare(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnSquare(mode, value); }
	void DivaAnimationNode::OnTriangle(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnTriangle(mode, value); }
	void DivaAnimationNode::OnLeft(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnLeft(mode, value); }
	void DivaAnimationNode::OnRight(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnRight(mode, value); }
	void DivaAnimationNode::OnUp(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnUp(mode, value); }
	void DivaAnimationNode::OnDown(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnDown(mode, value); }
	void DivaAnimationNode::OnStar(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnStar(mode, value); }
	void DivaAnimationNode::OnSwipeL(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnSwipeL(mode, value); }
	void DivaAnimationNode::OnSwipeR(LyInputEventType mode, float value) { m_NoteNodes[m_hitNote]->OnSwipeR(mode, value); }

	//-----------------------------------------------------------------------
	// CAnimNode has these.
	//-----------------------------------------------------------------------
	void DivaAnimationNode::OnStart() {}
	void DivaAnimationNode::OnReset() {
		if (this->m_NoteNodes.size() > 0) for (AZStd::vector<DivaNoteBaseNode *>::iterator it1 = this->m_NoteNodes.begin(); it1 != this->m_NoteNodes.end(); it1++) (*it1)->Reset();
		this->m_Particles.clear();

		this->m_renderRangeEnd = this->m_renderRangeStart = this->m_hitNote = 0;
	}
	void DivaAnimationNode::OnResetHard() { OnReset(); }
	void DivaAnimationNode::OnPause() {}
	void DivaAnimationNode::OnResume() {}
	void DivaAnimationNode::OnStop() {}
	void DivaAnimationNode::OnLoop() {}

	//-----------------------------------------------------------------------
	// Inherited via IAnimNode
	//-----------------------------------------------------------------------
	void DivaAnimationNode::Animate(SAnimContext & ec) {
		if (this->m_NoteNodes.size() > 0) {
			//animate first then shift ranges
			for (unsigned int i = this->m_renderRangeStart; i <= this->m_renderRangeEnd; i++) this->m_NoteNodes[i]->Animate(ec);

			//shift the start range to a note node that needs to render
			while (this->m_NoteNodes[this->m_renderRangeStart]->NeedToRender(ec.time) == false) {
				this->m_renderRangeStart++;

				if (this->m_renderRangeStart >= this->m_renderRangeEnd) {
					this->m_renderRangeStart = this->m_renderRangeEnd;
					break;
				}
			}

			//shift the end range to a note node that doesnt need to render
			while (this->m_NoteNodes[this->m_renderRangeEnd]->NeedToRender(ec.time) == true) {
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
				ParticleBase * ep = this->m_Particles.at(i);
				ep->Tick(ec.dt);
				if (!ep->Alive()) {
					this->m_Particles.erase(AZStd::remove(m_Particles.begin(), m_Particles.end(), ep), m_Particles.end());
					i--;
					delete ep;
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
	// Inherited via ITrackEventListener
	//-----------------------------------------------------------------------
	void DivaAnimationNode::OnTrackEvent(IAnimSequence* pSequence, int reason, const char* event, void* pUserData) {
		if (pSequence != (IAnimSequence*)this) return;

		if (reason == eTrackEventReason_Triggered) {
			AZStd::string sEvent = AZStd::string(event);
			if (sEvent.compare("Zone") == 0) {
				AZStd::string sparam((const char *)pUserData);
				if (sparam.compare("ChanceEnter") == 0) EBUS_EVENT(DivaHudBus, OnChanceEnter);
				else if (sparam.compare("ChanceExit") == 0) EBUS_EVENT(DivaHudBus, OnChanceExit);
				else if (sparam.compare("TechEnter") == 0) EBUS_EVENT(DivaHudBus, OnTechEnter);
				else if (sparam.compare("TechExit") == 0) EBUS_EVENT(DivaHudBus, OnTechExit);
			} else if (sEvent.compare("Lyrics")) EBUS_EVENT(DivaHudBus, SetLyrics, AZStd::string((const char *)pUserData));
			else if (sEvent.compare("Translations")) EBUS_EVENT(DivaHudBus, SetRomaji, AZStd::string((const char *)pUserData));
			else if (sEvent.compare("Romaji")) EBUS_EVENT(DivaHudBus, SetTranslation, AZStd::string((const char *)pUserData));
		}
	}

	//-----------------------------------------------------------------------
	// Inherited via IAnimNode
	//-----------------------------------------------------------------------
	IAnimSequence * DivaAnimationNode::GetSequence() { return this->m_pSequence; }
	void DivaAnimationNode::SetSequence(IAnimSequence * sequence) {
		if (this->m_pSequence || !sequence) {
			if (this->m_pSequence) {
				this->m_pSequence->RemoveTrackEvent("Zone");
				this->m_pSequence->RemoveTrackEvent("Lyrics");
				this->m_pSequence->RemoveTrackEvent("Translations");
				this->m_pSequence->RemoveTrackEvent("Romaji");
				this->m_pSequence->RemoveTrackEventListener(this);
			}

			if (this->m_pEvents) this->m_pEvents->Release();
		}

		this->m_pSequence = sequence;

		if (this->m_pSequence) {
			this->m_pSequence->AddTrackEvent("Zone");
			this->m_pSequence->AddTrackEvent("Lyrics");
			this->m_pSequence->AddTrackEvent("Translations");
			this->m_pSequence->AddTrackEvent("Romaji");
			this->m_pSequence->AddTrackEventListener(this);

			this->m_pEvents = this->m_pSequence->CreateNode(EAnimNodeType::eAnimNodeType_Event);
			this->m_pEvents->CreateDefaultTracks();
			this->m_pEvents->AddRef();
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
		pSizer->AddObject(m_pEvents);
	}
	//-----------------------------------------------------------------------------
}