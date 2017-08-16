#ifndef _H_NOTEFILEENTRIES_
#define _H_NOTEFILEENTRIES_
#pragma once

#include "NoteFileEntryFactory.h"

namespace OpenDiva {
	/*struct CustomPath {
	const char * script;
	NoteVec sPos, ePos;
	};*/ //future expansion

	enum PathType {
		ePT_BCurve,
		ePT_Line/*,
		ePT_Custom*/ //future expansion
	};
        
	enum SectionType {
		eST_Norm,
		eST_Tech,
		eST_Chance
	};

	struct NoteEntrySerializeError {
		bool malformed;
		AZStd::vector<AZStd::string> errors;

		NoteEntrySerializeError() : malformed(false) {}
	};

	//the all encompasing note class
	class NoteEntry {
	public:
		virtual float getTime() { return 0; }
		virtual ENoteHitType getClassType() { return eNHT_INVALID; } //we are an unknown note type
		virtual NoteEntrySerializeError Serialize(XmlNodeRef node) { return NoteEntrySerializeError(); }
		virtual AZStd::string toString() { return ""; }
		virtual AZStd::string getTagName() { return ""; }
	public:
		unsigned int id;
        SectionType sType;
	};
        
    #define NoteEntryClass(x) class NoteEntry##x : public NoteEntry

	//bpm entry
	//<bpm id="" val="60" time="" />
	NoteEntryClass(BPM) {
	REGISTER_NOTE_ENTRY("bpm", NoteEntryBPM)

	public:
		float getTime() { return this->time; }
		float getDelay() { return this->getMSDelay()/1000.0f; } //to seconds
		float getMSDelay() { return ((float)((60000.0f/(float)this->bpm)*4.0f/*(float)this->beats*/)); }
		ENoteHitType getClassType() { return eNHT_BPM; } //we are a bpm entry
		NoteEntrySerializeError Serialize(XmlNodeRef node);
		AZStd::string toString();
		AZStd::string getTagName() { return "bpm"; }
	public:
		unsigned int bpm;
		//unsigned int beats;
		float time;
	};

	//single hit note
	/*
	//section is optional
	<note id="" time="" type="" section="tech/chance">
	  <bcurve pos="x,y" angle="" ctrDist1="0.5" ctrDist2="" />
	</note>
	<note id="" time="" type="" section="tech/chance">
	  <line pos="x,y" angle="" />
	</note>
	*/
	NoteEntryClass(Single) {
	REGISTER_NOTE_ENTRY("note", NoteEntrySingle)

	public:
		float getTime() { return this->time; }
		ENoteHitType getClassType() { return eNHT_Single; } // we are a hit note
		NoteEntrySerializeError Serialize(XmlNodeRef node);
		AZStd::string toString();
		AZStd::string getTagName() { return "note"; }
	public:
		float time;
		ENoteType type;
		PathType pType;

		AZ::Vector2 pos;
		float angle, ctrlDist1, ctrlDist2;
	};

	//hold note
	/*
	//section is optional
	<hold id="" time1="" time2="" type="" section="tech/chance">
	  <bcurve pos="x,y" angle="" ctrDist1="0.5" ctrDist2="" />
	</hold>
	<hold id="" time1="" time2="" type="" section="tech/chance">
	  <line pos="x,y" angle="" />
	</hold>
	*/
	NoteEntryClass(Hold) {
	REGISTER_NOTE_ENTRY("hold", NoteEntryHold)

	public:
		float getTime() { return this->hold1; }
		ENoteHitType getClassType() { return eNHT_Hold; } // we are a hold note
		NoteEntrySerializeError Serialize(XmlNodeRef node);
		AZStd::string toString();
		AZStd::string getTagName() { return "hold"; }
	public:
		float hold1, hold2;
		ENoteType type;
		PathType pType;

		AZ::Vector2 pos;
		float angle, ctrlDist1, ctrlDist2;
	};
        
	#undef NoteEntryClass
}

#endif //_H_NOTEFILEENTRIES_