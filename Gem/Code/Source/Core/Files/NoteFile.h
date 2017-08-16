#ifndef _H_NOTEFILE_
#define _H_NOTEFILE_
#pragma once

#include "NoteFileEntries.h"

namespace OpenDiva {
	class NoteFile {
	public:
		struct NoteFileInfo {
			AZStd::string author;
			unsigned int version, difficulty;
			AZStd::string desc;
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
		AZStd::string toString();
	private:
		static NoteFileInfo GetInfo(XmlNodeRef xmlNode);
		static bool noteSort(NoteEntry* a, NoteEntry* b);
	private:
		AZStd::vector<NoteEntry*> m_Notes;
		NoteFileInfo m_FileInfo;
	};
}

#endif //_H_NOTEFILE_