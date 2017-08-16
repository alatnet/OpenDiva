#ifndef _H_NOTEFILEENTRYFACTORY
#define _H_NOTEFILEENTRYFACTORY
#pragma once

namespace OpenDiva {
	class NoteEntry;

	typedef AZStd::function<NoteEntry* (void)> NewNoteEntryFunc;

	class NoteFileEntryFactory {
	public:
		static NoteFileEntryFactory& getFactory();
	public:
		NoteEntry* newNoteEntry(std::string name);
		std::vector<std::string>& getNoteEntries() { return *(this->m_names); }

	public:
		void registerNoteEntry(std::string name, NewNoteEntryFunc ptr);

	private:
		NoteFileEntryFactory();
		~NoteFileEntryFactory();
		NoteFileEntryFactory(const NoteFileEntryFactory&) = delete;
		NoteFileEntryFactory& operator= (const NoteFileEntryFactory&) = delete;
	private:
		std::unordered_map<std::string, NewNoteEntryFunc> *m_funcs;
		std::vector<std::string> *m_names;
	};

	#define REGISTER_NOTE_ENTRY(tag,entry) \
		public: \
			static NoteEntry* new##entry##FactoryReg() { return new entry##(); }\
			static void Register##entry##ToFactory() {\
				static_constructor<&##entry::Register##entry##ToFactory>::c;\
				NoteFileEntryFactory::getFactory().registerNoteEntry(##tag##, &##entry##::new##entry##FactoryReg);\
			}
}

#endif