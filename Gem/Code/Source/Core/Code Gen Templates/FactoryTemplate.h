#ifndef _H_{{class.nameCaps}}FACTORY
#define _H_{{class.nameCaps}}FACTORY

#pragma once

namespace OpenDiva {
	class NoteEntry;
	class {{object.name}}

	typedef std::function<{{object.name}}* (void)> New{{object.name}}Func;

	class {{class.name}}Factory {
	public:
		static {{class.name}}Factory& getFactory();
	public:
		{{object.name}}* new{{object.name}}(std::string name);
		std::vector<std::string>& get{{object.namePlural}}() { return *(this->m_names); }

	public:
		void register{{object.name}}(std::string name, New{{object.name}}Func ptr);

	private:
		{{class.name}}Factory();
		~{{class.name}}Factory();
		{{class.name}}Factory(const {{class.name}}Factory&) = delete;
		{{class.name}}Factory& operator= (const {{class.name}}Factory&) = delete;
	private:
		std::unordered_map<std::string, New{{object.name}}Func> *m_funcs;
		std::vector<std::string> *m_names;
	};

	#define REGISTER_{{object.nameCaps}}(tag,entry) \
		public: \
			static {{object.name}}* new##entry##FactoryReg() { return new entry##(); }\
			static void Register##entry##ToFactory() {\
				static_constructor<&##entry::Register##entry##ToFactory>::c;\
				{{class.name}}Factory::getFactory().register{{object.name}}(##tag##, &##entry##::new##entry##FactoryReg);\
			}
}

#endif