#ifndef _H_NOTEFILE_
#define _H_NOTEFILE_
#pragma once

#include "NoteFileEntries.h"

namespace OpenDiva {
	class NoteFile {
	public:
		struct Info {
			AZ::Crc32 hash;
			AZStd::string author;
			unsigned int version, difficulty;
			AZStd::string desc;
			bool valid;
			Info() :
				author(""),
				version(1),
				difficulty(0),
				desc(""),
				valid(false) {}

			void GetMemoryUsage(ICrySizer* pSizer) const {
				pSizer->AddObject(this, sizeof(*this));
			}
		};
	public:
		NoteFile(const char * filename);
	public:
		static Info GetInfo(const char * filename);
		//static NoteFile * Open(const char * filename) { return new NoteFile(filename); }
	public:
		~NoteFile();
		Info GetInfo() { return this->m_FileInfo; }
		unsigned int GetNumNotes() { return this->m_Notes.size(); }
		NoteEntry* GetNote(unsigned int index) { return this->m_Notes[index]; }
		AZStd::string toString();
	private:
		static Info GetInfo(XmlNodeRef xmlNode);
		static bool noteSort(NoteEntry* a, NoteEntry* b);
	private:
		AZStd::vector<NoteEntry*> m_Notes;
		Info m_FileInfo;
	};
}

#endif //_H_NOTEFILE_