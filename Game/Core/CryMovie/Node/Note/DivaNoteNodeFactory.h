#ifndef _H_DIVANOTENODEFACTORY
#define _H_DIVANOTENODEFACTORY
#pragma once

namespace LYGame {

	class DivaNoteBaseNode;

	typedef std::function<DivaNoteBaseNode* (ResourceCollection *rc, IDivaJudge *divaJudge, NoteEntry* entry)> NewDivaNoteNodeFunc;

	//singleton factory
	class DivaNoteNodeFactory {
	public:
		static DivaNoteNodeFactory& getFactory();
	public:
		DivaNoteBaseNode* newNoteNode(std::string name, ResourceCollection *rc, IDivaJudge *divaJudge, NoteEntry* entry);
		std::vector<std::string>& getNoteNodes() { return *(this->m_names); }

	public:
		void registerDivaNoteNode(std::string name, NewDivaNoteNodeFunc ptr);

	private:
		DivaNoteNodeFactory();
		~DivaNoteNodeFactory();
		DivaNoteNodeFactory(const DivaNoteNodeFactory&) = delete;
		DivaNoteNodeFactory& operator= (const DivaNoteNodeFactory&) = delete;
	private:
		std::unordered_map<std::string, NewDivaNoteNodeFunc> *m_funcs;
		std::vector<std::string> *m_names;
	};

	#define REGISTER_DIVA_NOTE_NODE(name,notenode,fileentry) \
		public: \
			static DivaNoteBaseNode* new##notenode##FactoryReg(ResourceCollection *rc, IDivaJudge *divaJudge, NoteEntry* entry) { return new notenode##(rc,divaJudge,static_cast<##fileentry##*>(entry)); }\
			static void Register##notenode##ToFactory() {\
				static_constructor<&##notenode::Register##notenode##ToFactory>::c;\
				DivaNoteNodeFactory::getFactory().registerDivaNoteNode(name, &##notenode##::new##notenode##FactoryReg);\
			}
}

#endif