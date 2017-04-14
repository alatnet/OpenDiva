#include <StdAfx.h>
#include "DivaSequence.h"

namespace LYGame {
	//-----------------------------------------------------------------------
	//DivaSequence Stuff
	//-----------------------------------------------------------------------
	DivaSequence::DivaSequence(ResourceCollection * rc) : CAnimSequence(gEnv->pSystem->GetIMovieSystem(), gEnv->pSystem->GetIMovieSystem()->GrabNextSequenceId()), m_pRC(rc) {
		this->m_hitNote = this->m_renderRangeStart = this->m_renderRangeEnd = 0;
		this->SetFlags(IAnimSequence::eSeqFlags_NoAbort /*| IAnimSequence::eSeqFlags_CutScene*/ | IAnimSequence::eSeqFlags_OutOfRangeConstant | IAnimSequence::eSeqFlags_NoMPSyncingNeeded | IAnimSequence::eSeqFlags_NoSpeed);
		this->SetName("DivaSequence");

		//use an anim node to call DivaSequences Animate and Render functions.
		this->m_DivaSeqRedirect = new DivaSequenceRedirect(this);
		this->m_DivaSeqRedirect->AddRef();
		this->AddNode(this->m_DivaSeqRedirect);

		/*
		//first event track
		IAnimNode * node = this->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		node->CreateDefaultTracks();
		IAnimTrack * track = node->GetTrackByIndex(0);
		IEventKey key;
		key.event = "TestEvent";
		key.eventValue = "TEST";
		key.time = 3;
		int keyindex = track->CreateKey(3);
		track->SetKey(keyindex, &key);

		key.eventValue = "TEST2";
		key.time = 3;
		keyindex = track->CreateKey(3);
		track->SetKey(keyindex, &key);

		key.eventValue = "TEST3";
		key.time = 8;
		keyindex = track->CreateKey(8);
		track->SetKey(keyindex, &key);

		//second event track
		node = this->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		node->CreateDefaultTracks();
		track = node->GetTrackByIndex(0);
		key.event = "TestEvent2";
		key.eventValue = "TEST";
		key.time = 4;
		keyindex = track->CreateKey(3);
		track->SetKey(keyindex, &key);

		key.eventValue = "TEST2";
		key.time = 5;
		keyindex = track->CreateKey(6);
		track->SetKey(keyindex, &key);

		key.eventValue = "TEST3";
		key.time = 7;
		keyindex = track->CreateKey(8);
		track->SetKey(keyindex, &key);
		*/

		//setup the zone events track
		this->m_pZoneEvents = this->CreateNode(EAnimNodeType::eAnimNodeType_Event);
		this->m_pZoneEvents->CreateDefaultTracks();
	}

	DivaSequence::~DivaSequence() {
		//delete the redirect
		this->RemoveNode(this->m_DivaSeqRedirect);
		delete this->m_DivaSeqRedirect;

		//delete the notes
		while (this->m_NoteNodes.size() > 0) {
			DivaNoteBaseNode* node = this->m_NoteNodes.back();
			this->m_NoteNodes.pop_back();
			//this->RemoveNode(node);
			node->Release();
		}
		this->m_NoteNodes.clear();
		
		//clear all particles
		this->m_Particles.clear();
	}

	bool DivaSequence::Init(NoteFile *noteFile, IDivaJudge * judge) {
		unsigned int numNotes = noteFile->GetNumNotes();

		NoteEntryBPM * currBPM = nullptr;

		judge->setDivaSequence(this);

		if (noteFile->GetNote(0)->getClassType() == eNHT_BPM) {
			currBPM = (NoteEntryBPM*)(noteFile->GetNote(0));

			float endTime = 0;
			SectionType sectionEvent = eST_Norm;
			unsigned int techZoneNotes = 0;
			std::vector<unsigned int> techZoneNotesVector;

			IAnimTrack * zoneTrack = m_pZoneEvents->GetTrackByIndex(0);

			for (int i = 1; i < numNotes; i++) {
				NoteEntry *currNote = noteFile->GetNote(i);

				if (currNote->getClassType() == eNHT_BPM){
					currBPM = (NoteEntryBPM*)currNote;
				} else {
					DivaNoteBaseNode* node = DivaNoteNodeFactory::getFactory().newNoteNode(currNote->getTagName(), this->m_pRC, judge, currNote); //create a new diva note
					if (node != nullptr) { // if the note was created ok
						node->AddRef(); //add a reference (smart pointer thing)
						node->Init(*currBPM); //initialize it with the current bpm
						node->setDivaSeqEff(this); //set the diva sequence effect
						this->m_NoteNodes.push_back(node); //add this note to the list of notes to render
						if (endTime < node->GetTimeRange().end) endTime = node->GetTimeRange().end; //check end times

						techZoneNotes++; //increase the number of tech zone notes

						//set zone events
						if (currNote->sType != sectionEvent) { //if the section is different
							//setup the sequence event
							IEventKey key;
							key.event = "ZoneEvent";

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
							} else if (currNote->sType == eST_Norm && sectionEvent == eST_Chance) { //if we are exiting chancetime and entering normal
								key.eventValue = "ChanceExit";
								techZoneNotes = 0;
							}else if (currNote->sType == eST_Tech && sectionEvent == eST_Chance) { //if we are swaping zones (chance to tech)
								key.eventValue = "ChanceExit";
								techZoneNotes = 0;

								IEventKey key2;
								key2.eventValue = "TechEnter";
								key2.time = key.time-2;
								int key2index = zoneTrack->CreateKey(key2.time);
								zoneTrack->SetKey(key2index, &key2);
							} else if (currNote->sType == eST_Chance && sectionEvent == eST_Tech) { //if we are swaping zones (tech to chance)
								key.eventValue = "TechExit";
								techZoneNotesVector.push_back(techZoneNotes);
								techZoneNotes = 0;

								IEventKey key2;
								key2.eventValue = "ChanceEnter";
								key2.time = key.time-2;
								int key2index = zoneTrack->CreateKey(key2.time);
								zoneTrack->SetKey(key2index, &key2);
							}

							int keyindex = zoneTrack->CreateKey(key.time);
							zoneTrack->SetKey(keyindex, &key);

							sectionEvent = currNote->sType;
						}
					}
				}
			}

			//make sure we exit out of a zone
			if (sectionEvent != eST_Norm) {
				IEventKey key;
				key.event = "ZoneEvent";
				key.time = endTime;

				if (sectionEvent == eST_Tech) {
					key.eventValue = "TechExit";
					techZoneNotesVector.push_back(techZoneNotes);
				} else {
					key.eventValue = "ChanceExit";
				}

				int keyindex = zoneTrack->CreateKey(key.time);
				zoneTrack->SetKey(keyindex, &key);
			}

			judge->setTotalNotes(this->m_NoteNodes.size());
			EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaCompletionBus, SetTotalNotes, this->m_NoteNodes.size());

			judge->SetTechZoneNotes(techZoneNotesVector);
			
			//find effect animation time and whichever biggest ending time is, tack it onto the end time of note nodes.
			this->SetTimeRange(Range(0, endTime + 10));
		}
		else {
			//error out
			//first note should ALWAYS be bpm note
			return false;
		}

		return true;
	}

	void DivaSequence::Animate(SAnimContext &ec) {
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
					this->m_Particles.erase(std::remove(m_Particles.begin(), m_Particles.end(), ep), m_Particles.end());
					i--;
					delete ep;
				}
			}
		}
	}

	void DivaSequence::Render() {
		OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
			//render particles
			if (this->m_Particles.size() > 0) for (ParticleBase* particle : this->m_Particles) particle->Render();

			//render nodes
			if (this->m_NoteNodes.size() > 0)
				for (int i = this->m_renderRangeEnd; i != this->m_renderRangeStart-1; i--)
					this->m_NoteNodes[i]->Render(); //reverse render so that current note to hit is on top.
		OD_Draw2d::getDraw2D().EndDraw2d();
	}


	void DivaSequence::Reset(bool bSeekToStart) {
		CAnimSequence::Reset(bSeekToStart);

		if (this->m_NoteNodes.size() > 0) for (std::vector<DivaNoteBaseNode *>::iterator it1 = this->m_NoteNodes.begin(); it1 != this->m_NoteNodes.end(); it1++) (*it1)->Reset();
		this->m_Particles.clear();

		this->m_renderRangeEnd = this->m_renderRangeStart = this->m_hitNote = 0;
	}

	void DivaSequence::ResetHard() {
		CAnimSequence::ResetHard();
	}

	//potential use
	void DivaSequence::TriggerTrackEvent(const char* event, const char* param) {
		CAnimSequence::TriggerTrackEvent(event, param);
	}

	//particle stuff
	void DivaSequence::pushbackRating(Vec2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong) {
		this->m_Particles.push_back(new RatingParticle(this->m_pRC, pos, nType, hitscore, combo, wrong, RATINGPARTICLE_LIFE));
	}

	void DivaSequence::pushbackEffect(Vec2 pos, EEffectList eff) {
		this->m_Particles.push_back(new EffectParticle(this->m_pRC, pos, eff));
	}

	void DivaSequence::pushbackHoldMulti(Vec2 pos, unsigned int score, EHitScore hitscore, bool wrong) {
		this->m_Particles.push_back(new HoldMultiParticle(this->m_pRC, pos, score, hitscore, wrong));
	}

	void DivaSequence::updateNoteHit() {
		this->m_hitNote++; //we are done scoring this note, on to the next one.
		if (this->m_hitNote >= this->m_NoteNodes.size()) this->m_hitNote = this->m_NoteNodes.size() - 1;
	}

	//Input
	void DivaSequence::OnCross(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnCross(mode, value);
	}

	void DivaSequence::OnCircle(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnCircle(mode, value);
	}

	void DivaSequence::OnSquare(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnSquare(mode, value);
	}

	void DivaSequence::OnTriangle(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnTriangle(mode, value);
	}

	void DivaSequence::OnLeft(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnLeft(mode, value);
	}

	void DivaSequence::OnRight(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnRight(mode, value);
	}

	void DivaSequence::OnUp(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnUp(mode, value);
	}

	void DivaSequence::OnDown(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnDown(mode, value);
	}

	void DivaSequence::OnStar(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnStar(mode, value);
	}

	void DivaSequence::OnSwipeL(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnSwipeL(mode, value);
	}

	void DivaSequence::OnSwipeR(int mode, float value) {
		m_NoteNodes[m_hitNote]->OnSwipeR(mode, value);
	}

	void DivaSequence::GetMemoryUsage(ICrySizer* pSizer) const {
		CAnimSequence::GetMemoryUsage(pSizer);
		pSizer->AddObject(m_NoteNodes);
		pSizer->AddObject(m_Particles);
	}
}