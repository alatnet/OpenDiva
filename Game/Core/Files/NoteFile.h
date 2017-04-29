#ifndef _H_NOTEFILE_
#define _H_NOTEFILE_
#pragma once

#include "NoteFileEntries.h"
#include "string"

namespace LYGame {
	class NoteFile {
	public:
		struct NoteFileInfo {
			string author;
			unsigned int version, difficulty;
			string desc;
			bool valid;
			NoteFileInfo() :
				author(""),
				version(1),
				difficulty(0),
				desc(""),
				valid(false) {
			}

			void GetMemoryUsage(ICrySizer* pSizer) const {
				pSizer->AddObject(this, sizeof(*this));
			}
		};
	public:
		NoteFile(const char * filename);
	public:
		static NoteFileInfo GetInfo(const char * filename);
		//static NoteFile * Open(const char * filename) { return new NoteFile(filename); }
	public:
		~NoteFile();
		NoteFileInfo GetInfo() { return this->m_FileInfo; }
		unsigned int GetNumNotes() { return this->m_Notes.size(); }
		NoteEntry* GetNote(unsigned int index) { return this->m_Notes[index]; }
		string toString();
	private:
		static NoteFileInfo GetInfo(XmlNodeRef xmlNode);
		static bool noteSort(NoteEntry* a, NoteEntry* b);
	private:
		std::vector<NoteEntry*> m_Notes;
		NoteFileInfo m_FileInfo;
	};
}

#endif //_H_NOTEFILE_