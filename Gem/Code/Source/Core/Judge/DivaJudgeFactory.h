#ifndef _H_DIVAJUDGEFACTORY
#define _H_DIVAJUDGEFACTORY

#pragma once

namespace OpenDiva {

	typedef AZStd::function<IDivaJudge* (void *)> NewJudgeFunc;

	//singleton factory
	class DivaJudgeFactory {
	public:
		static DivaJudgeFactory& getFactory();
	public:
		IDivaJudge* newJudge(std::string name, void * userData = nullptr);
		std::vector<std::string>& getJudges() { return *(this->m_names); }

	public:
		void registerDivaJudge(std::string name, NewJudgeFunc ptr);

	private:
		DivaJudgeFactory();
		~DivaJudgeFactory();
		DivaJudgeFactory(const DivaJudgeFactory&) = delete;
		DivaJudgeFactory& operator= (const DivaJudgeFactory&) = delete;
	private:
		std::unordered_map<std::string, NewJudgeFunc> *m_funcs;
		std::vector<std::string> *m_names;
	};

	#define REGISTER_DIVA_JUDGE(name,judge) \
		public: \
			static IDivaJudge* new##judge##FactoryReg(void* userdata) { return new judge##(userdata); }\
			static void Register##judge##ToFactory() {\
				static_constructor<&##judge::Register##judge##ToFactory>::c;\
				DivaJudgeFactory::getFactory().registerDivaJudge(name, &##judge##::new##judge##FactoryReg);\
			}
}

#endif