#include <StdAfx.h>
#include "NoteFile.h"

namespace LYGame {
	NoteFile::NoteFile(const char * filename) {
		CryLog("(NoteFile) Reading File: %s",filename);

		XmlNodeRef xmlFile = gEnv->pSystem->LoadXmlFromFile(filename);

		if (xmlFile == 0) return;

		this->m_FileInfo = NoteFile::GetInfo(xmlFile);

		if (!this->m_FileInfo.valid) return;

		int numNotes = xmlFile->getChildCount();
		CryLog("(NoteFile) Number of Entries: %i", numNotes);

		#pragma omp parallel for
		for (int i = 0; i < numNotes; i++) {
			XmlNodeRef child = xmlFile->getChild(i);
			string tag = child->getTag();

			NoteEntrySerializeError error;
			NoteEntry* note = NoteFileEntryFactory::getFactory().newNoteEntry(tag.c_str());

			if (note != nullptr) {
				error = note->Serialize(child);

				//switch (note->getClassType()) { //what to do based on class type
				//default: //grab the id of the entry.
					if (child->haveAttr("id")) {
						child->getAttr("id", note->id);
					}
					else {
						string err = "cannot find id attribute";
						err += " - Line: " + child->getLine();
						error.errors.push_back(err);
						error.malformed = true; //error checking
					}
				//	break;
				//}
			} else if (tag.compare("description")==0){
				//do nothing
			} else {
				string err;
				err = "invalid notemap tag";
				err += " - Line: " + child->getLine();
				error.errors.push_back(err);
				error.malformed = true;
			}

			//we have multiple threads but std::vector isnt thread safe.
			#pragma omp critical
			{
				if (!error.malformed && note != nullptr) { //if there is no errors
					//switch (note->getClassType()) { //based on class type
					//default: //otherwise
						this->m_Notes.push_back(note); //add this note to the list of notes
					//	break;
					//}
			    } else if (tag.compare("description")==0){
					//do nothing
				} else {
					if (note != nullptr) {
						CryLog("Malformed notemap tag. \n- ID: %u\n- Note Num: %i", note->id, i);
						delete note;
					} else {
						CryLog("Malformed notemap tag. \n- Note Num: %i", i);
					}
					//log the errors
					while (!error.errors.empty()) { //while we are not empty
						string str = error.errors.at(error.errors.size() - 1); //get the last error
						error.errors.pop_back(); //delete the last error
						CryLog("- %s",str.c_str()); //log the error
					}
				}
			}
		}

		//sort the notes
		std::stable_sort(this->m_Notes.begin(), this->m_Notes.end(), NoteFile::noteSort);
                
        if (this->m_Notes.at(0)->getClassType() != eNHT_BPM) {
            CryLog("Warning! First entry in notefile is not a bpm entry!");
        }
	}

	NoteFileInfo NoteFile::GetInfo(const char * filename) {
		//TODO: Error checking needed
		XmlNodeRef xmlFile = gEnv->pSystem->LoadXmlFromFile(filename);

		NoteFileInfo ret;

		if (xmlFile != 0) {
			xmlFile->getAttr("version", ret.version);
			xmlFile->getAttr("difficulty", ret.difficulty);
			ret.author = xmlFile->getAttr("author");

			XmlNodeRef desc = xmlFile->findChild("desc");
			if (desc != 0) ret.desc = desc->getContent();

			ret.valid = true;
		}

		return ret;
	}

	NoteFileInfo NoteFile::GetInfo(XmlNodeRef xmlNode) {
		//TODO: Error checking needed
		NoteFileInfo ret;

		if (xmlNode != 0) {
			xmlNode->getAttr("version", ret.version);
			xmlNode->getAttr("difficulty", ret.difficulty);
			ret.author = xmlNode->getAttr("author");

			XmlNodeRef desc = xmlNode->findChild("desc");
			if (desc != 0) ret.desc = desc->getContent();

			ret.valid = true;
		}

		return ret;
	}

	NoteFile::~NoteFile() {
		/*while (this->m_Notes.size() > 0) {
			NoteEntry* note = this->m_Notes.at(this->m_Notes.size() - 1);
			this->m_Notes.pop_back();
			delete note;
		}*/
		this->m_Notes.clear(); //calls deconstructor.
	} 

	//sort notes by time;
	bool NoteFile::noteSort(NoteEntry* a, NoteEntry* b) {
		return a->getTime() < b->getTime();
	}

	string NoteFile::toString() {
		string ret = "";

		ret += "Author: ";
		ret += this->m_FileInfo.author;
		ret += "\nVersion: " + string(std::to_string(this->m_FileInfo.version).c_str());
		ret += "\nDifficulty: " + string(std::to_string(this->m_FileInfo.difficulty).c_str());
		ret += "\nNotes:\n";

		for (NoteEntry* e : this->m_Notes) {
			ret += e->toString() += "\n";
		}

		return ret;
	}
}